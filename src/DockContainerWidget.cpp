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
#include "FloatingDockContainer.h"
#include "DockOverlay.h"
#include "DockStateSerialization.h"
#include "ads_globals.h"

#include <iostream>

namespace ads
{
static unsigned int zOrderCounter = 0;

/**
 * Helper function to ease insertion of dock area into splitter
 */
static void insertWidgetIntoSplitter(QSplitter* Splitter, QWidget* widget, bool Append)
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
	bool isFloating = false;

	/**
	 * Private data constructor
	 */
	DockContainerWidgetPrivate(CDockContainerWidget* _public);

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

	/**
	 * Drop floating widget into container
	 */
	void dropIntoContainer(CFloatingDockContainer* FloatingWidget, DockWidgetArea area);

	/**
	 * Drop floating widget into dock area
	 */
	void dropIntoSection(CFloatingDockContainer* FloatingWidget,
		CDockAreaWidget* TargetArea, DockWidgetArea area);

	/**
	 * Adds new dock areas to the internal dock area list
	 */
	void addDockAreasToList(const QList<CDockAreaWidget*> NewDockAreas);

	/**
	 * Save state of child nodes
	 */
	void saveChildNodesState(QDataStream& Stream, QWidget* Widget);

	/**
	 * Restore state of child nodes
	 */
	void restoreChildNodes(QDataStream& Stream, QWidget* Parent);
}; // struct DockContainerWidgetPrivate


//============================================================================
DockContainerWidgetPrivate::DockContainerWidgetPrivate(CDockContainerWidget* _public) :
	_this(_public)
{

}


//============================================================================
void DockContainerWidgetPrivate::dropIntoContainer(CFloatingDockContainer* FloatingWidget,
	DockWidgetArea area)
{
	auto InsertParam = internal::dockAreaInsertParameters(area);
	auto NewDockAreas = FloatingWidget->dockContainer()->findChildren<CDockAreaWidget*>(
		QString(), Qt::FindChildrenRecursively);
	QSplitter* OldSplitter = nullptr;

	// If the container already contains dock areas, then we need to ensure that
	// we have a splitter with an orientation that matches the orientation of
	// the current drop action
	if (!DockAreas.isEmpty())
	{
		OldSplitter = _this->findChild<QSplitter*>(QString(), Qt::FindDirectChildrenOnly);
		// First replace the dock widget with a splitter
		if (DockAreas.count() == 1)
		{
			auto DockArea = dynamic_cast<CDockAreaWidget*>(Layout->itemAtPosition(0, 0)->widget());
			QSplitter* NewSplitter = internal::newSplitter(InsertParam.orientation());
			Layout->replaceWidget(DockArea, NewSplitter);
			NewSplitter->addWidget(DockArea);
			OldSplitter = NewSplitter;
		}
		else if (OldSplitter->orientation() != InsertParam.orientation())
		{
			QSplitter* NewSplitter = internal::newSplitter(InsertParam.orientation());
			QLayoutItem* li = Layout->replaceWidget(OldSplitter, NewSplitter);
			NewSplitter->addWidget(OldSplitter);
			OldSplitter = NewSplitter;
		}
	}

	// Now we can insert the floating widget content into this container
	auto Widget = FloatingWidget->dockContainer()->findChild<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
	auto FloatingSplitter = dynamic_cast<QSplitter*>(Widget);
	if (DockAreas.isEmpty())
	{
		auto Widget = FloatingWidget->dockContainer()->findChild<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
		Layout->addWidget(Widget, 0, 0);
	}
	else if (!FloatingSplitter)
	{
		insertWidgetIntoSplitter(OldSplitter, Widget, InsertParam.append());
	}
	else if (FloatingSplitter->orientation() == InsertParam.orientation())
	{
		while (FloatingSplitter->count())
		{
			insertWidgetIntoSplitter(OldSplitter, FloatingSplitter->widget(0), InsertParam.append());
		}
	}
	else
	{
		insertWidgetIntoSplitter(OldSplitter, FloatingSplitter, InsertParam.append());
	}

	addDockAreasToList(NewDockAreas);
	FloatingWidget->deleteLater();
}


