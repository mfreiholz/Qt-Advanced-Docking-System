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
#include "DockContainerWidget.h"
#include "DockWidgetSideTab.h"
#include "DockWidgetTab.h"

#include <QBoxLayout>
#include <QStyleOption>
#include <QPainter>

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
	delete d;
}


//============================================================================
void CSideTabBar::insertSideTab(int Index, CDockWidgetSideTab* SideTab)
{
    d->TabsLayout->insertWidget(Index, SideTab);
    SideTab->setSideTabBar(this);
    show();
}


//============================================================================
void CSideTabBar::removeSideTab(CDockWidgetSideTab* SideTab)
{
	qDebug() << "CSideTabBar::removeSideTab " << SideTab->text();
    d->TabsLayout->removeWidget(SideTab);
    if (d->TabsLayout->isEmpty())
    {
    	hide();
    }
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
