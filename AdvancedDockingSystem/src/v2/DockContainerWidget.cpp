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

#include "DockManager.h"
#include "DockAreaWidget.h"
#include "ads_globals.h"

namespace ads
{
static unsigned int zOrderCounter = 0;

/**
 * Helper function to ease insertion of dock area into splitter
 */
static void inserDockAreaIntoSplitter(QSplitter* Splitter, QWidget* widget, bool Append)
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
	CDockManager* DockManager = nullptr;
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
		CDockAreaWidget* DockAreaWidget);
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
	auto InsertParam = internal::dockAreaInsertParameters(area);

	if (DockAreas.isEmpty())
	{
		Layout->addWidget(NewDockArea, 0, 0);
	}
	else if (DockAreas.count() == 1)
	{
		QSplitter* Splitter = internal::newSplitter(InsertParam.first);
		auto DockArea = dynamic_cast<CDockAreaWidget*>(Layout->itemAtPosition(0, 0)->widget());
		Layout->replaceWidget(DockArea, Splitter);
		Splitter->addWidget(DockArea);
		inserDockAreaIntoSplitter(Splitter, NewDockArea, InsertParam.second);
	}
	else
	{
		QSplitter* Splitter = _this->findChild<QSplitter*>(QString(), Qt::FindDirectChildrenOnly);
		if (Splitter->orientation() == InsertParam.first)
		{
			inserDockAreaIntoSplitter(Splitter, NewDockArea, InsertParam.second);
		}
		else
		{
			QSplitter* NewSplitter = internal::newSplitter(InsertParam.first);
			if (InsertParam.second)
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
	return NewDockArea;
}


//============================================================================
CDockAreaWidget* DockContainerWidgetPrivate::dockWidgetIntoDockArea(DockWidgetArea area,
	CDockWidget* Dockwidget, CDockAreaWidget* DockAreaWidget)
{
	if (CenterDockWidgetArea == area)
	{
		DockAreaWidget->addDockWidget(Dockwidget);
		return DockAreaWidget;
	}

	auto InsertParam = internal::dockAreaInsertParameters(area);
	return 0;
}


//============================================================================
CDockContainerWidget::CDockContainerWidget(CDockManager* DockManager, QWidget *parent) :
	QFrame(parent),
	d(new DockContainerWidgetPrivate(this))
{
	//setStyleSheet("background: green;");
	d->DockManager = DockManager;

	d->Layout = new QGridLayout();
	d->Layout->setContentsMargins(0, 1, 0, 0);
	d->Layout->setSpacing(0);
	setLayout(d->Layout);
}

//============================================================================
CDockContainerWidget::~CDockContainerWidget()
{
	delete d;
}


//============================================================================
CDockAreaWidget* CDockContainerWidget::addDockWidget(DockWidgetArea area, CDockWidget* Dockwidget,
	CDockAreaWidget* DockAreaWidget)
{
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
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockContainerWidget.cpp
