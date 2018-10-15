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
#include <QBoxLayout>
#include <QStyle>
#include <QMenu>
#include <QScrollArea>
#include <QMouseEvent>
#include <QDebug>
#include <QStyleOptionButton>

#include "FloatingDockContainer.h"
#include "DockAreaWidget.h"
#include "DockOverlay.h"
#include "DockManager.h"
#include "DockWidget.h"
#include "DockWidgetTab.h"
#include "DockAreaTabBar.h"

#include <iostream>

namespace ads
{
/**
 * Private data class of CDockAreaTitleBar class (pimpl)
 */
struct DockAreaTitleBarPrivate
{
	CDockAreaTitleBar* _this;
	QPushButton* TabsMenuButton;
	QPushButton* CloseButton;
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
	TabsMenuButton = new QPushButton();
	TabsMenuButton->setObjectName("tabsMenuButton");
	TabsMenuButton->setFlat(true);
	TabsMenuButton->setIcon(_this->style()->standardIcon(QStyle::SP_TitleBarUnshadeButton));
	TabsMenuButton->setMaximumWidth(TabsMenuButton->iconSize().width());

	QMenu* TabsMenu = new QMenu(TabsMenuButton);
	_this->connect(TabsMenu, SIGNAL(aboutToShow()), SLOT(onTabsMenuAboutToShow()));
	TabsMenuButton->setMenu(TabsMenu);
	TopLayout->addWidget(TabsMenuButton, 0);
	TabsMenuButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	_this->connect(TabsMenuButton->menu(), SIGNAL(triggered(QAction*)),
		SLOT(onTabsMenuActionTriggered(QAction*)));

	CloseButton = new QPushButton();
	CloseButton->setObjectName("closeButton");
	CloseButton->setFlat(true);
	CloseButton->setIcon(_this->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	CloseButton->setToolTip(QObject::tr("Close"));
	CloseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
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
	d->TabBar->closeTab(d->TabBar->currentIndex());
}


//============================================================================
void CDockAreaTitleBar::onTabsMenuActionTriggered(QAction* Action)
{
	int Index = Action->data().toInt();
	d->TabBar->setCurrentIndex(Index);
}


//============================================================================
void CDockAreaTitleBar::onCurrentTabChanged(int Index)
{
	CDockWidget* DockWidget = d->TabBar->tab(Index)->dockWidget();
	d->CloseButton->setVisible(DockWidget->features().testFlag(CDockWidget::DockWidgetClosable));
}


} // namespace ads

//---------------------------------------------------------------------------
// EOF DockAreaTitleBar.cpp
