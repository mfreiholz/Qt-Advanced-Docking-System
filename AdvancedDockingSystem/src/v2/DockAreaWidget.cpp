/*******************************************************************************
** QtAdcancedDockingSystem
** Copyright (C) 2017 Uwe Kindler
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program. If not, see <http://www.gnu.org/licenses/>.
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
#include "DockAreaWidget.h"

#include <QStackedLayout>
#include <QScrollBar>
#include <QScrollArea>
#include <QWheelEvent>
#include <QStyle>
#include <QPushButton>
#include <QDebug>

#include "DockContainerWidget.h"
#include "DockWidget.h"
#include "DockWidgetTitleBar.h"

#include <iostream>

namespace ads
{
/**
 * Custom scroll bar implementation for dock area tab bar
 */
class CTabsScrollArea : public QScrollArea
{
public:
	CTabsScrollArea(QWidget* parent = nullptr)
		: QScrollArea(parent)
	{
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
		setFrameStyle(QFrame::NoFrame);
		setWidgetResizable(true);
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}

protected:
	virtual void wheelEvent(QWheelEvent* Event)
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
}; // class CTabsScrollArea


/**
 * Private data class of CDockAreaWidget class (pimpl)
 */
struct DockAreaWidgetPrivate
{
	CDockAreaWidget* _this;
	QBoxLayout* Layout;
	QBoxLayout* TopLayout;
	QStackedLayout* ContentsLayout;
	QScrollArea* TabsScrollArea;
	QWidget* TabsContainerWidget;
	QBoxLayout* TabsLayout;
	QPushButton* TabsMenuButton;
	QPushButton* CloseButton;
	int TabsLayoutInitCount;

	/**
	 * Private data constructor
	 */
	DockAreaWidgetPrivate(CDockAreaWidget* _public);

	/**
	 * Creates the layout for top area with tabs and close button
	 */
	void createTabBar();
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
	TopLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	TopLayout->setContentsMargins(0, 0, 0, 0);
	TopLayout->setSpacing(0);
	Layout->addLayout(TopLayout);

	TabsScrollArea = new CTabsScrollArea(_this);
	TopLayout->addWidget(TabsScrollArea, 1);

	TabsContainerWidget = new QWidget();
	TabsContainerWidget->setObjectName("tabsContainerWidget");
	TabsScrollArea->setWidget(TabsContainerWidget);

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
	TopLayout->addWidget(TabsMenuButton, 0);

	CloseButton = new QPushButton();
	CloseButton->setObjectName("closeButton");
	CloseButton->setFlat(true);
	CloseButton->setIcon(_this->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	CloseButton->setToolTip(_this->tr("Close"));
	CloseButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	TopLayout->addWidget(CloseButton, 0);
	//connect(_closeButton, SIGNAL(clicked(bool)), this, SLOT(onCloseButtonClicked()));

	TabsLayoutInitCount = TabsLayout->count();
}

//============================================================================
CDockAreaWidget::CDockAreaWidget(CDockManager* DockManager, CDockContainerWidget* parent) :
	QFrame(parent),
	d(new DockAreaWidgetPrivate(this))
{
	setStyleSheet("ads--CDockAreaWidget {border: 1px solid white;}");
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
	delete d;
}


//============================================================================
CDockContainerWidget* CDockAreaWidget::dockContainerWidget() const
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
	d->ContentsLayout->addWidget(DockWidget);
	auto TitleBar = DockWidget->titleBar();
	d->TabsLayout->insertWidget(d->TabsLayout->count() - d->TabsLayoutInitCount,
		TitleBar);
	connect(TitleBar, SIGNAL(clicked()), this, SLOT(onDockWidgetTitleClicked()));
	// if this is the first tab, then activate it
	if (d->ContentsLayout->count() == 1)
	{
		setCurrentIndex(0);
	}
}


//============================================================================
void CDockAreaWidget::onDockWidgetTitleClicked()
{
	CDockWidgetTitleBar* TitleWidget = qobject_cast<CDockWidgetTitleBar*>(sender());
	if (!TitleWidget)
	{
		return;
	}

	int index = d->TabsLayout->indexOf(TitleWidget);
	setCurrentIndex(index);
}


//============================================================================
void CDockAreaWidget::setCurrentIndex(int index)
{
	if (index < 0 || index > (d->TabsLayout->count() - 1))
	{
		qWarning() << Q_FUNC_INFO << "Invalid index" << index;
		return;
	}

	// Set active TAB and update all other tabs to be inactive
	for (int i = 0; i < d->TabsLayout->count(); ++i)
	{
		QLayoutItem* item = d->TabsLayout->itemAt(i);
		if (!item->widget())
		{
			continue;
		}

		auto TitleWidget = dynamic_cast<CDockWidgetTitleBar*>(item->widget());
		if (!TitleWidget)
		{
			continue;
		}

		if (i == index)
		{
			TitleWidget->setActiveTab(true);
			d->TabsScrollArea->ensureWidgetVisible(TitleWidget);
			auto Features = TitleWidget->dockWidget()->features();
			d->CloseButton->setEnabled(Features.testFlag(CDockWidget::DockWidgetClosable));
		}
		else
		{
			TitleWidget->setActiveTab(false);
		}
	}

	d->ContentsLayout->setCurrentIndex(index);
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockAreaWidget.cpp
