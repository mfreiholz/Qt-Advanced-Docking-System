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
/// \file   DockAreaTitleBar.cpp
/// \author Uwe Kindler
/// \date   12.10.2018
/// \brief  Implementation of CDockAreaTitleBar class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockAreaTitleBar.h"

#include <QPushButton>
#include <QToolButton>
#include <QBoxLayout>
#include <QStyle>
#include <QMenu>
#include <QScrollArea>
#include <QMouseEvent>
#include <QDebug>

#include "ads_globals.h"
#include "FloatingDockContainer.h"
#include "DockAreaWidget.h"
#include "DockOverlay.h"
#include "DockManager.h"
#include "DockWidget.h"
#include "DockWidgetTab.h"
#include "DockAreaTabBar.h"


namespace ads
{
using tTileBarButton = QToolButton;
/**
 * Private data class of CDockAreaTitleBar class (pimpl)
 */
struct DockAreaTitleBarPrivate
{
	CDockAreaTitleBar* _this;
	tTileBarButton* TabsMenuButton;
	tTileBarButton* UndockButton;
	tTileBarButton* CloseButton;
	QBoxLayout* TopLayout;
	CDockAreaWidget* DockArea;
	CDockAreaTabBar* TabBar;
	bool MenuOutdated = true;
	QMenu* TabsMenu;

	/**
	 * Private data constructor
	 */
	DockAreaTitleBarPrivate(CDockAreaTitleBar* _public);

	/**
	 * Creates the title bar close and menu buttons
	 */
	void createButtons();