//============================================================================
void DockContainerWidgetPrivate::dropIntoSection(CFloatingDockContainer* FloatingWidget,
		CDockAreaWidget* TargetArea, DockWidgetArea area)
{
	CDockContainerWidget* FloatingContainer = FloatingWidget->dockContainer();
	if (area == CenterDockWidgetArea)
	{
		auto NewDockWidgets = FloatingContainer->findChildren<CDockWidget*>(
			QString(), Qt::FindChildrenRecursively);
		for (auto DockWidget : NewDockWidgets)
		{
			TargetArea->insertDockWidget(0, DockWidget, false);
		}
		TargetArea->setCurrentIndex(0); // make the topmost widget active
		FloatingWidget->deleteLater();
		TargetArea->updateDockArea();
		return;
	}

	auto InsertParam = internal::dockAreaInsertParameters(area);
	auto NewDockAreas = FloatingWidget->dockContainer()->findChildren<CDockAreaWidget*>(
		QString(), Qt::FindChildrenRecursively);
	QSplitter* TargetAreaSplitter = internal::findParent<QSplitter*>(TargetArea);

	if (!TargetAreaSplitter)
	{
		QSplitter* Splitter = internal::newSplitter(InsertParam.orientation());
		Layout->replaceWidget(TargetArea, Splitter);
		Splitter->addWidget(TargetArea);
		TargetAreaSplitter = Splitter;
	}

	int AreaIndex = TargetAreaSplitter->indexOf(TargetArea);
	auto Widget = FloatingWidget->dockContainer()->findChild<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
	auto FloatingSplitter = dynamic_cast<QSplitter*>(Widget);

	if (TargetAreaSplitter->orientation() == InsertParam.orientation())
	{
		if (!FloatingSplitter || FloatingSplitter->orientation() != InsertParam.orientation())
		{
			TargetAreaSplitter->insertWidget(AreaIndex + InsertParam.insertOffset(), Widget);
		}
		else
		{
			int InsertIndex = AreaIndex + InsertParam.insertOffset();
			while (FloatingSplitter->count())
			{
				TargetAreaSplitter->insertWidget(InsertIndex++, FloatingSplitter->widget(0));
			}
		}
	}
	else
	{
		QSplitter* NewSplitter = internal::newSplitter(InsertParam.orientation());
		if (!FloatingSplitter || FloatingSplitter->orientation() != InsertParam.orientation())
		{
			NewSplitter->addWidget(Widget);
		}
		else
		{
			while (FloatingSplitter->count())
			{
				NewSplitter->addWidget(FloatingSplitter->widget(0));
			}
		}

		insertWidgetIntoSplitter(NewSplitter, TargetArea, !InsertParam.append());
		TargetAreaSplitter->insertWidget(AreaIndex, NewSplitter);
	}

	FloatingWidget->deleteLater();
	addDockAreasToList(NewDockAreas);
}


//============================================================================
void DockContainerWidgetPrivate::addDockAreasToList(const QList<CDockAreaWidget*> NewDockAreas)
{
	int CountBefore = DockAreas.count();
	int NewAreaCount = NewDockAreas.count();
	DockAreas.append(NewDockAreas);

	// We need to ensure, that the dock area title bar is visible. The title bar
	// is invisible, if the dock are is a single dock area in a floating widget.
	if (1 == CountBefore)
	{
		DockAreas.at(0)->updateDockArea();
	}

	if (1 == NewAreaCount)
	{
		DockAreas.last()->updateDockArea();
	}

	emit _this->dockAreasAdded();
}


