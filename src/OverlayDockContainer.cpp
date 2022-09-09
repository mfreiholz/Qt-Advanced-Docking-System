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
#include "OverlayDockContainer.h"
#include "DockManager.h"
#include "DockWidgetSideTab.h"
#include "DockWidgetTab.h"
#include "SideTabBar.h"
#include "DockAreaTitleBar.h"
#include "DockAreaWidget.h"
#include "DockingStateReader.h"

#include <QXmlStreamWriter>
#include <QAbstractButton>
#include <QBoxLayout>
#include <QPainter>
#include <QSplitter>
#include <QPointer>

namespace ads
{
struct OverlayDockContainerPrivate
{
    COverlayDockContainer* _this;
	CDockAreaWidget* DockArea{nullptr};
	CDockWidget* DockWidget{nullptr};
	QPointer<CDockManager> DockManager{nullptr};
	QSplitter* Splitter;
	CDockWidgetSideTab::SideTabBarArea Area;

	/**
	 * Private data constructor
	 */
	OverlayDockContainerPrivate(COverlayDockContainer *_public);
}; // struct OverlayDockContainerPrivate

//============================================================================
OverlayDockContainerPrivate::OverlayDockContainerPrivate(
    COverlayDockContainer *_public) :
	_this(_public)
{

}

CDockContainerWidget* COverlayDockContainer::parentContainer() const
{
	return qobject_cast<CDockContainerWidget*>(parent());
}

//============================================================================
COverlayDockContainer::COverlayDockContainer(CDockManager* DockManager, CDockWidgetSideTab::SideTabBarArea area, CDockContainerWidget* parent) :
    QFrame(parent),
    d(new OverlayDockContainerPrivate(this))
{
	d->DockManager = DockManager;
	d->Area = area;
	d->DockArea = new CDockAreaWidget(DockManager, parent);
	d->DockArea->setObjectName("OverlayDockArea");
	d->DockArea->setOverlayDockContainer(this);
	d->DockArea->updateAutoHidebuttonCheckState();

	QBoxLayout* l = new QBoxLayout(QBoxLayout::LeftToRight);
	d->Splitter = new QSplitter(Qt::Orientation::Horizontal);
	d->Splitter->setObjectName("overlaySplitter");
	d->Splitter->setChildrenCollapsible(false);

	const auto emptyWidget = new QWidget();
	emptyWidget->setMinimumWidth(50); // Prevents you from dragging the splitter too far

	if (area == CDockWidgetSideTab::SideTabBarArea::Left)
	{
        d->Splitter->addWidget(d->DockArea);
        d->Splitter->addWidget(emptyWidget);
	}
	else
	{
        d->Splitter->addWidget(emptyWidget);
        d->Splitter->addWidget(d->DockArea);
	}

	l->setContentsMargins(QMargins());
	l->setSpacing(0);
	l->addWidget(d->Splitter);
	setLayout(l);

	updateMask();
	updateSize();

	parent->registerOverlayWidget(this);

	d->DockArea->installEventFilter(this);
	parent->installEventFilter(this);
}

//============================================================================
void COverlayDockContainer::updateMask()
{
	const auto rect = d->DockArea->frameGeometry();
	const auto topLeft = rect.topLeft();
	const auto handleSize = d->Splitter->handleWidth();
	const auto offset = d->Area == CDockWidgetSideTab::SideTabBarArea::Left ? 0 : handleSize;
    setMask(QRect(QPoint(topLeft.x() - offset, topLeft.y()), QSize(rect.size().width() + handleSize, rect.size().height())));
}

//============================================================================
void COverlayDockContainer::updateSize()
{
	const auto dockContainerParent = parentContainer();
	const auto rootSplitter = dockContainerParent->rootSplitter();
	const auto rect = rootSplitter->frameGeometry();
	move(rect.topLeft());
    resize(rect.width(), rect.height());
}

//============================================================================
COverlayDockContainer::COverlayDockContainer(CDockWidget* DockWidget, CDockWidgetSideTab::SideTabBarArea area, CDockContainerWidget* parent) : 
	COverlayDockContainer(DockWidget->dockManager(), area, parent)
{
	addDockWidget(DockWidget);
}

//============================================================================
COverlayDockContainer::~COverlayDockContainer()
{
	ADS_PRINT("~COverlayDockContainer");

	// Remove event filter in case there are any queued messages
    parent()->removeEventFilter(this);

	if (d->DockManager)
	{
		parentContainer()->removeOverlayWidget(this);
	}

	delete d;
}

//============================================================================
CSideTabBar* COverlayDockContainer::sideTabBar() const
{
	return parentContainer()->sideTabBar(d->Area);
}

//============================================================================
CDockWidget* COverlayDockContainer::dockWidget() const
{
	return d->DockWidget;
}

//============================================================================
void COverlayDockContainer::addDockWidget(CDockWidget* DockWidget)
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

