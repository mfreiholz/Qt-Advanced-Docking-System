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
/// \file   FloatingDockContainer.cpp
/// \author Uwe Kindler
/// \date   01.03.2017
/// \brief  Implementation of CFloatingDockContainer class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "FloatingDockContainer.h"

#include <QBoxLayout>
#include <QApplication>
#include <QMouseEvent>
#include <QPointer>
#include <QAction>
#include <QDebug>

#include "DockContainerWidget.h"
#include "DockAreaWidget.h"
#include "DockManager.h"
#include "DockWidget.h"
#include "DockOverlay.h"

#include <iostream>

namespace ads
{
static unsigned int zOrderCounter = 0;

/**
 * Private data class of CFloatingDockContainer class (pimpl)
 */
struct FloatingDockContainerPrivate
{
	CFloatingDockContainer* _this;
	CDockContainerWidget* DockContainer;
	unsigned int zOrderIndex = ++zOrderCounter;
	QPointer<CDockManager> DockManager;
	bool DraggingActive = false;
	bool NonClientAreaMouseButtonPress = false;
	QPoint DragStartMousePosition;
	CDockContainerWidget* DropContainer = nullptr;
	CDockAreaWidget* SingleDockArea = nullptr;

	/**
	 * Private data constructor
	 */
	FloatingDockContainerPrivate(CFloatingDockContainer* _public);