//============================================================================
void DockContainerWidgetPrivate::saveChildNodesState(QDataStream& stream, QWidget* Widget)
{
	QSplitter* Splitter = dynamic_cast<QSplitter*>(Widget);
	if (Splitter)
	{
		stream << NodeSplitter << Splitter->orientation() << Splitter->count();
		std::cout << "NodeSplitter " << Splitter->orientation() << std::endl;
		for (int i = 0; i < Splitter->count(); ++i)
		{
			saveChildNodesState(stream, Splitter->widget(i));
		}
	}
	else
	{
		CDockAreaWidget* DockArea = dynamic_cast<CDockAreaWidget*>(Widget);
		if (DockArea)
		{
			std::cout << "NodeDockArea " << std::endl;
			DockArea->saveState(stream);
		}
	}
}


//============================================================================
void DockContainerWidgetPrivate::restoreChildNodes(QDataStream& stream, QWidget* Parent)
{
	int NodeType;
	stream >> NodeType;
	QSplitter* ParentSplitter = dynamic_cast<QSplitter*>(Parent);
	if (NodeSplitter == NodeType)
	{
		int Orientation;
		int Count;
		stream >> Orientation >> Count;
		std::cout << "Restore NodeSplitter " << Orientation << std::endl;
		QSplitter* Splitter = internal::newSplitter((Qt::Orientation)Orientation);
		if (ParentSplitter)
		{
			ParentSplitter->addWidget(Splitter);
		}
		else
		{
			Parent->layout()->addWidget(Splitter);
		}
		for (int i = 0; i < Count; ++i)
		{
			restoreChildNodes(stream, Splitter);
		}
	}
	else
	{
		std::cout << "Restore NodeDockArea " << std::endl;
		CDockAreaWidget* DockArea = new CDockAreaWidget(DockManager, _this);
		if (ParentSplitter)
		{
			ParentSplitter->addWidget(DockArea);
		}
		else
		{
			Parent->layout()->addWidget(DockArea);
		}
		DockAreas.append(DockArea);
	}
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
		_this->layout()->addWidget(NewDockArea);
	}
	else if (DockAreas.count() == 1)
	{
		QSplitter* Splitter = internal::newSplitter(InsertParam.orientation());
		auto DockArea = dynamic_cast<CDockAreaWidget*>(Layout->itemAtPosition(0, 0)->widget());
		Layout->replaceWidget(DockArea, Splitter);
		Splitter->addWidget(DockArea);
		insertWidgetIntoSplitter(Splitter, NewDockArea, InsertParam.append());
	}
	else
	{
		QSplitter* Splitter = _this->findChild<QSplitter*>(QString(), Qt::FindDirectChildrenOnly);
		if (Splitter->orientation() == InsertParam.orientation())
		{
			insertWidgetIntoSplitter(Splitter, NewDockArea, InsertParam.append());
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
	NewDockArea->updateDockArea();
	emit _this->dockAreasAdded();
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
	if (DockAreas.count() == 1)
	{
		QSplitter* Splitter = internal::newSplitter(InsertParam.orientation());
		auto DockArea = dynamic_cast<CDockAreaWidget*>(Layout->itemAtPosition(0, 0)->widget());
		Layout->replaceWidget(DockArea, Splitter);
		Splitter->addWidget(DockArea);
		insertWidgetIntoSplitter(Splitter, NewDockArea, InsertParam.append());
	}
	else
	{
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
			insertWidgetIntoSplitter(NewSplitter, NewDockArea, InsertParam.append());
			TargetAreaSplitter->insertWidget(index, NewSplitter);
		}
	}

	DockAreas.append(NewDockArea);
	emit _this->dockAreasAdded();
	return NewDockArea;
}