	updateSize();
	updateMask();
}


//============================================================================
CDockWidgetSideTab::SideTabBarArea COverlayDockContainer::sideTabBarArea() const
{
	return d->Area;
}

//============================================================================
CDockAreaWidget* COverlayDockContainer::dockAreaWidget() const
{
	return d->DockArea;
}

//============================================================================
void COverlayDockContainer::moveContentsToParent()
{
	const auto position = mapToGlobal(d->Area == CDockWidgetSideTab::Left ? QPoint(1,height() / 2) : QPoint(width() - 1, height() / 2));

	const auto dockAreaWidget = parentContainer()->dockAreaAt(position);
	if (dockAreaWidget != nullptr && !dockAreaWidget->isCentralWidgetArea())
	{
        parentContainer()->addDockWidget(CenterDockWidgetArea, d->DockWidget, dockAreaWidget);
	}
	else
	{
        parentContainer()->addDockWidget(d->Area == CDockWidgetSideTab::Left ? LeftDockWidgetArea : RightDockWidgetArea, d->DockWidget);
	}
	cleanupAndDelete();
}


//============================================================================
void COverlayDockContainer::cleanupAndDelete()
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
void COverlayDockContainer::saveState(QXmlStreamWriter& s)
{
    s.writeAttribute("SideTabBarArea", QString::number(sideTabBarArea())); 
	QStringList Sizes;
    for (auto Size : d->Splitter->sizes())
    {
		Sizes << QString::number(Size);
    }

    s.writeAttribute("Sizes", Sizes.join(" "));
}


//============================================================================
bool COverlayDockContainer::restoreState(CDockingStateReader& s, bool Testing)
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

	if (Sizes.count() != d->Splitter->count())
	{
		return false;
	}

	if (!Testing)
	{
		d->Splitter->setSizes(Sizes);
	}

	return true;
}

void COverlayDockContainer::toggleView(bool Enable)
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
	}
}

void COverlayDockContainer::collapseView(bool Enable)
{
	if (Enable)
	{
		hide();
		d->DockArea->hide();
		d->DockWidget->hide();
	}
	else
	{
		show();
		d->DockArea->show();
		d->DockWidget->show();
	}
}


//============================================================================
bool COverlayDockContainer::areaExistsInConfig(CDockWidgetSideTab::SideTabBarArea area)
{
	if (area == CDockWidgetSideTab::Left && !CDockManager::testConfigFlag(CDockManager::DockContainerHasLeftSideBar))
	{
		return false;
	}
	if (area == CDockWidgetSideTab::Right && !CDockManager::testConfigFlag(CDockManager::DockContainerHasRightSideBar))
	{
		return false;
	}

	return true;
}


//============================================================================
bool COverlayDockContainer::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::Resize)
	{
		updateSize();
		updateMask();
	}
	return QWidget::eventFilter(watched, event);
}


//============================================================================
void COverlayDockContainer::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
}


//============================================================================
void COverlayDockContainer::resizeEvent(QResizeEvent* event)
{
	updateMask();
    QWidget::resizeEvent(event);
}
}