	void titleMouseReleaseEvent();
	void updateDropOverlays(const QPoint& GlobalPos);
	void setDraggingActive(bool Active);
};
// struct FloatingDockContainerPrivate

//============================================================================
FloatingDockContainerPrivate::FloatingDockContainerPrivate(CFloatingDockContainer* _public) :
	_this(_public)
{

}



//============================================================================
void FloatingDockContainerPrivate::titleMouseReleaseEvent()
{
	setDraggingActive(false);
	if (!DropContainer)
	{
		return;
	}

	if (DockManager->dockAreaOverlay()->dropAreaUnderCursor() != InvalidDockWidgetArea
	 || DockManager->containerOverlay()->dropAreaUnderCursor() != InvalidDockWidgetArea)
	{
		// Resize the floating widget to the size of the highlighted drop area
		// rectangle
		QRect Rect = DockManager->containerOverlay()->dropOverlayRect();
		if (!Rect.isValid())
		{
			Rect = DockManager->dockAreaOverlay()->rect();
		}

		if (Rect.isValid())
		{
			_this->resize(Rect.size());
		}
		DropContainer->dropFloatingWidget(_this, QCursor::pos());
	}

	DockManager->containerOverlay()->hideOverlay();
	DockManager->dockAreaOverlay()->hideOverlay();
}



//============================================================================
void FloatingDockContainerPrivate::updateDropOverlays(const QPoint& GlobalPos)
{
	if (!_this->isVisible() || !DockManager)
	{
		return;
	}

    auto Containers = DockManager->dockContainers();
    CDockContainerWidget* TopContainer = nullptr;
    for (auto ContainerWidget : Containers)
    {
    	if (!ContainerWidget->isVisible())
    	{
    		continue;
    	}

    	if (DockContainer == ContainerWidget)
    	{
    		continue;
    	}

    	QPoint MappedPos = ContainerWidget->mapFromGlobal(GlobalPos);
    	if (ContainerWidget->rect().contains(MappedPos))
    	{
    		if (!TopContainer || ContainerWidget->isInFrontOf(TopContainer))
    		{
    			TopContainer = ContainerWidget;
    		}
    	}
    }

    DropContainer = TopContainer;
    auto ContainerOverlay = DockManager->containerOverlay();
    auto DockAreaOverlay = DockManager->dockAreaOverlay();

    if (!TopContainer)
    {
    	ContainerOverlay->hideOverlay();
    	DockAreaOverlay->hideOverlay();
    	return;
    }

    int VisibleDockAreas = TopContainer->visibleDockAreaCount();
    ContainerOverlay->setAllowedAreas(VisibleDockAreas > 1 ?
    	OuterDockAreas : AllDockAreas);
	DockWidgetArea ContainerArea = ContainerOverlay->showOverlay(TopContainer);

    auto DockArea = TopContainer->dockAreaAt(GlobalPos);
    if (DockArea && DockArea->isVisible() && VisibleDockAreas > 0)
    {
    	DockAreaOverlay->enableDropPreview(true);
    	DockAreaOverlay->setAllowedAreas((VisibleDockAreas == 1) ?
    		NoDockWidgetArea : AllDockAreas);
        DockWidgetArea Area = DockAreaOverlay->showOverlay(DockArea);

        // A CenterDockWidgetArea for the dockAreaOverlay() indicates that
        // the mouse is in the title bar. If the ContainerArea is valid
        // then we ignore the dock area of the dockAreaOverlay() and disable
        // the drop preview
        if ((Area == CenterDockWidgetArea) && (ContainerArea != InvalidDockWidgetArea))
        {
        	DockAreaOverlay->enableDropPreview(false);
        	ContainerOverlay->enableDropPreview(true);
        }
        else
        {
            ContainerOverlay->enableDropPreview(InvalidDockWidgetArea == Area);
        }
    }
    else
    {
    	DockAreaOverlay->hideOverlay();
    }
}


//============================================================================
void FloatingDockContainerPrivate::setDraggingActive(bool Active)
{
	DraggingActive = Active;
	if (!DraggingActive)
	{
		NonClientAreaMouseButtonPress = false;
	}
}


//============================================================================
CFloatingDockContainer::CFloatingDockContainer(CDockManager* DockManager) :
	QWidget(DockManager, Qt::Window),
	d(new FloatingDockContainerPrivate(this))
{
	//setAttribute(Qt::WA_DeleteOnClose);
	d->DockManager = DockManager;
    QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    setLayout(l);

    d->DockContainer = new CDockContainerWidget(DockManager, this);
    connect(d->DockContainer, SIGNAL(dockAreasAdded()), this, SLOT(onDockAreasAddedOrRemoved()));
    connect(d->DockContainer, SIGNAL(dockAreasRemoved()), this, SLOT(onDockAreasAddedOrRemoved()));
	l->addWidget(d->DockContainer);
	DockManager->registerFloatingWidget(this);

	// We install an event filter to detect mouse release events because we
	// do not receive mouse release event if the floating widget is behind
	// the drop overlay cross
	qApp->installEventFilter(this);
}


//============================================================================
CFloatingDockContainer::CFloatingDockContainer(CDockAreaWidget* DockArea) :
	CFloatingDockContainer(DockArea->dockManager())
{
	d->DockContainer->addDockArea(DockArea);
}


//============================================================================
CFloatingDockContainer::CFloatingDockContainer(CDockWidget* DockWidget) :
	CFloatingDockContainer(DockWidget->dockManager())
{
	d->DockContainer->addDockWidget(CenterDockWidgetArea, DockWidget);
}

//============================================================================
CFloatingDockContainer::~CFloatingDockContainer()
{
	qDebug() << "~CFloatingDockContainer";
	if (d->DockManager)
	{
		d->DockManager->removeFloatingWidget(this);
	}
	delete d;
}


//============================================================================
CDockContainerWidget* CFloatingDockContainer::dockContainer() const
{
	return d->DockContainer;
}


//============================================================================
void CFloatingDockContainer::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);
	if ((event->type() == QEvent::ActivationChange) && isActiveWindow())
    {
		qDebug() << "FloatingWidget::changeEvent QEvent::ActivationChange ";
		d->zOrderIndex = ++zOrderCounter;
        return;
    }
}


//============================================================================
void CFloatingDockContainer::moveEvent(QMoveEvent *event)
{
	QWidget::moveEvent(event);
	if (!qApp->mouseButtons().testFlag(Qt::LeftButton))
	{
		if (d->DraggingActive)
		{
			d->setDraggingActive(false);
		}
		return;
	}

	if (d->DraggingActive)
	{
		d->updateDropOverlays(QCursor::pos());
	}
}


//============================================================================
void CFloatingDockContainer::closeEvent(QCloseEvent *event)
{
	std::cout << "closeEvent" << std::endl;
	d->setDraggingActive(false);
	QWidget::closeEvent(event);
}


//============================================================================
void CFloatingDockContainer::hideEvent(QHideEvent *event)
{
	std::cout << "hideEvent" << std::endl;
	QWidget::hideEvent(event);
	auto OpenDockAreas = d->DockContainer->openedDockAreas();
	for (auto DockArea : OpenDockAreas)
	{
		auto OpenDockWidgets = DockArea->openedDockWidgets();
		for (auto DockWidget : OpenDockWidgets)
		{
			DockWidget->toggleView(false);
		}
	}
}


