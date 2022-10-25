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
/// \file   AutoHideDockContainer.h
/// \author Syarif Fakhri
/// \date   05.09.2022
/// \brief  Implementation of CAutoHideDockContainer class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <AutoHideDockContainer.h>
#include "DockManager.h"
#include "DockWidgetSideTab.h"
#include "DockWidgetTab.h"
#include "SideTabBar.h"
#include "DockAreaWidget.h"
#include "DockingStateReader.h"
#include "ResizeHandle.h"

#include <QXmlStreamWriter>
#include <QBoxLayout>
#include <QPainter>
#include <QSplitter>
#include <QPointer>
#include <QApplication>

#include <iostream>

namespace ads
{
static const int ResizeMargin = 4;

//============================================================================
bool static isHorizontalArea(CDockWidgetSideTab::SideTabBarArea Area)
{
	switch (Area)
	{
	case CDockWidgetSideTab::Top:
	case CDockWidgetSideTab::Bottom: return true;
	case CDockWidgetSideTab::Left:
	case CDockWidgetSideTab::Right: return false;
	}

	return true;
}


//============================================================================
Qt::Edge static edgeFromSideTabBarArea(CDockWidgetSideTab::SideTabBarArea Area)
{
	switch (Area)
	{
	case CDockWidgetSideTab::Top: return Qt::BottomEdge;
	case CDockWidgetSideTab::Bottom: return Qt::TopEdge;
	case CDockWidgetSideTab::Left: return Qt::RightEdge;
	case CDockWidgetSideTab::Right: return Qt::LeftEdge;
	}

	return Qt::LeftEdge;
}


//============================================================================
int resizeHandleLayoutPosition(CDockWidgetSideTab::SideTabBarArea Area)
{
	switch (Area)
	{
	case CDockWidgetSideTab::Bottom:
	case CDockWidgetSideTab::Right: return 0;

	case CDockWidgetSideTab::Top:
	case CDockWidgetSideTab::Left: return 1;
	}

	return 0;
}

struct AutoHideDockContainerPrivate
{
    CAutoHideDockContainer* _this;
	CDockAreaWidget* DockArea{nullptr};
	CDockWidget* DockWidget{nullptr};
	QPointer<CDockManager> DockManager{nullptr};
	CDockWidgetSideTab::SideTabBarArea SideTabBarArea;
	QBoxLayout* Layout;
	CResizeHandle* ResizeHandle = nullptr;
	QSize Size;

	/**
	 * Private data constructor
	 */
	AutoHideDockContainerPrivate(CAutoHideDockContainer *_public);

	/**
	 * Convenience function to get a dock widget area
	 */
	DockWidgetArea getArea(CDockWidgetSideTab::SideTabBarArea area)
	{
        switch (area)
        {
            case CDockWidgetSideTab::Left:
            {
				return LeftDockWidgetArea;
            }
            case CDockWidgetSideTab::Right:
            {
				return RightDockWidgetArea;
            }
            case CDockWidgetSideTab::Bottom: 
            {
				return BottomDockWidgetArea;
            }
            case CDockWidgetSideTab::Top:
            {
				return TopDockWidgetArea;
            }
        }

		return LeftDockWidgetArea;
	}

	/*
	 * Convenience function to get dock position
	 */
	QPoint getSimplifiedDockAreaPosition() const
    {
        switch (SideTabBarArea)
            {
                case CDockWidgetSideTab::Left: 
                {
                    return QPoint(1, _this->height() / 2);
                }
                case CDockWidgetSideTab::Right:
                {
                    return QPoint(_this->width() - 1, _this->height() / 2);
                }
                case CDockWidgetSideTab::Bottom:
                {
                    return QPoint(_this->width() / 2, _this->height() - 1);
                }
                case CDockWidgetSideTab::Top:
                {
                    return QPoint(_this->width() / 2, 1);
                }
            }

		return QPoint();
    }