	/**
	 * Creates the internal TabBar
	 */
	void createTabBar();
};// struct DockAreaTitleBarPrivate



//============================================================================
DockAreaTitleBarPrivate::DockAreaTitleBarPrivate(CDockAreaTitleBar* _public) :
	_this(_public)
{

}


//============================================================================
void DockAreaTitleBarPrivate::createButtons()
{
	TabsMenuButton = new tTileBarButton();
	TabsMenuButton->setObjectName("tabsMenuButton");
	TabsMenuButton->setAutoRaise(true);
	TabsMenuButton->setPopupMode(QToolButton::InstantPopup);
	TabsMenuButton->setIcon(_this->style()->standardIcon(QStyle::SP_TitleBarUnshadeButton));

	QMenu* TabsMenu = new QMenu(TabsMenuButton);
	_this->connect(TabsMenu, SIGNAL(aboutToShow()), SLOT(onTabsMenuAboutToShow()));
	TabsMenuButton->setMenu(TabsMenu);
	TabsMenuButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	TopLayout->addWidget(TabsMenuButton, 0);
	_this->connect(TabsMenuButton->menu(), SIGNAL(triggered(QAction*)),
		SLOT(onTabsMenuActionTriggered(QAction*)));

	// Undock button
	UndockButton = new tTileBarButton();
	UndockButton->setObjectName("undockButton");
	UndockButton->setAutoRaise(true);
	UndockButton->setIcon(_this->style()->standardIcon(QStyle::SP_TitleBarNormalButton));
	UndockButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	TopLayout->addWidget(UndockButton, 0);
	_this->connect(UndockButton, SIGNAL(clicked()), SLOT(onUndockButtonClicked()));

	CloseButton = new tTileBarButton();
	CloseButton->setObjectName("closeButton");
	CloseButton->setAutoRaise(true);

	// The standard icons do not look good on high DPI screens
	QIcon CloseIcon =  _this->style()->standardIcon(QStyle::SP_TitleBarCloseButton);
	QPixmap normalPixmap = _this->style()->standardPixmap(QStyle::SP_TitleBarCloseButton, 0, CloseButton);
	QPixmap disabledPixmap = internal::createTransparentPixmap(normalPixmap, 0.25);
	CloseIcon.addPixmap(disabledPixmap, QIcon::Disabled);

	CloseButton->setIcon(CloseIcon);
	CloseButton->setToolTip(QObject::tr("Close all tabs"));
	CloseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	TopLayout->addWidget(CloseButton, 0);
	_this->connect(CloseButton, SIGNAL(clicked()), SLOT(onCloseButtonClicked()));
}


//============================================================================
void DockAreaTitleBarPrivate::createTabBar()
{
	TabBar = new CDockAreaTabBar(DockArea);
	TopLayout->addWidget(TabBar);
	_this->connect(TabBar, SIGNAL(tabClosed(int)), SLOT(markTabsMenuOutdated()));
	_this->connect(TabBar, SIGNAL(tabOpened(int)), SLOT(markTabsMenuOutdated()));
	_this->connect(TabBar, SIGNAL(tabInserted(int)), SLOT(markTabsMenuOutdated()));
	_this->connect(TabBar, SIGNAL(removingTab(int)), SLOT(markTabsMenuOutdated()));
	_this->connect(TabBar, SIGNAL(tabMoved(int, int)), SLOT(markTabsMenuOutdated()));
	_this->connect(TabBar, SIGNAL(currentChanged(int)), SLOT(onCurrentTabChanged(int)));
	_this->connect(TabBar, SIGNAL(tabBarClicked(int)), SIGNAL(tabBarClicked(int)));
}


//============================================================================
CDockAreaTitleBar::CDockAreaTitleBar(CDockAreaWidget* parent) :
	QFrame(parent),
	d(new DockAreaTitleBarPrivate(this))
{
	d->DockArea = parent;

	setObjectName("dockAreaTitleBar");
	d->TopLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	d->TopLayout->setContentsMargins(0, 0, 0, 0);
	d->TopLayout->setSpacing(0);
	setLayout(d->TopLayout);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	d->createTabBar();
	d->createButtons();

}


//============================================================================
CDockAreaTitleBar::~CDockAreaTitleBar()
{
	delete d;
}


//============================================================================
CDockAreaTabBar* CDockAreaTitleBar::tabBar() const
{
	return d->TabBar;
}


//============================================================================
void CDockAreaTitleBar::markTabsMenuOutdated()
{
	d->MenuOutdated = true;
}


//============================================================================
void CDockAreaTitleBar::onTabsMenuAboutToShow()
{
	if (!d->MenuOutdated)
	{
		return;
	}

	QMenu* menu = d->TabsMenuButton->menu();
	menu->clear();
	for (int i = 0; i < d->TabBar->count(); ++i)
	{
		if (!d->TabBar->isTabOpen(i))
		{
			continue;
		}
		auto Tab = d->TabBar->tab(i);
		QAction* Action = menu->addAction(Tab->icon(), Tab->text());
		Action->setData(i);
	}

	d->MenuOutdated = false;
}


//============================================================================
void CDockAreaTitleBar::onCloseButtonClicked()
{
	qDebug() << "CDockAreaTitleBar::onCloseButtonClicked";
	//d->TabBar->closeTab(d->TabBar->currentIndex());
	d->DockArea->closeArea();
}


//============================================================================
void CDockAreaTitleBar::onUndockButtonClicked()
{
	d->TabBar->makeAreaFloating(mapFromGlobal(QCursor::pos()));
}


//============================================================================
void CDockAreaTitleBar::onTabsMenuActionTriggered(QAction* Action)
{
	int Index = Action->data().toInt();
	d->TabBar->setCurrentIndex(Index);
	emit tabBarClicked(Index);
}


//============================================================================
void CDockAreaTitleBar::onCurrentTabChanged(int Index)
{
	if (Index < 0)
	{
		return;
	}
	/*CDockWidget* DockWidget = d->TabBar->tab(Index)->dockWidget();
	d->CloseButton->setEnabled(DockWidget->features().testFlag(CDockWidget::DockWidgetClosable));*/
}


//============================================================================
QAbstractButton* CDockAreaTitleBar::button(TitleBarButton which) const
{
	switch (which)
	{
	case TitleBarButtonTabsMenu: return d->TabsMenuButton;
	case TitleBarButtonUndock: return d->UndockButton;
	case TitleBarButtonClose: return d->CloseButton;
	default:
		return nullptr;
	}
}


//============================================================================
void CDockAreaTitleBar::setVisible(bool Visible)
{
	Super::setVisible(Visible);
}


} // namespace ads

//---------------------------------------------------------------------------
// EOF DockAreaTitleBar.cpp
