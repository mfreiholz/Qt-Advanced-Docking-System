//============================================================================
/// \file   FloatingOverlay.cpp
/// \author Uwe Kindler
/// \date   26.11.2019
/// \brief  Implementation of CFloatingOverlay
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "FloatingOverlay.h"

#include <iostream>

#include <QEvent>
#include <QApplication>
#include <QPainter>

#include "DockWidget.h"
#include "DockAreaWidget.h"
#include "DockManager.h"
#include "DockContainerWidget.h"
#include "DockOverlay.h"

namespace ads
{

/**
 * Private data class (pimpl)
 */
struct FloatingOverlayPrivate
{
	CFloatingOverlay *_this;
	QWidget* Content;
	CDockAreaWidget* ContentSourceArea = nullptr;
	CDockContainerWidget* ContenSourceContainer = nullptr;
	QPoint DragStartMousePosition;
	CDockManager* DockManager;
	CDockContainerWidget *DropContainer = nullptr;
	qreal WindowOpacity;
	bool Hidden = false;
	bool IgnoreMouseEvents = false;


	/**
	 * Private data constructor
	 */
	FloatingOverlayPrivate(CFloatingOverlay *_public);
	void updateDropOverlays(const QPoint &GlobalPos);

	void setHidden(bool Value)
	{
		Hidden = Value;
		_this->update();
	}
};
// struct LedArrayPanelPrivate


//============================================================================
void FloatingOverlayPrivate::updateDropOverlays(const QPoint &GlobalPos)
{
	if (!_this->isVisible() || !DockManager)
	{
		return;
	}

	auto Containers = DockManager->dockContainers();
	CDockContainerWidget *TopContainer = nullptr;
	for (auto ContainerWidget : Containers)
	{
		if (!ContainerWidget->isVisible())
		{
			continue;
		}

		/*if (DockContainer == ContainerWidget)
		{
			continue;
		}*/

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
	auto DockDropArea = DockAreaOverlay->dropAreaUnderCursor();
	auto ContainerDropArea = ContainerOverlay->dropAreaUnderCursor();

	if (!TopContainer)
	{
		ContainerOverlay->hideOverlay();
		DockAreaOverlay->hideOverlay();
		setHidden(false);
		return;
	}

	int VisibleDockAreas = TopContainer->visibleDockAreaCount();
	ContainerOverlay->setAllowedAreas(
	    VisibleDockAreas > 1 ? OuterDockAreas : AllDockAreas);
	DockWidgetArea ContainerArea = ContainerOverlay->showOverlay(TopContainer);
	ContainerOverlay->enableDropPreview(ContainerArea != InvalidDockWidgetArea);
	auto DockArea = TopContainer->dockAreaAt(GlobalPos);
	if (DockArea && DockArea->isVisible() && VisibleDockAreas > 0 && DockArea != ContentSourceArea)
	{
		DockAreaOverlay->enableDropPreview(true);
		DockAreaOverlay->setAllowedAreas(
		    (VisibleDockAreas == 1) ? NoDockWidgetArea : AllDockAreas);
		DockWidgetArea Area = DockAreaOverlay->showOverlay(DockArea);

		// A CenterDockWidgetArea for the dockAreaOverlay() indicates that
		// the mouse is in the title bar. If the ContainerArea is valid
		// then we ignore the dock area of the dockAreaOverlay() and disable
		// the drop preview
		if ((Area == CenterDockWidgetArea)
		    && (ContainerArea != InvalidDockWidgetArea))
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
		if (DockArea == ContentSourceArea && InvalidDockWidgetArea == ContainerDropArea)
		{
			DropContainer = nullptr;
		}
	}

	setHidden(DockDropArea != InvalidDockWidgetArea || ContainerDropArea != InvalidDockWidgetArea);
}


//============================================================================
FloatingOverlayPrivate::FloatingOverlayPrivate(CFloatingOverlay *_public) :
	_this(_public)
{

}

//============================================================================
CFloatingOverlay::CFloatingOverlay(QWidget* Content, QWidget* parent) :
	QFrame(parent),
	d(new FloatingOverlayPrivate(this))
{
	d->Content = Content;
	setAttribute(Qt::WA_DeleteOnClose);

	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	setWindowOpacity(1);
	setWindowTitle("FloatingOverlay");
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_TranslucentBackground);
	// We install an event filter to detect mouse release events because we
	// do not receive mouse release event if the floating widget is behind
	// the drop overlay cross
	qApp->installEventFilter(this);
}


//============================================================================
CFloatingOverlay::CFloatingOverlay(CDockWidget* Content)
	: CFloatingOverlay((QWidget*)Content, Content->dockManager())
{
	d->DockManager = Content->dockManager();
	if (Content->dockAreaWidget()->openDockWidgetsCount() == 1)
	{
		d->ContentSourceArea = Content->dockAreaWidget();
		d->ContenSourceContainer = Content->dockContainer();
	}
}


//============================================================================
CFloatingOverlay::CFloatingOverlay(CDockAreaWidget* Content)
	: CFloatingOverlay((QWidget*)Content, Content->dockManager())
{
	d->DockManager = Content->dockManager();
	d->ContentSourceArea = Content;
	d->ContenSourceContainer = Content->dockContainer();
}


//============================================================================
CFloatingOverlay::~CFloatingOverlay()
{
	delete d;
}


//============================================================================
void CFloatingOverlay::moveFloating()
{
	int BorderSize = (frameSize().width() - size().width()) / 2;
	const QPoint moveToPos = QCursor::pos() - d->DragStartMousePosition
	    - QPoint(BorderSize, 0);
	move(moveToPos);
}


//============================================================================
void CFloatingOverlay::startFloating(const QPoint &DragStartMousePos,
    const QSize &Size, eDragState DragState, QWidget *MouseEventHandler)
{
	Q_UNUSED(MouseEventHandler)
	Q_UNUSED(DragState)
	resize(Size);
	d->DragStartMousePosition = DragStartMousePos;
	moveFloating();
	show();

}


//============================================================================
void CFloatingOverlay::moveEvent(QMoveEvent *event)
{
	std::cout << "CFloatingOverlay::moveEvent" << std::endl;
	QWidget::moveEvent(event);
	d->updateDropOverlays(QCursor::pos());
}


//============================================================================
bool CFloatingOverlay::eventFilter(QObject *watched, QEvent *event)
{
	Q_UNUSED(watched);
	if (event->type() == QEvent::MouseButtonRelease && !d->IgnoreMouseEvents)
	{
		ADS_PRINT("FloatingWidget::eventFilter QEvent::MouseButtonRelease");
		std::cout << "CFloatingOverlay::eventFilter QEvent::MouseButtonRelease" << std::endl;

		if (d->DropContainer)
		{
			d->DropContainer->dropWidget(d->Content, QCursor::pos());
			d->DropContainer = nullptr;
		}
		else
		{
			CDockWidget* DockWidget = qobject_cast<CDockWidget*>(d->Content);
			CFloatingDockContainer* FloatingWidget;
			if (DockWidget)
			{
				FloatingWidget = new CFloatingDockContainer(DockWidget);
			}
			else
			{
				CDockAreaWidget* DockArea = qobject_cast<CDockAreaWidget*>(d->Content);
				FloatingWidget = new CFloatingDockContainer(DockArea);
			}
			FloatingWidget->setGeometry(this->geometry());
			FloatingWidget->show();
		}

		this->close();
		d->DockManager->containerOverlay()->hideOverlay();
		d->DockManager->dockAreaOverlay()->hideOverlay();
		d->IgnoreMouseEvents = true;
	}

	return false;
}


//============================================================================
void CFloatingOverlay::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);

	if (d->Hidden)
	{
		return;
	}

	QRect r = rect();
	QPainter painter(this);
    QColor Color = palette().color(QPalette::Active, QPalette::Highlight);
    QPen Pen = painter.pen();
    Pen.setColor(Color.darker(120));
    Pen.setStyle(Qt::SolidLine);
    Pen.setWidth(1);
    Pen.setCosmetic(true);
    painter.setPen(Pen);
    Color = Color.lighter(130);
    Color.setAlpha(64);
    painter.setBrush(Color);
	painter.drawRect(r.adjusted(0, 0, -1, -1));
}



} // namespace ads

//---------------------------------------------------------------------------
// EOF FloatingOverlay.cpp
