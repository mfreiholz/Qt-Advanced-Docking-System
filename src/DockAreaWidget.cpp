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
/// \file   DockAreaWidget.cpp
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Implementation of CDockAreaWidget class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <DockWidgetTab.h>
#include "DockAreaWidget.h"

#include <QStackedLayout>
#include <QScrollBar>
#include <QScrollArea>
#include <QWheelEvent>
#include <QStyle>
#include <QPushButton>
#include <QDebug>
#include <QMenu>
#include <QSplitter>
#include <QXmlStreamWriter>


#include "DockContainerWidget.h"
#include "DockWidget.h"
#include "FloatingDockContainer.h"
#include "DockManager.h"
#include "DockOverlay.h"
#include "DockAreaTabBar.h"


namespace ads
{
static const char* const INDEX_PROPERTY = "index";
static const char* const ACTION_PROPERTY = "action";
static const int APPEND = -1;


/**
 * Private data class of CDockAreaWidget class (pimpl)
 */
struct DockAreaWidgetPrivate
{
	CDockAreaWidget* _this;
	QBoxLayout* Layout;
	QFrame* TitleBar;
	QBoxLayout* TopLayout;
	QStackedLayout* ContentsLayout;
	CDockAreaTabBar* TabBar;
	QWidget* TabsContainerWidget;
	QBoxLayout* TabsLayout;
	QPushButton* TabsMenuButton;
	QPushButton* CloseButton;
	int TabsLayoutInitCount;
	CDockManager* DockManager = nullptr;

	/**
	 * Private data constructor
	 */
	DockAreaWidgetPrivate(CDockAreaWidget* _public);

	/**
	 * Creates the layout for top area with tabs and close button
	 */
	void createTabBar();

	/**
	 * Returns the dock widget with the given index
	 */
	CDockWidget* dockWidgetAt(int index)
	{
		return dynamic_cast<CDockWidget*>(ContentsLayout->widget(index));
	}

	/**
	 * Convenience function to ease title widget access by index
	 */
	CDockWidgetTab* titleWidgetAt(int index)
	{
		return dockWidgetAt(index)->titleBar();
	}

	/**
	 * Adds a tabs menu entry for the given dock widget
	 * If menu is 0, a menu entry is added to the menu of the TabsMenuButton
	 * member. If menu is a valid menu pointer, the entry will be added to
	 * the given menu
	 */
	void addTabsMenuEntry(CDockWidget* DockWidget, int Index = -1, QMenu* menu = 0);

	/**
	 * Returns the tab action of the given dock widget
	 */
	QAction* dockWidgetTabAction(CDockWidget* DockWidget) const
	{
		return qvariant_cast<QAction*>(DockWidget->property(ACTION_PROPERTY));
	}

	/**
	 * Returns the index of the given dock widget
	 */
	int dockWidgetIndex(CDockWidget* DockWidget) const
	{
		return DockWidget->property(INDEX_PROPERTY).toInt();
	}

	/**
	 * Update the tabs menu if dock widget order changed or if dock widget has
	 * been removed
	 */
	void updateTabsMenu();

