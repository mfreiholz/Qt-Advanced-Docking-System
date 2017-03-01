/*******************************************************************************
** QtAdcancedDockingSystem
** Copyright (C) 2017 Uwe Kindler
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program. If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


//============================================================================
/// \file   DockContainerWidget.cpp
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Implementation of CDockContainerWidget class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockContainerWidget.h"

#include <QEvent>
#include <QList>
#include <QGridLayout>
#include <QSplitter>
#include <QPointer>

#include "DockManager.h"
#include "DockAreaWidget.h"
#include "DockWidget.h"
#include "ads_globals.h"

#include <iostream>

namespace ads
{
static unsigned int zOrderCounter = 0;

/**
 * Helper function to ease insertion of dock area into splitter
 */
static void insertDockAreaIntoSplitter(QSplitter* Splitter, QWidget* widget, bool Append)
{
	if (Append)
	{
		Splitter->addWidget(widget);
	}
	else
	{
		Splitter->insertWidget(0, widget);
	}
}

/**
 * Private data class of CDockContainerWidget class (pimpl)
 */
struct DockContainerWidgetPrivate
{
	CDockContainerWidget* _this;
	QPointer<CDockManager> DockManager;
	unsigned int zOrderIndex = 0;
	QList<CDockAreaWidget*> DockAreas;
	QGridLayout* Layout = nullptr;

	/**
	 * Private data constructor
	 */
	DockContainerWidgetPrivate(CDockContainerWidget* _public);

	/**
	 * Create a new dock area widget and adds it to the list of doc areas
	 */
	CDockAreaWidget* newDockArea()
	{
		auto DockAreaWidget = new CDockAreaWidget(DockManager, _this);
		DockAreas.append(DockAreaWidget);
		return DockAreaWidget;
	}

	/**
	 * Adds dock widget to container and returns the dock area that contains
	 * the inserted dock widget
	 */
	CDockAreaWidget* dockWidgetIntoContainer(DockWidgetArea area, CDockWidget* Dockwidget);

	/**
	 * Adds dock widget to a existing DockWidgetArea
	 */
	CDockAreaWidget* dockWidgetIntoDockArea(DockWidgetArea area, CDockWidget* Dockwidget,
		CDockAreaWidget* TargetDockArea);

	/**
	 * Add dock area to this container
	 */
	void addDockArea(CDockAreaWidget* NewDockWidget, DockWidgetArea area = CenterDockWidgetArea);
}; // struct DockContainerWidgetPrivate


//============================================================================
DockContainerWidgetPrivate::DockContainerWidgetPrivate(CDockContainerWidget* _public) :
	_this(_public)
{

}


//============================================================================
CDockAreaWidget* DockContainerWidgetPrivate::dockWidgetIntoContainer(DockWidgetArea area,
	CDockWidget* Dockwidget)
{
	CDockAreaWidget* NewDockArea = new CDockAreaWidget(DockManager, _this);
	NewDockArea->addDockWidget(Dockwidget);
	addDockArea(NewDockArea, area);
	return NewDockArea;
}


//============================================================================
void DockContainerWidgetPrivate::addDockArea(CDockAreaWidget* NewDockArea, DockWidgetArea area)
{
	auto InsertParam = internal::dockAreaInsertParameters(area);
	if (DockAreas.isEmpty())
	{
		Layout->addWidget(NewDockArea, 0, 0);
	}
	else if (DockAreas.count() == 1)
	{
		QSplitter* Splitter = internal::newSplitter(InsertParam.orientation());
		auto DockArea = dynamic_cast<CDockAreaWidget*>(Layout->itemAtPosition(0, 0)->widget());
		Layout->replaceWidget(DockArea, Splitter);
		Splitter->addWidget(DockArea);
		insertDockAreaIntoSplitter(Splitter, NewDockArea, InsertParam.append());
	}
	else
	{
		QSplitter* Splitter = _this->findChild<QSplitter*>(QString(), Qt::FindDirectChildrenOnly);
		if (Splitter->orientation() == InsertParam.orientation())
		{
			insertDockAreaIntoSplitter(Splitter, NewDockArea, InsertParam.append());
		}
		else
		{
			QSplitter* NewSplitter = internal::newSplitter(InsertParam.orientation());
			if (InsertParam.append())
			{
				QLayoutItem* li = Layout->replaceWidget(Splitter, NewSplitter);
				NewSplitter->addWidget(Splitter);
				NewSplitter->addWidget(NewDockArea);
				delete li;
			}
			else
			{
				NewSplitter->addWidget(NewDockArea);
				QLayoutItem* li = Layout->replaceWidget(Splitter, NewSplitter);
				NewSplitter->addWidget(Splitter);
				delete li;
			}
		}
	}

	DockAreas.append(NewDockArea);
}


