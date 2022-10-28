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
/// \brief  Implementation of CSideTabBar class
//============================================================================



//============================================================================
//                                   INCLUDES
//============================================================================
#include "SideTabBar.h"

#include <QBoxLayout>
#include <QStyleOption>
#include <QPainter>

#include "DockContainerWidget.h"
#include "DockWidgetSideTab.h"
#include "DockWidgetTab.h"
#include "DockFocusController.h"
#include "AutoHideDockContainer.h"

namespace ads
{
/**
 * Private data class of CSideTabBar class (pimpl)
 */
struct SideTabBarPrivate
{
	/**
	 * Private data constructor
	 */
	SideTabBarPrivate(CSideTabBar* _public);

    CSideTabBar* _this;
    CDockContainerWidget* ContainerWidget;
    QBoxLayout* TabsLayout;
    Qt::Orientation Orientation;
    SideBarLocation SideTabArea = SideBarLocation::Left;

    /**
     * Convenience function to check if this is a horizontal side bar
     */
    bool isHorizontal() const
    {
    	return Qt::Horizontal == Orientation;
    }
}; // struct SideTabBarPrivate

//============================================================================
SideTabBarPrivate::SideTabBarPrivate(CSideTabBar* _public) :
    _this(_public)
{
}


//============================================================================
CSideTabBar::CSideTabBar(CDockContainerWidget* parent, SideBarLocation area) :
    Super(parent),
    d(new SideTabBarPrivate(this))
{
	d->SideTabArea = area;
    d->ContainerWidget = parent;
    d->Orientation = (area == SideBarLocation::Bottom || area == SideBarLocation::Top)
    	? Qt::Horizontal : Qt::Vertical;

    auto mainLayout = new QBoxLayout(d->Orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);

    d->TabsLayout = new QBoxLayout(d->Orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);
    d->TabsLayout->setContentsMargins(0, 0, 0, 0);
    d->TabsLayout->setSpacing(0);
    mainLayout->addLayout(d->TabsLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addStretch(1);
    setLayout(mainLayout);

    setFocusPolicy(Qt::NoFocus);
	if (d->isHorizontal())
	{
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	}
	else
	{
		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	}

	hide();
}


//============================================================================
CSideTabBar::~CSideTabBar() 
{
	qDebug() << "~CSideTabBar() ";
	// The SideTabeBar is not the owner of the tabs and to prevent deletion
	// we set the parent here to nullptr to remove it from the children
	auto Tabs = findChildren<CDockWidgetSideTab*>(QString(), Qt::FindDirectChildrenOnly);
	for (auto Tab : Tabs)
	{
		Tab->setParent(nullptr);
	}
	delete d;
}


//============================================================================
void CSideTabBar::insertSideTab(int Index, CDockWidgetSideTab* SideTab)
{
	SideTab->installEventFilter(this);
    d->TabsLayout->insertWidget(Index, SideTab);
    SideTab->setSideTabBar(this);
    show();
}


//============================================================================
CAutoHideDockContainer* CSideTabBar::insertDockWidget(int Index, CDockWidget* DockWidget)
{
	CDockWidgetSideTab* Tab = new CDockWidgetSideTab(DockWidget);
	auto area = sideTabBarArea();
	qDebug() << "area " << area;
    Tab->setSideTabBar(this);
	Tab->updateOrientationAndSpacing(area);
	d->TabsLayout->insertWidget(Index, Tab);
    Tab->show();

	auto AutoHideContainer = new CAutoHideDockContainer(DockWidget, area, d->ContainerWidget);
	AutoHideContainer->hide();
	DockWidget->dockManager()->dockFocusController()->clearDockWidgetFocus(DockWidget);
	Tab->updateStyle();

	connect(Tab, &CDockWidgetSideTab::pressed, AutoHideContainer, &CAutoHideDockContainer::toggleCollapseState);
	show();
	return AutoHideContainer;
}


//============================================================================
void CSideTabBar::removeDockWidget(CDockWidget* DockWidget)
{
	Q_UNUSED(DockWidget);
	// TODO implement
}


//============================================================================
void CSideTabBar::removeSideTab(CDockWidgetSideTab* SideTab)
{
	qDebug() << "CSideTabBar::removeSideTab " << SideTab->text();
	SideTab->removeEventFilter(this);
    d->TabsLayout->removeWidget(SideTab);
    if (d->TabsLayout->isEmpty())
    {
    	hide();
    }
}


//============================================================================
bool CSideTabBar::event(QEvent* e)
{
	switch (e->type())
	{
	case QEvent::ChildRemoved:
		if (d->TabsLayout->isEmpty())
		{
			hide();
		}
		break;

	case QEvent::Resize:
		if (d->TabsLayout->count())
		{
			auto ev = static_cast<QResizeEvent*>(e);
			auto Tab = tabAt(0);
			int Size = d->isHorizontal() ? ev->size().height() : ev->size().width();
			int TabSize = d->isHorizontal() ? Tab->size().height() : Tab->size().width();
			// If the size of the side bar is less than the size of the first tab
			// then there are no visible tabs in this side bar. This check will
			// fail if someone will force a very big border via CSS!!
			if (Size < TabSize)
			{
				hide();
			}
		}
		else
		{
			hide();
		}
		break;

	default:
		break;
	}
	return Super::event(e);
}


//============================================================================
bool CSideTabBar::eventFilter(QObject *watched, QEvent *event)
{
	if (event->type() != QEvent::ShowToParent)
	{
		return false;
	}

	// As soon as on tab is shhown, we need to show the side tab bar
	auto Tab = qobject_cast<CDockWidgetSideTab*>(watched);
	if (Tab)
	{
		show();
	}
	return false;
}


//============================================================================
void CSideTabBar::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event)

    QStyleOption option;
    option.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);
}


//============================================================================
Qt::Orientation CSideTabBar::orientation() const
{
    return d->Orientation;
}


//============================================================================
CDockWidgetSideTab* CSideTabBar::tabAt(int index) const
{
    return qobject_cast<CDockWidgetSideTab*>(d->TabsLayout->itemAt(index)->widget());
}


//============================================================================
int CSideTabBar::tabCount() const
{
    return d->TabsLayout->count();
}


//============================================================================
SideBarLocation CSideTabBar::sideTabBarArea() const
{
	return d->SideTabArea;
}

}
