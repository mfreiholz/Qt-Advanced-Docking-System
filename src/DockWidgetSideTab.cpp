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
#include "DockWidgetSideTab.h"
#include "SideTabBar.h"

#include <QBoxLayout>

#include "DockAreaWidget.h"
#include "ElidingLabel.h"

#include "DockWidget.h"
#include "OverlayDockContainer.h"

namespace ads
{

using tTabLabel = CVerticalElidingLabel;

/**
 * Private data class of CDockWidgetTab class (pimpl)
 */
struct DockWidgetSideTabPrivate
{
    CDockWidgetSideTab* _this;
    CDockWidget* DockWidget;
    tTabLabel* TitleLabel;
	QBoxLayout* Layout;
	CSideTabBar* SideTabBar;

	/**
	 * Private data constructor
	 */
	DockWidgetSideTabPrivate(CDockWidgetSideTab* _public);

	/**
	 * Creates the complete layout
	 */
	void createLayout();
};
// struct DockWidgetTabPrivate


//============================================================================
DockWidgetSideTabPrivate::DockWidgetSideTabPrivate(CDockWidgetSideTab* _public) :
	_this(_public)
{

}

//============================================================================
void DockWidgetSideTabPrivate::createLayout()
{
	TitleLabel = new tTabLabel();
	TitleLabel->setElideMode(Qt::ElideRight);
	TitleLabel->setText(DockWidget->windowTitle());
	TitleLabel->setObjectName("dockWidgetTabLabel");
	TitleLabel->setAlignment(Qt::AlignCenter);
	_this->connect(TitleLabel, SIGNAL(elidedChanged(bool)), SIGNAL(elidedChanged(bool)));

	QFontMetrics fm(TitleLabel->font());
	int Spacing = qRound(fm.height() / 2.0);

	// Fill the layout
	Layout = new QBoxLayout(QBoxLayout::LeftToRight);
	Layout->setContentsMargins(Spacing,Spacing,0,Spacing);
	Layout->setSpacing(0);
	_this->setLayout(Layout);
	Layout->addWidget(TitleLabel, 1);
	Layout->setAlignment(Qt::AlignCenter);

	TitleLabel->setVisible(true);
}

//============================================================================
void CDockWidgetSideTab::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
        emit clicked();
	}

    QFrame::mousePressEvent(event);
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
	QFrame(parent),
	d(new DockWidgetSideTabPrivate(this))
{
	setAttribute(Qt::WA_NoMousePropagation);
	d->DockWidget = DockWidget;
	d->createLayout();
	setFocusPolicy(Qt::NoFocus);
}

//============================================================================
CDockWidgetSideTab::~CDockWidgetSideTab()
{
	delete d;
}

//============================================================================
void CDockWidgetSideTab::updateStyle()
{
	internal::repolishStyle(this, internal::RepolishDirectChildren);
}

CDockWidgetSideTab::SideTabBarArea CDockWidgetSideTab::sideTabBarArea() const
{
	auto dockAreaWidget = d->DockWidget->dockAreaWidget();
	if (dockAreaWidget && dockAreaWidget->isOverlayed())
	{
		return dockAreaWidget->overlayDockContainer()->sideTabBarArea();
	}

	return Left;
}
}
