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
/// \file   DockWidgetTitleBar.cpp
/// \author Uwe Kindler
/// \date   27.02.2017
/// \brief  Implementation of CDockWidgetTitleBar class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockWidgetTitleBar.h"

#include <QBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QStyle>
#include <QApplication>
#include <QSplitter>

#include <iostream>

#include "ads_globals.h"
#include "DockWidget.h"
#include "DockAreaWidget.h"
#include "FloatingDockContainer.h"
#include "DockOverlay.h"
#include "DockManager.h"

namespace ads
{
/**
 * The different dragging states
 */
enum eDragState
{
	DraggingInactive,     //!< DraggingInactive
	DraggingMousePressed, //!< DraggingMousePressed
	DraggingTab,          //!< DraggingTab
	DraggingFloatingWidget//!< DraggingFloatingWidget
};


/**
 * Private data class of CDockWidgetTitleBar class (pimpl)
 */
struct DockWidgetTitleBarPrivate
{
	CDockWidgetTitleBar* _this;
	CDockWidget* DockWidget;
	QLabel* IconLabel;
	QLabel* TitleLabel;
	QPoint DragStartMousePosition;
	bool IsActiveTab = false;
	CDockAreaWidget* DockArea = nullptr;
	eDragState DragState = DraggingInactive;
	CFloatingDockContainer* FloatingWidget = nullptr;

	/**
	 * Private data constructor
	 */
	DockWidgetTitleBarPrivate(CDockWidgetTitleBar* _public);

	/**
	 * Creates the complete layout including all controls
	 */
	void createLayout();

	/**
	 * Moves the tab depending on the position in the given mouse event
	 */
	void moveTab(QMouseEvent* ev);

	/**
	 * Test function for current drag state
	 */
	bool isDraggingState(eDragState dragState)
	{
		return this->DragState == dragState;
	}

	/**
	 * Returns true if the given global point is inside the title area geometry
	 * rectangle.
	 * The position is given as global position.
	 */
	bool titleAreaGeometryContains(const QPoint& GlobalPos) const
	{
		return DockArea->titleAreaGeometry().contains(DockArea->mapFromGlobal(GlobalPos));
	}

	/**
	 * Starts floating of the dock widget that belongs to this title bar
	 * Returns true, if floating has been started and false if floating
	 * is not possible for any reason
	 */
	bool startFloating(const QPoint& GlobalPos);
};
// struct DockWidgetTitleBarPrivate


//============================================================================
DockWidgetTitleBarPrivate::DockWidgetTitleBarPrivate(CDockWidgetTitleBar* _public) :
	_this(_public)
{

}


//============================================================================
void DockWidgetTitleBarPrivate::createLayout()
{
	QBoxLayout* l = new QBoxLayout(QBoxLayout::LeftToRight);
	l->setContentsMargins(0, 0, 0, 0);
	_this->setLayout(l);

	IconLabel = new QLabel();
	IconLabel->setAlignment(Qt::AlignVCenter);
	l->addWidget(IconLabel, Qt::AlignVCenter);

	TitleLabel = new QLabel();
	l->addWidget(TitleLabel, 1);

	IconLabel->setVisible(false);
	TitleLabel->setVisible(true);
	TitleLabel->setText(DockWidget->windowTitle());
}

//============================================================================
void DockWidgetTitleBarPrivate::moveTab(QMouseEvent* ev)
{
    ev->accept();
    int left, top, right, bottom;
    _this->getContentsMargins(&left, &top, &right, &bottom);
    QPoint moveToPos = _this->mapToParent(ev->pos()) - DragStartMousePosition;
    moveToPos.setY(0);
    _this->move(moveToPos);
    _this->raise();
}


//============================================================================
bool DockWidgetTitleBarPrivate::startFloating(const QPoint& GlobalPos)
{
	std::cout << "isFloating " << DockWidget->dockContainer()->isFloating() << std::endl;
	std::cout << "areaCount " << DockWidget->dockContainer()->dockAreaCount() << std::endl;
	std::cout << "widgetCount " << DockWidget->dockAreaWidget()->count() << std::endl;
	// if this is the last dock widget inside of this floating widget,
	// then it does not make any sense, to make if floating because
	// it is already floating
	 if (DockWidget->dockContainer()->isFloating()
	 && (DockWidget->dockContainer()->dockAreaCount() == 1)
	 && (DockWidget->dockAreaWidget()->count() == 1))
	{
		return false;
	}

	std::cout << "startFloating" << std::endl;
	DragState = DraggingFloatingWidget;
	QSize Size = DockArea->size();
	CFloatingDockContainer* FloatingWidget = nullptr;
	if (DockArea->count() > 1)
	{
		// If section widget has multiple tabs, we take only one tab
		FloatingWidget = new CFloatingDockContainer(DockWidget);
	}
	else
	{
		std::cout << "DockWidgetTitleBarPrivate::startFloating DockArea" << std::endl;
		// If section widget has only one content widget, we can move the complete
		// section widget into floating widget
		auto splitter = internal::findParent<QSplitter*>(DockArea);
		FloatingWidget = new CFloatingDockContainer(DockArea);
	}

    FloatingWidget->startFloating(DragStartMousePosition, Size);
    auto Overlay = DockWidget->dockManager()->containerOverlay();
	Overlay->setAllowedAreas(OuterDockAreas);
	this->FloatingWidget = FloatingWidget;
	return true;
}


//============================================================================
CDockWidgetTitleBar::CDockWidgetTitleBar(CDockWidget* DockWidget, QWidget *parent) :
	QFrame(parent),
	d(new DockWidgetTitleBarPrivate(this))
{
	setAttribute(Qt::WA_NoMousePropagation, true);
	d->DockWidget = DockWidget;
	d->createLayout();
}

//============================================================================
CDockWidgetTitleBar::~CDockWidgetTitleBar()
{
	std::cout << "~CDockWidgetTitleBar()" << std::endl;
	delete d;
}


//============================================================================
void CDockWidgetTitleBar::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		std::cout << "CDockWidgetTitleBar::mousePressEvent" << std::endl;
		ev->accept();
        d->DragStartMousePosition = ev->pos();
        d->DragState = DraggingMousePressed;
		return;
	}
	QFrame::mousePressEvent(ev);
}



