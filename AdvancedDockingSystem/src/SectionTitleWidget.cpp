#include "ads/SectionTitleWidget.h"

#include <QString>
#include <QApplication>
#include <QBoxLayout>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QCursor>
#include <QStyle>
#include <QSplitter>
#include <QPushButton>

#ifdef ADS_ANIMATIONS_ENABLED
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#endif

#include "ads/Internal.h"
#include "ads/DropOverlay.h"
#include "ads/SectionContent.h"
#include "ads/SectionWidget.h"
#include "ads/FloatingWidget.h"
#include <ads/MainContainerWidget.h>

#include <iostream>

ADS_NAMESPACE_BEGIN

SectionTitleWidget::SectionTitleWidget(SectionContent::RefPtr content, QWidget* parent) :
	QFrame(parent),
    m_Content(content),
    m_TabMoving(false),
    m_IsActiveTab(false)
{
	QBoxLayout* l = new QBoxLayout(QBoxLayout::LeftToRight);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addWidget(content->titleWidget());
	setLayout(l);
}


SectionTitleWidget::~SectionTitleWidget()
{
    layout()->removeWidget(m_Content->titleWidget());
}

bool SectionTitleWidget::isActiveTab() const
{
    return m_IsActiveTab;
}

void SectionTitleWidget::setActiveTab(bool active)
{
    if (active != m_IsActiveTab)
	{
        m_IsActiveTab = active;

		style()->unpolish(this);
		style()->polish(this);
		update();

		emit activeTabChanged();
	}
}

void SectionTitleWidget::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		ev->accept();
        m_DragStartPosition = ev->pos();
		return;
	}
	QFrame::mousePressEvent(ev);
}

void SectionTitleWidget::mouseReleaseEvent(QMouseEvent* ev)
{
	SectionWidget* section = nullptr;
	MainContainerWidget* cw = findParentContainerWidget(this);

	if (isDraggingFloatingWidget() && cw->rect().contains(cw->mapFromGlobal(ev->globalPos())))
	{
		cw->dropFloatingWidget(m_FloatingWidget, ev->globalPos());
	}
	// End of tab moving, change order now
	else if (m_TabMoving && (section = findParentSectionWidget(this)) != nullptr)
	{
		// Find tab under mouse
		QPoint pos = ev->globalPos();
		pos = section->mapFromGlobal(pos);
        int fromIndex = section->indexOfContent(m_Content);
        int toIndex = section->indexOfContentByTitlePos(pos, this);
        qInfo() << "fromIndex: " << fromIndex << " toIndex: " << toIndex;
        if (-1 == toIndex)
        {
            toIndex = section->indexOfContent(section->contents().last());
        }
		section->moveContent(fromIndex, toIndex);
	}

    if (!m_DragStartPosition.isNull())
    {
		emit clicked();
    }

	// Reset
    m_DragStartPosition = QPoint();
    m_TabMoving = false;
	cw->m_SectionDropOverlay->hideDropOverlay();
	cw->hideContainerOverlay();
	QFrame::mouseReleaseEvent(ev);
}


void SectionTitleWidget::moveFloatingWidget(QMouseEvent* ev, MainContainerWidget* cw)
{
    const QPoint moveToPos = ev->globalPos() - (m_DragStartPosition + QPoint(ADS_WINDOW_FRAME_BORDER_WIDTH, ADS_WINDOW_FRAME_BORDER_WIDTH));
    m_FloatingWidget->move(moveToPos);
   // cw->moveFloatingWidget(moveToPos);

}


void SectionTitleWidget::startFloating(QMouseEvent* ev, MainContainerWidget* cw, SectionWidget* sectionwidget)
{
    QPoint moveToPos = ev->globalPos() - (m_DragStartPosition + QPoint(ADS_WINDOW_FRAME_BORDER_WIDTH, ADS_WINDOW_FRAME_BORDER_WIDTH));
    m_FloatingWidget = cw->startFloating(sectionwidget, m_Content->uid(), moveToPos);
}


void SectionTitleWidget::moveTab(QMouseEvent* ev)
{
    ev->accept();
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QPoint moveToPos = mapToParent(ev->pos()) - m_DragStartPosition;
    moveToPos.setY(0/* + top*/);
    move(moveToPos);
}


bool SectionTitleWidget::isDraggingFloatingWidget() const
{
	return m_FloatingWidget != nullptr;
}

void SectionTitleWidget::mouseMoveEvent(QMouseEvent* ev)
{
    if (!(ev->buttons() & Qt::LeftButton))
    {
        QFrame::mouseMoveEvent(ev);
        return;
    }

    QPoint Pos = QCursor::pos();
    // TODO make a member with the main container widget and assign it on
    // creation
    MainContainerWidget* MainContainerWidget = findParentContainerWidget(this);
    auto Containers = MainContainerWidget->m_Containers;
    CContainerWidget* TopContainer = nullptr;
    for (auto ContainerWidget : Containers)
    {
    	if (!ContainerWidget->isVisible())
    	{
    		continue;
    	}

    	if (!m_FloatingWidget || (m_FloatingWidget->containerWidget() == ContainerWidget))
    	{
    		continue;
    	}

    	QPoint MappedPos = ContainerWidget->mapFromGlobal(Pos);
    	if (ContainerWidget->rect().contains(MappedPos))
    	{
    		std::cout << "Container " <<  ContainerWidget << " contains maousepos" << std::endl;
    		if (!TopContainer || ContainerWidget->isInFrontOf(TopContainer))
    		{
    			TopContainer = ContainerWidget;
    		}
    	}
    }

    if (TopContainer)
    {
    	MainContainerWidget->dropOverlay()->showDropOverlay(TopContainer);
		MainContainerWidget->dropOverlay()->raise();
    }
    else
    {
    	MainContainerWidget->dropOverlay()->hideDropOverlay();
    }

    ev->accept();

    // Move already existing FloatingWidget
    if (isDraggingFloatingWidget())
	{
        moveFloatingWidget(ev, MainContainerWidget);
		return;
	}


    SectionWidget* sectionwidget = findParentSectionWidget(this);
    if (!sectionwidget)
    {
        QFrame::mouseMoveEvent(ev);
        return;
    }

    // move tab
    if (m_TabMoving)
    {
        moveTab(ev);
    }

    // leave if dragging is not active
    if (m_DragStartPosition.isNull())
    {
        QFrame::mouseMoveEvent(ev);
        return;
    }

	// Begin to drag/float the SectionContent.
    if (!sectionwidget->titleAreaGeometry().contains(sectionwidget->mapFromGlobal(ev->globalPos())))
	{
        startFloating(ev, MainContainerWidget, sectionwidget);
		return;
	}
	// Begin to drag title inside the title area to switch its position inside the SectionWidget.
    else if ((ev->pos() - m_DragStartPosition).manhattanLength() >= QApplication::startDragDistance() // Wait a few pixels before start moving
              && sectionwidget->titleAreaGeometry().contains(sectionwidget->mapFromGlobal(ev->globalPos())))
	{
        m_TabMoving = true;
		raise(); // Raise current title-widget above other tabs
		return;
	}
	QFrame::mouseMoveEvent(ev);
}

ADS_NAMESPACE_END
