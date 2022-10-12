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
/// \file   DockWidgetTab.h
/// \author Syarif Fakhri
/// \date   05.09.2022
/// \brief  Implementation of COverlayDockContainer class
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

#include <QXmlStreamWriter>
#include <QBoxLayout>
#include <QPainter>
#include <QSplitter>
#include <QPointer>
#include <QApplication>

#include <iostream>

namespace ads
{
struct AutoHideDockContainerPrivate
{
    CAutoHideDockContainer* _this;
	CDockAreaWidget* DockArea{nullptr};
	CDockWidget* DockWidget{nullptr};
	QPointer<CDockManager> DockManager{nullptr};
	CDockWidgetSideTab::SideTabBarArea Area;
	bool Collapsed = true;
	bool Ignore = false;

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
            case CDockWidgetSideTab::LeftBottom: 
            case CDockWidgetSideTab::LeftTop:
            {
				return LeftDockWidgetArea;
            }
            case CDockWidgetSideTab::RightBottom:
            case CDockWidgetSideTab::RightTop:
            {
				return RightDockWidgetArea;
            }
            case CDockWidgetSideTab::Bottom: 
            {
				return BottomDockWidgetArea;
            }
        }

		return LeftDockWidgetArea;
	}

	/*
	 * Convenience function to get dock position
	 */
	QPoint getSimplifiedDockAreaPosition() const
    {
        switch (Area)
            {
                case CDockWidgetSideTab::LeftTop: 
                case CDockWidgetSideTab::LeftBottom:
                {
                    return QPoint(1, _this->height() / 2);
                }
                case CDockWidgetSideTab::RightTop:
                case CDockWidgetSideTab::RightBottom:
                {
                    return QPoint(_this->width() - 1, _this->height() / 2);
                }
                case CDockWidgetSideTab::Bottom:
                {
                    return QPoint(_this->width() / 2, _this->height() - 1);
                }
            }

		return QPoint();
    }
}; // struct OverlayDockContainerPrivate

//============================================================================
AutoHideDockContainerPrivate::AutoHideDockContainerPrivate(
    CAutoHideDockContainer *_public) :
	_this(_public)
{

}

CDockContainerWidget* CAutoHideDockContainer::parentContainer() const
{
	return internal::findParent<CDockContainerWidget*>(this);
}

//============================================================================
CAutoHideDockContainer::CAutoHideDockContainer(CDockManager* DockManager, CDockWidgetSideTab::SideTabBarArea area, CDockContainerWidget* parent) :
    QSplitter(area == CDockWidgetSideTab::Bottom ? Qt::Orientation::Vertical : Qt::Orientation::Horizontal, parent),
    d(new AutoHideDockContainerPrivate(this))
{
	d->DockManager = DockManager;
	d->Area = area;
	d->DockArea = new CDockAreaWidget(DockManager, parent);
	d->DockArea->setObjectName("overlayDockArea");
	d->DockArea->setOverlayDockContainer(this);
	d->DockArea->updateAutoHideButtonCheckState();
	d->DockArea->updateTitleBarButtonToolTip();

	setObjectName("overlaySplitter");
	setChildrenCollapsible(false);

	const auto emptyWidget = new QWidget();
	emptyWidget->setMinimumWidth(50); 
	emptyWidget->setMinimumHeight(50); // Prevents you from dragging the splitter too far

    switch (area)
    {
        case CDockWidgetSideTab::LeftBottom: 
        case CDockWidgetSideTab::LeftTop:
        {
            addWidget(d->DockArea);
            addWidget(emptyWidget);
            break;
        }
        case CDockWidgetSideTab::RightBottom: 
        case CDockWidgetSideTab::RightTop: 
        {
            addWidget(emptyWidget);
            addWidget(d->DockArea);
            break;
        }
        case CDockWidgetSideTab::Bottom: 
        {
            addWidget(emptyWidget);
            addWidget(d->DockArea);
			break;
        }
    }

	updateMask();
	updateSize();

	parent->registerOverlayWidget(this);

	d->DockArea->installEventFilter(this);
	parent->installEventFilter(this);
}

//============================================================================
void CAutoHideDockContainer::updateMask()
{
    const auto rect = d->DockArea->frameGeometry();
    const auto topLeft = rect.topLeft();
    const auto handleSize = handleWidth();

	if (d->Area == CDockWidgetSideTab::Bottom)
	{
        setMask(QRect(QPoint(topLeft.x(), topLeft.y() - handleSize), QSize(rect.size().width(), rect.size().height() + handleSize)));
		return;
	}

	auto offset = 0;
    if (d->Area == CDockWidgetSideTab::SideTabBarArea::RightTop || d->Area == CDockWidgetSideTab::SideTabBarArea::RightBottom)
    {
        offset = handleSize;
    }
    setMask(QRect(QPoint(topLeft.x() - offset, topLeft.y()), QSize(rect.size().width() + handleSize, rect.size().height())));
}

//============================================================================
void CAutoHideDockContainer::updateSize()
{
	const auto dockContainerParent = parentContainer();
	const auto rootSplitter = dockContainerParent->rootSplitter();
	const auto rect = rootSplitter->frameGeometry();
	move(rect.topLeft());
    resize(rect.width(), rect.height());
}

