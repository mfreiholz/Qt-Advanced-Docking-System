//============================================================================
/// \file   DockAreaTabBar.cpp
/// \author Uwe Kindler
/// \date   24.08.2018
/// \brief  Implementation of CDockAreaTabBar class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockAreaTabBar.h"

#include <QMouseEvent>
#include <QScrollBar>
#include <QDebug>
#include <QBoxLayout>

#include "FloatingDockContainer.h"
#include "DockAreaWidget.h"
#include "DockOverlay.h"
#include "DockManager.h"
#include "DockWidget.h"
#include "DockWidgetTab.h"

#include <iostream>

namespace ads
{
/**
 * Private data class of CDockAreaTabBar class (pimpl)
 */
struct DockAreaTabBarPrivate
{
	CDockAreaTabBar* _this;
	QPoint DragStartMousePos;
	CDockAreaWidget* DockArea;
	CFloatingDockContainer* FloatingWidget = nullptr;
	QWidget* TabsContainerWidget;
	QBoxLayout* TabsLayout;
	int CurrentIndex = -1;

	/**
	 * Private data constructor
	 */
	DockAreaTabBarPrivate(CDockAreaTabBar* _public);
};
// struct DockAreaTabBarPrivate

//============================================================================
DockAreaTabBarPrivate::DockAreaTabBarPrivate(CDockAreaTabBar* _public) :
	_this(_public)
{

}

//============================================================================
CDockAreaTabBar::CDockAreaTabBar(CDockAreaWidget* parent) :
	QScrollArea(parent),
	d(new DockAreaTabBarPrivate(this))
{
	d->DockArea = parent;
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
	setFrameStyle(QFrame::NoFrame);
	setWidgetResizable(true);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	d->TabsContainerWidget = new QWidget();
	d->TabsContainerWidget->setObjectName("tabsContainerWidget");
	setWidget(d->TabsContainerWidget);

	d->TabsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	d->TabsLayout->setContentsMargins(0, 0, 0, 0);
	d->TabsLayout->setSpacing(0);
	d->TabsLayout->addStretch(1);
	d->TabsContainerWidget->setLayout(d->TabsLayout);
}

//============================================================================
CDockAreaTabBar::~CDockAreaTabBar()
{
	delete d;
}


//============================================================================
void CDockAreaTabBar::wheelEvent(QWheelEvent* Event)
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


//============================================================================
void CDockAreaTabBar::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		ev->accept();
		d->DragStartMousePos = ev->pos();
		return;
	}
	QScrollArea::mousePressEvent(ev);
}


//============================================================================
void CDockAreaTabBar::mouseReleaseEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		qDebug() << "CTabsScrollArea::mouseReleaseEvent";
		ev->accept();
		d->FloatingWidget = nullptr;
		d->DragStartMousePos = QPoint();
		return;
	}
	QScrollArea::mouseReleaseEvent(ev);
}


//============================================================================
void CDockAreaTabBar::mouseMoveEvent(QMouseEvent* ev)
{
	QScrollArea::mouseMoveEvent(ev);
	if (ev->buttons() != Qt::LeftButton)
	{
		return;
	}

	if (d->FloatingWidget)
	{
		d->FloatingWidget->moveFloating();
		return;
	}

	// If this is the last dock area in a dock container it does not make
	// sense to move it to a new floating widget and leave this one
	// empty
	if (d->DockArea->dockContainer()->isFloating()
	 && d->DockArea->dockContainer()->visibleDockAreaCount() == 1)
	{
		return;
	}

	if (!this->geometry().contains(ev->pos()))
	{
		qDebug() << "CTabsScrollArea::startFloating";
		startFloating(d->DragStartMousePos);
		auto Overlay = d->DockArea->dockManager()->containerOverlay();
		Overlay->setAllowedAreas(OuterDockAreas);
	}

	return;
}


//============================================================================
void CDockAreaTabBar::mouseDoubleClickEvent(QMouseEvent *event)
{
	// If this is the last dock area in a dock container it does not make
	// sense to move it to a new floating widget and leave this one
	// empty
	if (d->DockArea->dockContainer()->isFloating() && d->DockArea->dockContainer()->dockAreaCount() == 1)
	{
		return;
	}
	startFloating(event->pos());
}


//============================================================================
void CDockAreaTabBar::startFloating(const QPoint& Pos)
{
	QSize Size = d->DockArea->size();
	CFloatingDockContainer* FloatingWidget = new CFloatingDockContainer(d->DockArea);
	FloatingWidget->startFloating(Pos, Size);
	d->FloatingWidget = FloatingWidget;
	auto TopLevelDockWidget = d->FloatingWidget->topLevelDockWidget();
	if (TopLevelDockWidget)
	{
		TopLevelDockWidget->emitTopLevelChanged(true);
	}
}


//============================================================================
void CDockAreaTabBar::setCurrentIndex(int index)
{
	if (index == d->CurrentIndex)
	{
		return;
	}

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

		auto TabWidget = dynamic_cast<CDockWidgetTab*>(item->widget());
		if (!TabWidget)
		{
			continue;
		}

		if (i == index)
		{
			TabWidget->show();
			TabWidget->setActiveTab(true);
			ensureWidgetVisible(TabWidget);
		}
		else
		{
			TabWidget->setActiveTab(false);
		}
	}

	d->CurrentIndex = index;
	emit currentChanged(index);
}


//============================================================================
int CDockAreaTabBar::count() const
{
	return d->TabsLayout->count();
}


//===========================================================================
void CDockAreaTabBar::insertTab(int Index, CDockWidgetTab* Tab)
{
	d->TabsLayout->insertWidget(Index, Tab);
	connect(Tab, SIGNAL(clicked()), this, SLOT(onTabClicked()));
}


//===========================================================================
void CDockAreaTabBar::removeTab(CDockWidgetTab* Tab)
{
	d->TabsLayout->removeWidget(Tab);
	disconnect(Tab, SIGNAL(clicked()), this, SLOT(onTabClicked()));
}


//===========================================================================
int CDockAreaTabBar::currentIndex() const
{
	return d->CurrentIndex;
}


//===========================================================================
CDockWidgetTab* CDockAreaTabBar::currentTab() const
{
	return qobject_cast<CDockWidgetTab*>(d->TabsLayout->itemAt(d->CurrentIndex)->widget());
}


//===========================================================================
void CDockAreaTabBar::onTabClicked()
{
	CDockWidgetTab* Tab = qobject_cast<CDockWidgetTab*>(sender());
	if (!Tab)
	{
		return;
	}

	int index = d->TabsLayout->indexOf(Tab);
	setCurrentIndex(index);
	std::cout << "emit tabBarClicked " << index << std::endl;
 	emit tabBarClicked(index);
}


void CDockAreaTabBar::closeTabe(int Index)
{

}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockAreaTabBar.cpp