	/**
	 * Updates the tab bar visibility depending on the number of dock widgets
	 * in this area
	 */
	void updateTabBar();
};
// struct DockAreaWidgetPrivate


//============================================================================
DockAreaWidgetPrivate::DockAreaWidgetPrivate(CDockAreaWidget* _public) :
	_this(_public)
{

}


//============================================================================
void DockAreaWidgetPrivate::createTabBar()
{
	TitleBar = new QFrame(_this);
	TitleBar->setObjectName("dockAreaTitleBar");
	TopLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	TopLayout->setContentsMargins(0, 0, 0, 0);
	TopLayout->setSpacing(0);
	TitleBar->setLayout(TopLayout);
	Layout->addWidget(TitleBar);

	TabBar = new CDockAreaTabBar(_this);
	TopLayout->addWidget(TabBar, 1);

	TabsContainerWidget = new QWidget();
	TabsContainerWidget->setObjectName("tabsContainerWidget");
	TabBar->setWidget(TabsContainerWidget);

	TabsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	TabsLayout->setContentsMargins(0, 0, 0, 0);
	TabsLayout->setSpacing(0);
	TabsLayout->addStretch(1);
	TabsContainerWidget->setLayout(TabsLayout);

	TabsMenuButton = new QPushButton();
	TabsMenuButton->setObjectName("tabsMenuButton");
	TabsMenuButton->setFlat(true);
	TabsMenuButton->setIcon(_this->style()->standardIcon(QStyle::SP_TitleBarUnshadeButton));
	TabsMenuButton->setMaximumWidth(TabsMenuButton->iconSize().width());
	TabsMenuButton->setMenu(new QMenu(TabsMenuButton));
	TopLayout->addWidget(TabsMenuButton, 0);
	TabsMenuButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_this->connect(TabsMenuButton->menu(), SIGNAL(triggered(QAction*)),
		SLOT(onTabsMenuActionTriggered(QAction*)));

	CloseButton = new QPushButton();
	CloseButton->setObjectName("closeButton");
	CloseButton->setFlat(true);
	CloseButton->setIcon(_this->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	CloseButton->setToolTip(_this->tr("Close"));
	CloseButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	TopLayout->addWidget(CloseButton, 0);
	_this->connect(CloseButton, SIGNAL(clicked()), SLOT(onCloseButtonClicked()));

	TabsLayoutInitCount = TabsLayout->count();
}


//============================================================================
void DockAreaWidgetPrivate::updateTabBar()
{
	CDockContainerWidget* Container = _this->dockContainer();
	if (!Container)
	{
		return;
	}

	if (Container->isFloating() && (Container->dockAreaCount() == 1) && (_this->count() == 1))
	{
		TitleBar->setVisible(false);
	}
	else
	{
		TitleBar->setVisible(true);
	}
}


//============================================================================
void DockAreaWidgetPrivate::addTabsMenuEntry(CDockWidget* DockWidget,
	int Index, QMenu* menu)
{
	menu = menu ? menu : TabsMenuButton->menu();
	QAction* Action;
	if (Index >= 0 && Index < menu->actions().count())
	{
		Action = new QAction(DockWidget->icon(), DockWidget->windowTitle());
		menu->insertAction(menu->actions().at(Index), Action);
	}
	else
	{
		Action = menu->addAction(DockWidget->icon(), DockWidget->windowTitle());
	}
	QVariant vAction = QVariant::fromValue(Action);
	DockWidget->setProperty(ACTION_PROPERTY, vAction);
}


//============================================================================
void DockAreaWidgetPrivate::updateTabsMenu()
{
	QMenu* menu = TabsMenuButton->menu();
	menu->clear();
	for (int i = 0; i < ContentsLayout->count(); ++i)
	{
		addTabsMenuEntry(dockWidgetAt(i), APPEND, menu);
	}
}


//============================================================================
CDockAreaWidget::CDockAreaWidget(CDockManager* DockManager, CDockContainerWidget* parent) :
	QFrame(parent),
	d(new DockAreaWidgetPrivate(this))
{
	d->DockManager = DockManager;
	d->Layout = new QBoxLayout(QBoxLayout::TopToBottom);
	d->Layout->setContentsMargins(0, 0, 0, 0);
	d->Layout->setSpacing(0);
	setLayout(d->Layout);

	d->createTabBar();

	d->ContentsLayout = new QStackedLayout();
	d->ContentsLayout->setContentsMargins(0, 0, 0, 0);
	d->ContentsLayout->setSpacing(0);
	d->Layout->addLayout(d->ContentsLayout, 1);
}

//============================================================================
CDockAreaWidget::~CDockAreaWidget()
{
	qDebug() << "~CDockAreaWidget()";
	delete d;
}


//============================================================================
CDockManager* CDockAreaWidget::dockManager() const
{
	return d->DockManager;
}


//============================================================================
CDockContainerWidget* CDockAreaWidget::dockContainer() const
{
	QWidget* Parent = parentWidget();
	while (Parent)
	{
		CDockContainerWidget* Container = dynamic_cast<CDockContainerWidget*>(Parent);
		if (Container)
		{
			return Container;
		}
		Parent = Parent->parentWidget();
	}

	return 0;
}


//============================================================================
void CDockAreaWidget::addDockWidget(CDockWidget* DockWidget)
{
	insertDockWidget(d->ContentsLayout->count(), DockWidget);
}


//============================================================================
void CDockAreaWidget::insertDockWidget(int index, CDockWidget* DockWidget,
	bool Activate)
{
	d->ContentsLayout->insertWidget(index, DockWidget);
	DockWidget->titleBar()->setDockAreaWidget(this);
	auto TitleBar = DockWidget->titleBar();
	d->TabsLayout->insertWidget(index, TitleBar);
	TitleBar->show();
	connect(TitleBar, SIGNAL(clicked()), this, SLOT(onDockWidgetTitleClicked()));
	DockWidget->setProperty(INDEX_PROPERTY, index);
	if (Activate)
	{
		setCurrentIndex(index);
	}
	d->addTabsMenuEntry(DockWidget, index);
	DockWidget->setDockArea(this);
}


//============================================================================
void CDockAreaWidget::removeDockWidget(CDockWidget* DockWidget)
{
	qDebug() << "CDockAreaWidget::removeDockWidget";
	d->ContentsLayout->removeWidget(DockWidget);
	auto TitleBar = DockWidget->titleBar();
	TitleBar->hide();
	d->TabsLayout->removeWidget(TitleBar);
	disconnect(TitleBar, SIGNAL(clicked()), this, SLOT(onDockWidgetTitleClicked()));
	setCurrentIndex(d->ContentsLayout->currentIndex());
	d->updateTabsMenu();

	CDockContainerWidget* DockContainer = dockContainer();
	if (d->ContentsLayout->isEmpty())
	{
		qDebug() << "Dock Area empty";
		dockContainer()->removeDockArea(this);
		this->deleteLater();;
	}

	d->updateTabBar();
	DockWidget->setDockArea(nullptr);
	DockContainer->dumpLayout();
}


//============================================================================
void CDockAreaWidget::onDockWidgetTitleClicked()
{
	CDockWidgetTab* TitleWidget = qobject_cast<CDockWidgetTab*>(sender());
	if (!TitleWidget)
	{
		return;
	}

	int index = d->TabsLayout->indexOf(TitleWidget);
	setCurrentIndex(index);
}


//============================================================================
void CDockAreaWidget::onCloseButtonClicked()
{
	currentDockWidget()->toggleView(false);
}


//============================================================================
CDockWidget* CDockAreaWidget::currentDockWidget() const
{
	return dockWidget(currentIndex());
}


//============================================================================
void CDockAreaWidget::setCurrentDockWidget(CDockWidget* DockWidget)
{
	int Index = tabIndex(DockWidget);
	if (Index < 0)
	{
		return;
	}
	setCurrentIndex(Index);
}


//============================================================================
void CDockAreaWidget::setCurrentIndex(int index)
{
	if (index < 0 || index > (d->TabsLayout->count() - 1))
	{
		qWarning() << Q_FUNC_INFO << "Invalid index" << index;
		return;
    }

    emit currentChanging(index);

	// Set active TAB and update all other tabs to be inactive
	for (int i = 0; i < d->TabsLayout->count(); ++i)
	{
		QLayoutItem* item = d->TabsLayout->itemAt(i);
		if (!item->widget())
		{
			continue;
		}

		auto TitleWidget = dynamic_cast<CDockWidgetTab*>(item->widget());
		if (!TitleWidget)
		{
			continue;
		}

		if (i == index)
		{
			TitleWidget->show();
			TitleWidget->setActiveTab(true);
			d->TabBar->ensureWidgetVisible(TitleWidget);
			auto Features = TitleWidget->dockWidget()->features();
			d->CloseButton->setVisible(Features.testFlag(CDockWidget::DockWidgetClosable));
		}
		else
		{
			TitleWidget->setActiveTab(false);
		}
	}

	d->ContentsLayout->setCurrentIndex(index);
	d->ContentsLayout->currentWidget()->show();
	emit currentChanged(index);
}


//============================================================================
int CDockAreaWidget::currentIndex() const
{
	return d->ContentsLayout->currentIndex();
}


//============================================================================
QRect CDockAreaWidget::titleAreaGeometry() const
{
	return d->TopLayout->geometry();
}

//============================================================================
QRect CDockAreaWidget::contentAreaGeometry() const
{
	return d->ContentsLayout->geometry();
}


//============================================================================
int CDockAreaWidget::tabIndex(CDockWidget* DockWidget)
{
	return d->ContentsLayout->indexOf(DockWidget);
}


//============================================================================
QList<CDockWidget*> CDockAreaWidget::dockWidgets() const
{
	QList<CDockWidget*> DockWidgetList;
	for (int i = 0; i < d->ContentsLayout->count(); ++i)
	{
		DockWidgetList.append(dockWidget(i));
	}
	return DockWidgetList;
}


//============================================================================
QList<CDockWidget*> CDockAreaWidget::openedDockWidgets() const
{
	QList<CDockWidget*> DockWidgetList;
	for (int i = 0; i < d->ContentsLayout->count(); ++i)
	{
		CDockWidget* DockWidget = dockWidget(i);
		if (!DockWidget->isClosed())
		{
			DockWidgetList.append(dockWidget(i));
		}
	}
	return DockWidgetList;
}


//============================================================================
int CDockAreaWidget::indexOfContentByTitlePos(const QPoint& p, QWidget* exclude) const
{
	for (int i = 0; i < d->ContentsLayout->count(); ++i)
	{
		auto TitleWidget = d->titleWidgetAt(i);
		if (TitleWidget->geometry().contains(p) && (!exclude || TitleWidget != exclude))
		{
			return i;
		}
	}
	return -1;
}


//============================================================================
int CDockAreaWidget::count() const
{
	return d->ContentsLayout->count();
}


//============================================================================
CDockWidget* CDockAreaWidget::dockWidget(int Index) const
{
	return dynamic_cast<CDockWidget*>(d->ContentsLayout->widget(Index));
}


//============================================================================
void CDockAreaWidget::reorderDockWidget(int fromIndex, int toIndex)
{
	if (fromIndex >= d->ContentsLayout->count() || fromIndex < 0
     || toIndex >= d->ContentsLayout->count() || toIndex < 0 || fromIndex == toIndex)
	{
		qDebug() << "Invalid index for tab movement" << fromIndex << toIndex;
		d->TabsLayout->update();
		return;
	}

	CDockWidget* DockWidget = dockWidget(fromIndex);

	// reorder tabs menu action to match new order of contents
	auto Menu = d->TabsMenuButton->menu();
	auto TabsAction = d->dockWidgetTabAction(DockWidget);
	Menu->removeAction(TabsAction);
	if (toIndex >= Menu->actions().count())
	{
		Menu->addAction(TabsAction);
	}
	else
	{
		Menu->insertAction(Menu->actions().at(toIndex), TabsAction);
	}


	// now reorder contents and title bars
	QLayoutItem* liFrom = nullptr;
	liFrom = d->TabsLayout->takeAt(fromIndex);
	d->TabsLayout->insertItem(toIndex, liFrom);
	liFrom = d->ContentsLayout->takeAt(fromIndex);
	d->ContentsLayout->insertWidget(toIndex, liFrom->widget());
	delete liFrom;
}


//============================================================================
void CDockAreaWidget::onTabsMenuActionTriggered(QAction* Action)
{
	int Index = d->TabsMenuButton->menu()->actions().indexOf(Action);
	setCurrentIndex(Index);
}


//============================================================================
void CDockAreaWidget::updateDockArea()
{
	d->updateTabBar();
}


//============================================================================
void CDockAreaWidget::saveState(QXmlStreamWriter& s) const
{
	s.writeStartElement("DockAreaWidget");
	s.writeAttribute("Tabs", QString::number(d->ContentsLayout->count()));
	s.writeAttribute("CurrentIndex", QString::number(d->ContentsLayout->currentIndex()));
	qDebug() << "CDockAreaWidget::saveState TabCount: " << d->ContentsLayout->count()
			<< " CurrentIndex: " << d->ContentsLayout->currentIndex();
	for (int i = 0; i < d->ContentsLayout->count(); ++i)
	{
		dockWidget(i)->saveState(s);
	}
	s.writeEndElement();
}

} // namespace ads

//---------------------------------------------------------------------------
// EOF DockAreaWidget.cpp
