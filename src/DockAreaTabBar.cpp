//============================================================================
/// \file   DockAreaTabBar.cpp
/// \author Uwe Kindler
/// \date   24.08.2018
/// \brief  Implementation of CDockAreaTabBar class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockAreaTabBar.h"

#include <QMouseEvent>
#include <QScrollBar>
#include <QDebug>

#include "FloatingDockContainer.h"
#include "DockAreaWidget.h"
#include "DockOverlay.h"
#include "DockManager.h"

namespace ads
{
/**
 * Private data class of CDockAreaTabBar class (pimpl)
 */
struct DockAreaTabBarPrivate
{
	CDockAreaTabBar* _this;
	QPoint DragStartMousePos;
	CDockAreaWidget* DockArea;
	CFloatingDockContainer* FloatingWidget = nullptr;

	/**
	 * Private data constructor
	 */
	DockAreaTabBarPrivate(CDockAreaTabBar* _public);
};
// struct DockAreaTabBarPrivate

//============================================================================
DockAreaTabBarPrivate::DockAreaTabBarPrivate(CDockAreaTabBar* _public) :
	_this(_public)
{

}

//============================================================================
CDockAreaTabBar::CDockAreaTabBar(CDockAreaWidget* parent) :
	QScrollArea(parent),
	d(new DockAreaTabBarPrivate(this))
{
	d->DockArea = parent;
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
	setFrameStyle(QFrame::NoFrame);
	setWidgetResizable(true);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

//============================================================================
CDockAreaTabBar::~CDockAreaTabBar()
{
	delete d;
}


//============================================================================
void CDockAreaTabBar::wheelEvent(QWheelEvent* Event)
{
	Event->accept();
	const int direction = Event->angleDelta().y();
	if (direction < 0)
	{
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() + 20);
	}
	else
	{
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() - 20);
	}
}


//============================================================================
void CDockAreaTabBar::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		ev->accept();
		d->DragStartMousePos = ev->pos();
		return;
	}
	QScrollArea::mousePressEvent(ev);
}


//============================================================================
void CDockAreaTabBar::mouseReleaseEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		qDebug() << "CTabsScrollArea::mouseReleaseEvent";
		ev->accept();
		d->FloatingWidget = nullptr;
		d->DragStartMousePos = QPoint();
		return;
	}
	QScrollArea::mouseReleaseEvent(ev);
}


//============================================================================
void CDockAreaTabBar::mouseMoveEvent(QMouseEvent* ev)
{
	QScrollArea::mouseMoveEvent(ev);
	if (ev->buttons() != Qt::LeftButton)
	{
		return;
	}

	if (d->FloatingWidget)
	{
		d->FloatingWidget->moveFloating();
		return;
	}

	// If this is the last dock area in a dock container it does not make
	// sense to move it to a new floating widget and leave this one
	// empty
	if (d->DockArea->dockContainer()->isFloating()
	 && d->DockArea->dockContainer()->visibleDockAreaCount() == 1)
	{
		return;
	}

	if (!this->geometry().contains(ev->pos()))
	{
		qDebug() << "CTabsScrollArea::startFloating";
		startFloating(d->DragStartMousePos);
		auto Overlay = d->DockArea->dockManager()->containerOverlay();
		Overlay->setAllowedAreas(OuterDockAreas);
	}

	return;
}


//============================================================================
void CDockAreaTabBar::mouseDoubleClickEvent(QMouseEvent *event)
{
	// If this is the last dock area in a dock container it does not make
	// sense to move it to a new floating widget and leave this one
	// empty
	if (d->DockArea->dockContainer()->isFloating() && d->DockArea->dockContainer()->dockAreaCount() == 1)
	{
		return;
	}
	startFloating(event->pos());
}


//============================================================================
void CDockAreaTabBar::startFloating(const QPoint& Pos)
{
	QSize Size = d->DockArea->size();
	CFloatingDockContainer* FloatingWidget = new CFloatingDockContainer(d->DockArea);
	FloatingWidget->startFloating(Pos, Size);
	d->FloatingWidget = FloatingWidget;
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockAreaTabBar.cpp
