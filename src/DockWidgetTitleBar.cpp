/*******************************************************************************
** Qt Advanced Docking System
** Copyright (C) 2017 Uwe Kindler
** 
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public
** License along with this library; If not, see <http://www.gnu.org/licenses/>.
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
#include <QDebug>

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
	QIcon Icon;

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
	bool startFloating();
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
bool DockWidgetTitleBarPrivate::startFloating()
{
	qDebug() << "isFloating " << DockWidget->dockContainer()->isFloating();
	qDebug() << "areaCount " << DockWidget->dockContainer()->dockAreaCount();
	qDebug() << "widgetCount " << DockWidget->dockAreaWidget()->count();
	// if this is the last dock widget inside of this floating widget,
	// then it does not make any sense, to make it floating because
	// it is already floating
	 if (DockWidget->dockContainer()->isFloating()
	 && (DockWidget->dockContainer()->visibleDockAreaCount() == 1)
	 && (DockWidget->dockAreaWidget()->count() == 1))
	{
		return false;
	}

	qDebug() << "startFloating";
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
		qDebug() << "DockWidgetTitleBarPrivate::startFloating DockArea";
		// If section widget has only one content widget, we can move the complete
		// dock area into floating widget
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
	qDebug() << "~CDockWidgetTitleBar()";
	delete d;
}


//============================================================================
void CDockWidgetTitleBar::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		qDebug() << "CDockWidgetTitleBar::mousePressEvent";
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
	qDebug() << "CDockWidgetTitleBar::mouseReleaseEvent";
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
        qDebug() << "Move tab from " << fromIndex << " to " << toIndex;
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
    if (!(ev->buttons() & Qt::LeftButton) || d->isDraggingState(DraggingInactive))
    {
    	d->DragState = DraggingInactive;
        QFrame::mouseMoveEvent(ev);
        return;
    }

    if (d->isDraggingState(DraggingFloatingWidget))
    {
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
    	d->startFloating();
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


//============================================================================
void CDockWidgetTitleBar::setIcon(const QIcon& Icon)
{
	d->Icon = Icon;
	d->IconLabel->setPixmap(Icon.pixmap(this->windowHandle(), QSize(16, 16)));
	d->IconLabel->setVisible(true);
}


//============================================================================
const QIcon& CDockWidgetTitleBar::icon() const
{
	return d->Icon;
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockWidgetTitleBar.cpp
