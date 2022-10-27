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
/// \brief  Implementation of CDockWidgetSideTab class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <AutoHideDockContainer.h>
#include "DockWidgetSideTab.h"
#include "SideTabBar.h"

#include <QBoxLayout>

#include "DockAreaWidget.h"
#include "DockManager.h"

#include "DockWidget.h"

namespace ads
{
/**
 * Private data class of CDockWidgetTab class (pimpl)
 */
struct DockWidgetSideTabPrivate
{
    CDockWidgetSideTab* _this;
    CDockWidget* DockWidget;
	CSideTabBar* SideTabBar;
	Qt::Orientation Orientation{Qt::Vertical};

	/**
	 * Private data constructor
	 */
	DockWidgetSideTabPrivate(CDockWidgetSideTab* _public);
}; // struct DockWidgetTabPrivate


//============================================================================
DockWidgetSideTabPrivate::DockWidgetSideTabPrivate(CDockWidgetSideTab* _public) :
	_this(_public)
{

}


//============================================================================
void CDockWidgetSideTab::setSideTabBar(CSideTabBar* SideTabBar)
{
	d->SideTabBar = SideTabBar;
}


//============================================================================
void CDockWidgetSideTab::removeFromSideTabBar()
{
	if (d->SideTabBar == nullptr)
	{
		return;
	}
	d->SideTabBar->removeSideTab(this);
    setSideTabBar(nullptr);
}

//============================================================================
CDockWidgetSideTab::CDockWidgetSideTab(CDockWidget* DockWidget, QWidget* parent) : 
	Super(parent),
	d(new DockWidgetSideTabPrivate(this))
{
	setAttribute(Qt::WA_NoMousePropagation);
	d->DockWidget = DockWidget;
	setText(DockWidget->windowTitle());
	setFocusPolicy(Qt::NoFocus);
}


//============================================================================
CDockWidgetSideTab::~CDockWidgetSideTab()
{
	qDebug() << "~CDockWidgetSideTab()";
	delete d;
}


//============================================================================
void CDockWidgetSideTab::updateStyle()
{
	internal::repolishStyle(this, internal::RepolishDirectChildren);
	update();
}


//============================================================================
SideBarLocation CDockWidgetSideTab::sideTabBarArea() const
{
	auto dockAreaWidget = d->DockWidget->dockAreaWidget();
	if (dockAreaWidget && dockAreaWidget->isAutoHide())
	{
		return dockAreaWidget->autoHideDockContainer()->sideTabBarArea();
	}

	return Left;
}


//============================================================================
void CDockWidgetSideTab::setOrientation(Qt::Orientation Orientation)
{
	d->Orientation = Orientation;
	CPushButton::setButtonOrientation((Qt::Horizontal == Orientation)
		? CPushButton::Horizontal : CPushButton::VerticalTopToBottom);
	updateStyle();
}


//============================================================================
Qt::Orientation CDockWidgetSideTab::orientation() const
{
	return d->Orientation;
}


//============================================================================
void CDockWidgetSideTab::updateOrientationAndSpacing(SideBarLocation area)
{
	setOrientation((area == Bottom || area == Top) ? Qt::Horizontal : Qt::Vertical);

	if (icon().isNull())
	{
		return;
	}

	if (CDockManager::testConfigFlag(CDockManager::LeftSideBarPrioritizeIconOnly) && area == Left)
	{
		setText("");
		setOrientation(Qt::Horizontal);
		return;
	}
	if (CDockManager::testConfigFlag(CDockManager::RightSideBarPrioritizeIconOnly) && area == Right)
	{
		setText("");
		setOrientation(Qt::Horizontal);
		return;
	}
	if (CDockManager::testConfigFlag(CDockManager::BottomSideBarPrioritizeIconOnly) && area == Bottom)
	{
		setText("");
		setOrientation(Qt::Horizontal);
		return;
	}
	if (CDockManager::testConfigFlag(CDockManager::TopSideBarPrioritizeIconOnly) && area == Top)
	{
		setText("");
		setOrientation(Qt::Horizontal);
		return;
	}
}


//============================================================================
bool CDockWidgetSideTab::isActiveTab() const
{
	if (d->DockWidget->autoHideDockContainer())
	{
		return d->DockWidget->autoHideDockContainer()->isVisible();
	}

	return false;
}


//============================================================================
CDockWidget* CDockWidgetSideTab::dockWidget() const
{
	return d->DockWidget;
}

}