//============================================================================
CDockAreaWidget* DockContainerWidgetPrivate::dockWidgetIntoDockArea(DockWidgetArea area,
	CDockWidget* Dockwidget, CDockAreaWidget* TargetDockArea)
{
	if (CenterDockWidgetArea == area)
	{
		TargetDockArea->addDockWidget(Dockwidget);
		return TargetDockArea;
	}

	CDockAreaWidget* NewDockArea = new CDockAreaWidget(DockManager, _this);
	NewDockArea->addDockWidget(Dockwidget);
	auto InsertParam = internal::dockAreaInsertParameters(area);
	QSplitter* TargetAreaSplitter = internal::findParent<QSplitter*>(TargetDockArea);
	int index = TargetAreaSplitter ->indexOf(TargetDockArea);
	if (TargetAreaSplitter->orientation() == InsertParam.orientation())
	{
		std::cout << "TargetAreaSplitter->orientation() == InsertParam.orientation()" << std::endl;
		TargetAreaSplitter->insertWidget(index + InsertParam.insertOffset(), NewDockArea);
	}
	else
	{
		std::cout << "TargetAreaSplitter->orientation() != InsertParam.orientation()" << std::endl;
		QSplitter* NewSplitter = internal::newSplitter(InsertParam.orientation());
		NewSplitter->addWidget(TargetDockArea);
		insertDockAreaIntoSplitter(NewSplitter, NewDockArea, InsertParam.append());
		TargetAreaSplitter->insertWidget(index, NewSplitter);
	}

	DockAreas.append(NewDockArea);
	return NewDockArea;
}


//============================================================================
CDockContainerWidget::CDockContainerWidget(CDockManager* DockManager, QWidget *parent) :
	QFrame(parent),
	d(new DockContainerWidgetPrivate(this))
{
	//setStyleSheet("background: green;");
	d->DockManager = DockManager;
	if (DockManager != this)
	{
		d->DockManager->registerDockContainer(this);
	}

	d->Layout = new QGridLayout();
	d->Layout->setContentsMargins(0, 1, 0, 0);
	d->Layout->setSpacing(0);
	setLayout(d->Layout);
}

//============================================================================
CDockContainerWidget::~CDockContainerWidget()
{
	if (d->DockManager)
	{
		d->DockManager->removeDockContainer(this);
	}
	delete d;
}


//============================================================================
CDockAreaWidget* CDockContainerWidget::addDockWidget(DockWidgetArea area, CDockWidget* Dockwidget,
	CDockAreaWidget* DockAreaWidget)
{
	CDockAreaWidget* OldDockArea = Dockwidget->dockAreaWidget();
	if (OldDockArea)
	{
		OldDockArea->removeDockWidget(Dockwidget);
	}

	Dockwidget->setDockManager(d->DockManager);
	if (DockAreaWidget)
	{
		return d->dockWidgetIntoDockArea(area, Dockwidget, DockAreaWidget);
	}
	else
	{
		return d->dockWidgetIntoContainer(area, Dockwidget);
	}
}


//============================================================================
unsigned int CDockContainerWidget::zOrderIndex() const
{
	return d->zOrderIndex;
}


//============================================================================
bool CDockContainerWidget::isInFrontOf(CDockContainerWidget* Other) const
{
	return this->zOrderIndex() > Other->zOrderIndex();
}


//============================================================================
bool CDockContainerWidget::event(QEvent *e)
{
	bool Result = QWidget::event(e);
	if (e->type() == QEvent::WindowActivate)
    {
        d->zOrderIndex = ++zOrderCounter;
    }
	else if (e->type() == QEvent::Show && !d->zOrderIndex)
	{
		d->zOrderIndex = ++zOrderCounter;
	}

	return Result;
}


//============================================================================
void CDockContainerWidget::addDockArea(CDockAreaWidget* DockAreaWidget,
	DockWidgetArea area)
{
	CDockContainerWidget* Container = DockAreaWidget->dockContainer();
	if (Container && Container != this)
	{
		Container->removeDockArea(DockAreaWidget);
	}

	d->addDockArea(DockAreaWidget);
}


//============================================================================
void CDockContainerWidget::removeDockArea(CDockAreaWidget* area)
{
	d->DockAreas.removeAll(area);
	QSplitter* Splitter = internal::findParent<QSplitter*>(area);
	area->setParent(0);
	if (!(Splitter && Splitter->count() == 1))
	{
		return;
	}

	// It the splitter contains only one single widget, then we do not need
	// it anymore and can replace it with its content
	std::cout << "Replacing splitter with content" << std::endl;
	QWidget* widget = Splitter->widget(0);
	widget->setParent(this);
	QSplitter* ParentSplitter = internal::findParent<QSplitter*>(Splitter);
	if (ParentSplitter)
	{
		internal::replaceSplitterWidget(ParentSplitter, Splitter, widget);
	}
	else
	{
		d->Layout->replaceWidget(Splitter, widget);
	}
	delete Splitter;
}


//============================================================================
CDockAreaWidget* CDockContainerWidget::dockAreaAt(const QPoint& GlobalPos) const
{
	for (const auto& DockArea : d->DockAreas)
	{
		if (DockArea->rect().contains(DockArea->mapFromGlobal(GlobalPos)))
		{
			return DockArea;
		}
	}

	return 0;
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockContainerWidget.cpp