//============================================================================
void CDockWidgetTitleBar::mouseReleaseEvent(QMouseEvent* ev)
{
	std::cout << "CDockWidgetTitleBar::mouseReleaseEvent" << std::endl;
	// End of tab moving, change order now
	if (d->isDraggingState(DraggingTab) && d->DockArea)
	{
		// Find tab under mouse
		QPoint pos = d->DockArea->mapFromGlobal(ev->globalPos());
        int fromIndex = d->DockArea->tabIndex(d->DockWidget);
        int toIndex = d->DockArea->indexOfContentByTitlePos(pos, this);
        if (-1 == toIndex)
        {
            toIndex = d->DockArea->count() - 1;
        }
        std::cout << "Move tab from " << fromIndex << " to " << toIndex << std::endl;
        d->DockArea->reorderDockWidget(fromIndex, toIndex);
	}

    if (!d->DragStartMousePosition.isNull())
    {
		emit clicked();
    }

    d->DragStartMousePosition = QPoint();
    d->DragState = DraggingInactive;
	QFrame::mouseReleaseEvent(ev);
}


//============================================================================
void CDockWidgetTitleBar::mouseMoveEvent(QMouseEvent* ev)
{
	std::cout << "CDockWidgetTitleBar::mouseMoveEventmouseMoveEvent DragState "
		<< d->DragState << std::endl;
    if (!(ev->buttons() & Qt::LeftButton) || d->isDraggingState(DraggingInactive))
    {
    	d->DragState = DraggingInactive;
        QFrame::mouseMoveEvent(ev);
        return;
    }

    if (d->isDraggingState(DraggingFloatingWidget))
    {
    	std::cout << "DraggingFloatingWidget" << std::endl;
    	d->FloatingWidget->moveFloating();
        QFrame::mouseMoveEvent(ev);
        return;
    }

    // move tab
    if (d->isDraggingState(DraggingTab))
    {
        d->moveTab(ev);
    }

    bool MouseInsideTitleArea = d->titleAreaGeometryContains(ev->globalPos());
    if (!MouseInsideTitleArea)
	{
    	d->startFloating(ev->globalPos());
    	return;
	}
    else if (d->DockArea->count() > 1
     && (ev->pos() - d->DragStartMousePosition).manhattanLength() >= QApplication::startDragDistance()) // Wait a few pixels before start moving
	{
        d->DragState = DraggingTab;
		return;
	}

   QFrame::mouseMoveEvent(ev);
}


//============================================================================
bool CDockWidgetTitleBar::isActiveTab() const
{
	return d->IsActiveTab;
}


//============================================================================
void CDockWidgetTitleBar::setActiveTab(bool active)
{
	if (d->IsActiveTab == active)
	{
		return;
	}

	d->IsActiveTab = active;
	style()->unpolish(this);
	style()->polish(this);
	d->TitleLabel->style()->unpolish(d->TitleLabel);
	d->TitleLabel->style()->polish(d->TitleLabel);
	update();

	emit activeTabChanged();
}


//============================================================================
CDockWidget* CDockWidgetTitleBar::dockWidget() const
{
	return d->DockWidget;
}


//============================================================================
void CDockWidgetTitleBar::setDockAreaWidget(CDockAreaWidget* DockArea)
{
	d->DockArea = DockArea;
}


//============================================================================
CDockAreaWidget* CDockWidgetTitleBar::dockAreaWidget() const
{
	return d->DockArea;
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockWidgetTitleBar.cpp
