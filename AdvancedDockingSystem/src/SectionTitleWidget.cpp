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

#ifdef ADS_ANIMATIONS_ENABLED
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#endif

#include "ads/Internal.h"
#include "ads/DropOverlay.h"
#include "ads/SectionContent.h"
#include "ads/SectionWidget.h"
#include "ads/FloatingWidget.h"
#include "ads/ContainerWidget.h"

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
	SectionWidget* section = NULL;
	ContainerWidget* cw = findParentContainerWidget(this);

	// Drop contents of FloatingWidget into SectionWidget.
    if (m_FloatingWidget)
	{
		SectionWidget* sw = cw->sectionAt(cw->mapFromGlobal(ev->globalPos()));
		if (sw)
		{
			cw->_dropOverlay->setAllowedAreas(ADS_NS::AllAreas);
			DropArea loc = cw->_dropOverlay->showDropOverlay(sw);
			if (loc != InvalidDropArea)
			{
				InternalContentData data;
                m_FloatingWidget->takeContent(data);
                m_FloatingWidget->deleteLater();
                m_FloatingWidget.clear();
				cw->dropContent(data, sw, loc, true);
			}
		}
		// Mouse is over a outer-edge drop area
		else
		{
			DropArea dropArea = ADS_NS::InvalidDropArea;
			if (cw->outerTopDropRect().contains(cw->mapFromGlobal(ev->globalPos())))
				dropArea = ADS_NS::TopDropArea;
			if (cw->outerRightDropRect().contains(cw->mapFromGlobal(ev->globalPos())))
				dropArea = ADS_NS::RightDropArea;
			if (cw->outerBottomDropRect().contains(cw->mapFromGlobal(ev->globalPos())))
				dropArea = ADS_NS::BottomDropArea;
			if (cw->outerLeftDropRect().contains(cw->mapFromGlobal(ev->globalPos())))
				dropArea = ADS_NS::LeftDropArea;

			if (dropArea != ADS_NS::InvalidDropArea)
			{
				InternalContentData data;
                m_FloatingWidget->takeContent(data);
                m_FloatingWidget->deleteLater();
                m_FloatingWidget.clear();
				cw->dropContent(data, NULL, dropArea, true);
			}
		}
	}
	// End of tab moving, change order now
    else if (m_TabMoving
			&& (section = findParentSectionWidget(this)) != NULL)
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
		emit clicked();

	// Reset
    m_DragStartPosition = QPoint();
    m_TabMoving = false;
	cw->_dropOverlay->hideDropOverlay();
	QFrame::mouseReleaseEvent(ev);
}


void SectionTitleWidget::moveFloatingWidget(QMouseEvent* ev, ContainerWidget* cw)
{
    const QPoint moveToPos = ev->globalPos() - (m_DragStartPosition + QPoint(ADS_WINDOW_FRAME_BORDER_WIDTH, ADS_WINDOW_FRAME_BORDER_WIDTH));
    m_FloatingWidget->move(moveToPos);
    cw->moveFloatingWidget(moveToPos);
}


void SectionTitleWidget::startFloating(QMouseEvent* ev, ContainerWidget* cw, SectionWidget* sectionwidget)
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

void SectionTitleWidget::mouseMoveEvent(QMouseEvent* ev)
{
    if (!(ev->buttons() & Qt::LeftButton))
    {
        QFrame::mouseMoveEvent(ev);
        return;
    }

    ev->accept();
    ContainerWidget* cw = findParentContainerWidget(this);
	// Move already existing FloatingWidget
    if (m_FloatingWidget)
	{
        moveFloatingWidget(ev, cw);
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
        startFloating(ev, cw, sectionwidget);
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