//============================================================================
void CFloatingDockContainer::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);
	CDockContainerWidget* DockContainer = dockContainer();
	for (int i = 0; i < DockContainer->dockAreaCount(); ++i)
	{
		auto DockArea = DockContainer->dockArea(i);
		for (auto DockWidget : DockArea->openedDockWidgets())
		{
			DockWidget->setToggleViewActionChecked(true);
		}
	}
}


//============================================================================
bool CFloatingDockContainer::event(QEvent *e)
{
	if (e->type() == QEvent::NonClientAreaMouseButtonPress)
	{
		if (QGuiApplication::mouseButtons() == Qt::LeftButton)
		{
			qDebug() << "FloatingWidget::event Event::NonClientAreaMouseButtonPress" << e->type();
			d->setDraggingActive(true);
			d->NonClientAreaMouseButtonPress = true;
		}
	}
	else if (e->type() == QEvent::NonClientAreaMouseButtonDblClick)
	{
		qDebug() << "FloatingWidget::event QEvent::NonClientAreaMouseButtonDblClick";
		d->setDraggingActive(false);
	}
	else if ((e->type() == QEvent::NonClientAreaMouseButtonRelease) && d->DraggingActive)
	{
		qDebug() << "FloatingWidget::event QEvent::NonClientAreaMouseButtonRelease";
		d->titleMouseReleaseEvent();
	}
	else if (d->NonClientAreaMouseButtonPress && (e->type() == QEvent::Resize))
	{
		// If user resizes the floating widget, we do not want to show any
		// drop overlays or drop overlay icons
		d->setDraggingActive(false);
	}

	return QWidget::event(e);
}


//============================================================================
bool CFloatingDockContainer::eventFilter(QObject *watched, QEvent *event)
{
	Q_UNUSED(watched);
	if (event->type() == QEvent::MouseButtonRelease && d->DraggingActive)
	{
		qDebug() << "FloatingWidget::eventFilter QEvent::MouseButtonRelease";
		d->titleMouseReleaseEvent();
	}

	return false;
}


//============================================================================
void CFloatingDockContainer::startFloating(const QPoint& Pos, const QSize& Size)
{
	resize(Size);
	d->setDraggingActive(true);
	QPoint TargetPos = QCursor::pos() - Pos;
	move(TargetPos);
    show();
	d->DragStartMousePosition = Pos;
}


//============================================================================
void CFloatingDockContainer::moveFloating()
{
	int BorderSize = (frameSize().width() - size().width()) / 2;
	const QPoint moveToPos = QCursor::pos() - d->DragStartMousePosition - QPoint(BorderSize, 0);
	move(moveToPos);
}


//============================================================================
void CFloatingDockContainer::onDockAreasAddedOrRemoved()
{
	qDebug() << "CFloatingDockContainer::onDockAreasAddedOrRemoved()";
	if (d->DockContainer->dockAreaCount() == 1)
	{
		d->SingleDockArea = d->DockContainer->dockArea(0);
		this->setWindowTitle(d->SingleDockArea->currentDockWidget()->windowTitle());
		connect(d->SingleDockArea, SIGNAL(currentChanged(int)), this,
			SLOT(onDockAreaCurrentChanged(int)));
	}
	else
	{
		if (d->SingleDockArea)
		{
			disconnect(d->SingleDockArea, SIGNAL(currentChanged(int)), this,
				SLOT(onDockAreaCurrentChanged(int)));
			d->SingleDockArea = nullptr;
		}
		this->setWindowTitle(qApp->applicationDisplayName());
	}
}


//============================================================================
void CFloatingDockContainer::onDockAreaCurrentChanged(int Index)
{
	Q_UNUSED(Index);
	this->setWindowTitle(d->SingleDockArea->currentDockWidget()->windowTitle());
}


//============================================================================
bool CFloatingDockContainer::restoreState(QXmlStreamReader& Stream, bool Testing)
{
	if (!d->DockContainer->restoreState(Stream, Testing))
	{
		return false;
	}
	onDockAreasAddedOrRemoved();
	return true;
}


} // namespace ads

//---------------------------------------------------------------------------
// EOF FloatingDockContainer.cpp