//============================================================================
CDockContainerWidget::CDockContainerWidget(CDockManager* DockManager, QWidget *parent) :
	QFrame(parent),
	d(new DockContainerWidgetPrivate(this))
{
	d->isFloating = dynamic_cast<CFloatingDockContainer*>(parent) != 0;

	//setStyleSheet("background: green;");
	d->DockManager = DockManager;
	if (DockManager != this)
	{
		d->DockManager->registerDockContainer(this);
	}

	d->Layout = new QGridLayout();
	d->Layout->setContentsMargins(0, 1, 0, 1);
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
	std::cout << "CDockContainerWidget::removeDockArea" << std::endl;
	d->DockAreas.removeAll(area);
	QSplitter* Splitter = internal::findParent<QSplitter*>(area);
	area->setParent(0);
	if (!(Splitter && Splitter->count() == 1))
	{
		emit dockAreasRemoved();
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
	emit dockAreasRemoved();
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


//============================================================================
CDockAreaWidget* CDockContainerWidget::dockArea(int Index) const
{
	return (Index < dockAreaCount()) ? d->DockAreas[Index] : 0;
}


//============================================================================
bool CDockContainerWidget::isFloating() const
{
	return d->isFloating;
}


//============================================================================
int CDockContainerWidget::dockAreaCount() const
{
	return d->DockAreas.count();
}


//============================================================================
void CDockContainerWidget::dropFloatingWidget(CFloatingDockContainer* FloatingWidget,
	const QPoint& TargetPos)
{
	std::cout << "CDockContainerWidget::dropFloatingWidget" << std::endl;
	CDockAreaWidget* DockArea = dockAreaAt(TargetPos);
	auto dropArea = InvalidDockWidgetArea;
	if (DockArea)
	{
		auto dropOverlay = d->DockManager->dockAreaOverlay();
		dropOverlay->setAllowedAreas(AllDockAreas);
		dropArea = dropOverlay->showOverlay(DockArea);
		if (dropArea != InvalidDockWidgetArea)
		{
			std::cout << "Dock Area Drop Content: " << dropArea << std::endl;
			d->dropIntoSection(FloatingWidget, DockArea, dropArea);
		}
	}

	// mouse is over container
	if (InvalidDockWidgetArea == dropArea)
	{
		dropArea = d->DockManager->containerOverlay()->dropAreaUnderCursor();
		std::cout << "Container Drop Content: " << dropArea << std::endl;
		if (dropArea != InvalidDockWidgetArea)
		{
			d->dropIntoContainer(FloatingWidget, dropArea);
		}
	}
}


//============================================================================
QList<CDockAreaWidget*> CDockContainerWidget::openedDockAreas() const
{
	QList<CDockAreaWidget*> Result;
	for (auto DockArea : d->DockAreas)
	{
		if (DockArea->isVisible())
		{
			Result.append(DockArea);
		}
	}

	return Result;
}


//============================================================================
void CDockContainerWidget::saveState(QDataStream& stream) const
{
	std::cout << "CDockContainerWidget::saveState" << std::endl;
	stream << isFloating();
	if (isFloating())
	{
		CFloatingDockContainer* FloatingWidget = internal::findParent<CFloatingDockContainer*>(this);
		stream << FloatingWidget->saveGeometry();
	}

	QWidget* RootChild = d->Layout->itemAt(0)->widget();
	if (RootChild)
	{
		d->saveChildNodesState(stream, RootChild);
	}
}


//============================================================================
bool CDockContainerWidget::restoreState(QDataStream& stream)
{
	bool IsFloating;
	stream >> IsFloating;
	if (isFloating())
	{
		std::cout << "Restore floating widget" << std::endl;
		CFloatingDockContainer* FloatingWidget = internal::findParent<CFloatingDockContainer*>(this);
		QByteArray Geometry;
		stream >> Geometry;
		FloatingWidget->restoreGeometry(Geometry);
		FloatingWidget->show();
	}

	QWidget* RootChild = d->Layout->itemAt(0)->widget();
	if (RootChild)
	{
		d->DockAreas.clear();
		delete RootChild;
	}
	d->restoreChildNodes(stream, this);
	return true;
}


} // namespace ads

//---------------------------------------------------------------------------
// EOF DockContainerWidget.cpp