//============================================================================
CAutoHideDockContainer::CAutoHideDockContainer(CDockWidget* DockWidget, CDockWidgetSideTab::SideTabBarArea area, CDockContainerWidget* parent) : 
	CAutoHideDockContainer(DockWidget->dockManager(), area, parent)
{
	addDockWidget(DockWidget);
	setDockSizeProportion(DockWidget->DefaultOverlayDockProportion());
}

//============================================================================
CAutoHideDockContainer::~CAutoHideDockContainer()
{
	ADS_PRINT("~COverlayDockContainer");

	// Remove event filter in case there are any queued messages
	d->DockArea->removeEventFilter(this);
    parent()->removeEventFilter(this);

	if (d->DockManager)
	{
		parentContainer()->removeOverlayWidget(this);
	}

	delete d;
}

//============================================================================
CSideTabBar* CAutoHideDockContainer::sideTabBar() const
{
	return parentContainer()->sideTabBar(d->Area);
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
	d->DockWidget->sideTabWidget()->updateOrientationAndSpacing(d->Area);

	updateSize();
	updateMask();
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
    switch (d->Area)
    {
        case CDockWidgetSideTab::LeftBottom:
        case CDockWidgetSideTab::LeftTop:
        {
            setSizes({ dockSize, remainingSize });
			break;
        }
        case CDockWidgetSideTab::RightBottom: 
        case CDockWidgetSideTab::RightTop: 
        case CDockWidgetSideTab::Bottom:
        { 
            setSizes({ remainingSize, dockSize });
			break;
        }
    }

}


//============================================================================
CDockWidgetSideTab::SideTabBarArea CAutoHideDockContainer::sideTabBarArea() const
{
	return d->Area;
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

	const auto position = mapToGlobal(d->getSimplifiedDockAreaPosition());

	const auto dockAreaWidget = parentContainer()->dockAreaAt(position);
	if (dockAreaWidget != nullptr && !dockAreaWidget->containsCentralWidget())
	{
        parentContainer()->addDockWidget(CenterDockWidgetArea, d->DockWidget, dockAreaWidget);
	}
	else
	{
        parentContainer()->addDockWidget(d->getArea(d->Area), d->DockWidget);
	}

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
    for (auto Size : sizes())
    {
		Sizes << QString::number(Size);
    }

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

	if (Sizes.count() != count())
	{
		return false;
	}

	if (!Testing)
	{
		setSizes(Sizes);
	}

	return true;
}

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

void CAutoHideDockContainer::collapseView(bool Enable)
{
	std::cout << "COverlayDockContainer::collapseView " << Enable << "  "
		<< d->DockWidget->objectName().toStdString() << std::endl;

	if (d->Ignore)
	{
		return;
	}

	if (Enable)
	{
		hide();
		d->DockArea->hide();
		d->DockWidget->hide();
	}
	else
	{
		raise();
		show();
		d->DockArea->show();
		d->DockWidget->show();
		qApp->installEventFilter(this);
	}
	d->Collapsed = Enable;
}


//============================================================================
void CAutoHideDockContainer::toggleCollapseState()
{
	collapseView(isVisible());
}


//============================================================================
bool CAutoHideDockContainer::areaExistsInConfig(CDockWidgetSideTab::SideTabBarArea area)
{
    switch (area)
    {
        case CDockWidgetSideTab::LeftBottom:
        case CDockWidgetSideTab::LeftTop:
        {
			return CDockManager::testConfigFlag(CDockManager::DockContainerHasLeftSideBar);
        }
        case CDockWidgetSideTab::RightBottom:
        case CDockWidgetSideTab::RightTop:
        {
			return CDockManager::testConfigFlag(CDockManager::DockContainerHasRightSideBar);
        }
        case CDockWidgetSideTab::Bottom: 
        {
			return CDockManager::testConfigFlag(CDockManager::DockContainerHasBottomSideBar);
        }
    }

	return true;
}


//============================================================================
bool CAutoHideDockContainer::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::Resize)
	{
		updateSize();
		updateMask();
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
			return QSplitter::eventFilter(watched, event);
		}

		// Now we check, if the user clicked inside of this overlay. If the click
		// is inside of the overlay, the we can also ignore the event, because
		// the overlay should not get collapsed if user works in it
		QMouseEvent* me = static_cast<QMouseEvent*>(event);
		auto pos = d->DockArea->mapFromGlobal(me->globalPos());
		auto rect = d->DockArea->rect().adjusted(-handleWidth(), 0, 0, 0);
		if (rect.contains(pos))
		{
			return QSplitter::eventFilter(watched, event);
		}

		// If the mouse button down event is in the dock manager but outside
		// of the open overlay container, then the overlay dock widget
		// should get collapsed
		collapseView(true);
		d->Ignore = true;
	}
	else if (event->type() == QEvent::MouseButtonRelease)
	{
		std::cout << "Mouse release: " << watched->metaObject()->className() << std::endl;
		d->Ignore = false;
		if (!isVisible())
		{
			qApp->removeEventFilter(this);
		}
	}

	return QSplitter::eventFilter(watched, event);
}


//============================================================================
void CAutoHideDockContainer::resizeEvent(QResizeEvent* event)
{
	updateMask();
    QSplitter::resizeEvent(event);
}
}