	void updateResizeHandleSizeLimitMax()
	{
		auto Rect = _this->parentContainer()->contentRect();
		ResizeHandle->setMaxResizeSize(ResizeHandle->orientation() == Qt::Horizontal
			? Rect.width() : Rect.height());
	}
}; // struct AutoHideDockContainerPrivate


//============================================================================
AutoHideDockContainerPrivate::AutoHideDockContainerPrivate(
    CAutoHideDockContainer *_public) :
	_this(_public)
{

}


//============================================================================
CDockContainerWidget* CAutoHideDockContainer::parentContainer() const
{
	return internal::findParent<CDockContainerWidget*>(this);
}


//============================================================================
CAutoHideDockContainer::CAutoHideDockContainer(CDockManager* DockManager, CDockWidgetSideTab::SideTabBarArea area, CDockContainerWidget* parent) :
    Super(parent),
    d(new AutoHideDockContainerPrivate(this))
{
	d->DockManager = DockManager;
	d->SideTabBarArea = area;
	d->DockArea = new CDockAreaWidget(DockManager, parent);
	d->DockArea->setObjectName("autoHideDockArea");
	d->DockArea->setAutoHideDockContainer(this);
	d->DockArea->updateAutoHideButtonCheckState();
	d->DockArea->updateTitleBarButtonToolTip();

	setObjectName("autoHideDockContainer");

	d->Layout = new QBoxLayout(isHorizontalArea(area) ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);
	d->Layout->setContentsMargins(0, 0, 0, 0);
	d->Layout->setSpacing(0);
	setLayout(d->Layout);
	d->Layout->addWidget(d->DockArea);
	d->ResizeHandle = new CResizeHandle(edgeFromSideTabBarArea(area), this);
	d->ResizeHandle->setMinResizeSize(64);
	d->Layout->insertWidget(resizeHandleLayoutPosition(area), d->ResizeHandle);
	d->Size = d->DockArea->size();

	updateSize();
	parent->registerAutoHideWidget(this);
	setAutoFillBackground(true);
	setMouseTracking(true);
}


//============================================================================
CAutoHideDockContainer::CAutoHideDockContainer(CDockWidget* DockWidget, CDockWidgetSideTab::SideTabBarArea area, CDockContainerWidget* parent) :
	CAutoHideDockContainer(DockWidget->dockManager(), area, parent)
{
	addDockWidget(DockWidget);
	setDockSizeProportion(DockWidget->DefaultAutoHideDockProportion());
}


//============================================================================
void CAutoHideDockContainer::updateSize()
{
	qDebug() << "CAutoHideDockContainer::updateSize()";
	auto dockContainerParent = parentContainer();
	auto rect = dockContainerParent->contentRect();

	switch (sideTabBarArea())
	{
	case CDockWidgetSideTab::Top:
		 move(rect.topLeft());
		 resize(rect.width(), qMin(rect.height(), d->Size.height()));
		 break;

	case CDockWidgetSideTab::Left:
		 move(rect.topLeft());
		 resize(qMin(d->Size.width(), rect.width()), rect.height());
		 break;

	case CDockWidgetSideTab::Right:
		 {
			 QPoint p = rect.topRight();
			 p.rx() -= (width() - 1);
			 move(p);
			 resize(qMin(d->Size.width(), rect.width()), rect.height());
		 }
		 break;

	case CDockWidgetSideTab::Bottom:
		 {
			 QPoint p = rect.bottomLeft();
			 p.ry() -= (height() - 1);
			 move(p);
			 resize(rect.width(), qMin(rect.height(), d->Size.height()));
		 }
		 break;
	}

    //resize(rect.width(), rect.height());
}

//============================================================================
CAutoHideDockContainer::~CAutoHideDockContainer()
{
	ADS_PRINT("~CAutoHideDockContainer");

	// Remove event filter in case there are any queued messages
	qApp->removeEventFilter(this);

	if (d->DockManager)
	{
		parentContainer()->removeAutoHideWidget(this);
	}

	delete d;
}

//============================================================================
CSideTabBar* CAutoHideDockContainer::sideTabBar() const
{
	return parentContainer()->sideTabBar(d->SideTabBarArea);
}

//============================================================================
CDockWidget* CAutoHideDockContainer::dockWidget() const
{
	return d->DockWidget;
}

//============================================================================
void CAutoHideDockContainer::addDockWidget(CDockWidget* DockWidget)
{
	if (d->DockWidget)
	{
		// Remove the old dock widget at this area
        d->DockArea->removeDockWidget(d->DockWidget);
	}

	d->DockWidget = DockWidget;
    CDockAreaWidget* OldDockArea = DockWidget->dockAreaWidget();
    if (OldDockArea)
    {
        OldDockArea->removeDockWidget(DockWidget);
    }
	d->DockArea->addDockWidget(DockWidget);
	d->DockWidget->sideTabWidget()->updateOrientationAndSpacing(d->SideTabBarArea);
	qDebug() << "DockWidget->size(): " << DockWidget->size();
	this->resize(OldDockArea ? OldDockArea->size() : d->DockWidget->size());

	updateSize();
}


//============================================================================
void CAutoHideDockContainer::setDockSizeProportion(float SplitterProportion)
{
	if (SplitterProportion < 0 || SplitterProportion > 1)
	{
		ADS_PRINT("SplitterProportion must be set between 0 and 1.");
		return;
	}

	const auto dockSize = static_cast<int>(static_cast<float>(INT_MAX) * SplitterProportion);
	const auto remainingSize = INT_MAX - dockSize;
   /* switch (d->SideTabBarArea)
    {
        case CDockWidgetSideTab::Left:
        {
            setSizes({ dockSize, remainingSize });
			break;
        }
        case CDockWidgetSideTab::Right: 
        case CDockWidgetSideTab::Bottom:
        { 
            setSizes({ remainingSize, dockSize });
			break;
        }
    }*/

}


//============================================================================
CDockWidgetSideTab::SideTabBarArea CAutoHideDockContainer::sideTabBarArea() const
{
	return d->SideTabBarArea;
}

//============================================================================
CDockAreaWidget* CAutoHideDockContainer::dockAreaWidget() const
{
	return d->DockArea;
}

//============================================================================
void CAutoHideDockContainer::moveContentsToParent()
{
	cleanupAndDelete();
	// If we unpin the auto hide tock widget, then we insert it into the same
	// location like it had as a auto hide widget.  This brings the least surprise
	// to the user and he does not have to search where the widget was inserted.
	parentContainer()->addDockWidget(d->getArea(d->SideTabBarArea), d->DockWidget);
    parentContainer()->removeDockArea(d->DockArea);
}


//============================================================================
void CAutoHideDockContainer::cleanupAndDelete()
{
	const auto dockWidget = d->DockWidget;
	if (dockWidget)
	{
        dockWidget->sideTabWidget()->removeFromSideTabBar();
        dockWidget->sideTabWidget()->setParent(dockWidget);
        dockWidget->sideTabWidget()->hide();
	}

	hide();
	deleteLater();
}


//============================================================================
void CAutoHideDockContainer::saveState(QXmlStreamWriter& s)
{
    s.writeAttribute("SideTabBarArea", QString::number(sideTabBarArea())); 
	QStringList Sizes;
	// TODO implement auto hide dock container saving
    /*for (auto Size : sizes())
    {
		Sizes << QString::number(Size);
    }*/

    s.writeAttribute("Sizes", Sizes.join(" "));
}


//============================================================================
bool CAutoHideDockContainer::restoreState(CDockingStateReader& s, bool Testing)
{
	auto sSizes = s.attributes().value("Sizes").trimmed().toString();
	ADS_PRINT("Sizes: " << sSizes);
	QTextStream TextStream(&sSizes);
	QList<int> Sizes;
	while (!TextStream.atEnd())
	{
		int value;
		TextStream >> value;
		Sizes.append(value);
	}

	// TODO implement restore state
	/*if (Sizes.count() != count())
	{
		return false;
	}

	if (!Testing)
	{
		setSizes(Sizes);
	}*/

	return true;
}


//============================================================================
void CAutoHideDockContainer::toggleView(bool Enable)
{
	if (Enable)
	{
        const auto dockWidget = d->DockWidget;
        if (dockWidget)
        {
            dockWidget->sideTabWidget()->show();
        }
	}
	else
	{
        const auto dockWidget = d->DockWidget;
        if (dockWidget)
        {
            dockWidget->sideTabWidget()->hide();
        }
        hide();
        qApp->removeEventFilter(this);
	}
}


//============================================================================
void CAutoHideDockContainer::collapseView(bool Enable)
{
	if (Enable)
	{
		hide();
		d->DockArea->hide();
		d->DockWidget->hide();
		qApp->removeEventFilter(this);
	}
	else
	{
		d->updateResizeHandleSizeLimitMax();
		raise();
		show();
		d->DockArea->show();
		d->DockWidget->show();
		updateSize();
		d->DockManager->setDockWidgetFocused(d->DockWidget);
		qApp->installEventFilter(this);
	}

    d->DockWidget->sideTabWidget()->updateStyle();
}


//============================================================================
void CAutoHideDockContainer::toggleCollapseState()
{
	collapseView(isVisible());
}


//============================================================================
bool CAutoHideDockContainer::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::Resize)
	{
		if (!d->ResizeHandle->isResizing())
		{
			updateSize();
		}
	}
	else if (event->type() == QEvent::MouseButtonPress)
	{
		// First we check, if the mouse button press is inside the dock manager
		// widget. If it is not, i.e. if someone resizes the main window or
		// clicks into the application menu or toolbar, then we ignore the
		// event
		auto widget = qobject_cast<QWidget*>(watched);
		bool IsDockManager = false;
		while (widget)
		{
			if (widget == d->DockManager)
			{
				IsDockManager = true;
			}
			widget = widget->parentWidget();
		}

		if (!IsDockManager)
		{
			return Super::eventFilter(watched, event);
		}

		// Now we check, if the user clicked inside of this auto hide container.
		// If the click is inside of this auto hide container, then we can also
		// ignore the event, because the auto hide overlay should not get collapsed if
		// user works in it
		QMouseEvent* me = static_cast<QMouseEvent*>(event);
		auto pos = mapFromGlobal(me->globalPos());
		if (rect().contains(pos))
		{
			return Super::eventFilter(watched, event);
		}

		// Now check, if the user clicked into the side tab and ignore this event,
		// because the side tab click handler will call collapseView(). If we
		// do not ignore this here, then we will collapse the container and the side tab
		// click handler will uncollapse it
		auto SideTab = d->DockWidget->sideTabWidget();
		pos = SideTab->mapFromGlobal(me->globalPos());
		if (SideTab->rect().contains(pos))
		{
			return Super::eventFilter(watched, event);
		}

		// If the mouse button down event is in the dock manager but outside
		// of the open auto hide container, then the auto hide dock widget
		// should get collapsed
		collapseView(true);
	}

	return Super::eventFilter(watched, event);
}


//============================================================================
void CAutoHideDockContainer::resizeEvent(QResizeEvent* event)
{
	std::cout << "ResizeEvent" << std::endl;
    Super::resizeEvent(event);
	if (d->ResizeHandle->isResizing())
	{
		d->Size = this->size();
		qDebug() << "Size " << d->Size;
		d->updateResizeHandleSizeLimitMax();
	}
}

}

