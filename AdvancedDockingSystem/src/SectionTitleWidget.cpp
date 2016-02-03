#include "ads/SectionTitleWidget.h"

#include <QDebug>
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

#include "ads/DropOverlay.h"
#include "ads/SectionContent.h"
#include "ads/SectionWidget.h"
#include "ads/FloatingWidget.h"
#include "ads/ContainerWidget.h"

ADS_NAMESPACE_BEGIN

SectionTitleWidget::SectionTitleWidget(SectionContent::RefPtr content, QWidget* parent) :
	QFrame(parent),
	_content(content),
	_tabMoving(false),
	_activeTab(false)
{
	auto l = new QBoxLayout(QBoxLayout::LeftToRight);
	l->addWidget(content->titleWidget());
	setLayout(l);
}

bool SectionTitleWidget::isActiveTab() const
{
	return _activeTab;
}

void SectionTitleWidget::setActiveTab(bool active)
{
	if (active != _activeTab)
	{
		_activeTab = active;

		style()->unpolish(this);
		style()->polish(this);
		update();

		emit activeTabChanged();
	}
}

void SectionTitleWidget::mousePressEvent(QMouseEvent* ev)
{
//	qDebug() << Q_FUNC_INFO << ev->pos();
	if (ev->button() == Qt::LeftButton)
	{
		_dragStartPos = ev->pos();
		ev->accept();
		return;
	}
	QFrame::mousePressEvent(ev);
}

void SectionTitleWidget::mouseReleaseEvent(QMouseEvent* ev)
{
//	qDebug() << Q_FUNC_INFO << ev->pos();
	SectionWidget* section = NULL;

	// Drop contents of FloatingWidget into SectionWidget.
	if (_fw)
	{
		auto cw = findParentContainerWidget(this);
		auto sw = cw->sectionAt(cw->mapFromGlobal(ev->globalPos()));
		if (sw)
		{
			auto loc = showDropOverlay(sw);
			if (loc != InvalidDropArea)
			{
#if !defined(ADS_ANIMATIONS_ENABLED)
				auto data = _fw->takeContent();
				_fw->deleteLater();
				_fw.clear();
				cw->dropContent(data, sw, loc);
#else
				auto moveAnim = new QPropertyAnimation(_fw, "pos", this);
				moveAnim->setStartValue(_fw->pos());
				moveAnim->setEndValue(sw->mapToGlobal(sw->rect().topLeft()));
				moveAnim->setDuration(ADS_ANIMATION_DURATION);

				auto resizeAnim = new QPropertyAnimation(_fw, "size", this);
				resizeAnim->setStartValue(_fw->size());
				resizeAnim->setEndValue(sw->size());
				resizeAnim->setDuration(ADS_ANIMATION_DURATION);

				auto animGroup = new QParallelAnimationGroup(this);
				QObject::connect(animGroup, &QPropertyAnimation::finished, [this, data, sw, loc]()
				{
					auto data = _fw->takeContent();
					_fw->deleteLater();
					_fw.clear();
					cw->dropContent(data, sw, loc);
				});
				animGroup->addAnimation(moveAnim);
				animGroup->addAnimation(resizeAnim);
				animGroup->start(QAbstractAnimation::DeleteWhenStopped);
#endif
			}
		}
		// Mouse is over a outer-edge drop area
		else
		{
			DropArea dropArea = DropArea::InvalidDropArea;
			if (cw->outerTopDropRect().contains(cw->mapFromGlobal(ev->globalPos())))
				dropArea = DropArea::TopDropArea;
			if (cw->outerRightDropRect().contains(cw->mapFromGlobal(ev->globalPos())))
				dropArea = DropArea::RightDropArea;
			if (cw->outerBottomDropRect().contains(cw->mapFromGlobal(ev->globalPos())))
				dropArea = DropArea::BottomDropArea;
			if (cw->outerLeftDropRect().contains(cw->mapFromGlobal(ev->globalPos())))
				dropArea = DropArea::LeftDropArea;

			if (dropArea != DropArea::InvalidDropArea)
			{
#if !defined(ADS_ANIMATIONS_ENABLED)
				auto data = _fw->takeContent();
				_fw->deleteLater();
				_fw.clear();
				cw->dropContent(data, NULL, dropArea);
#else
#endif
			}
		}
	}
	// End of tab moving, change order now
	else if (_tabMoving
			&& (section = findParentSectionWidget(this)) != NULL)
	{
		qDebug() << "Stop tab move";

		// Find tab under mouse
		QPoint pos = ev->globalPos();
		pos = section->mapFromGlobal(pos);
		const int fromIndex = section->indexOfContent(_content);
		const int toIndex = section->indexOfContentByTitlePos(pos, this);
		qDebug() << "from" << fromIndex << "to" << toIndex;
		section->moveContent(fromIndex, toIndex);
		section->layout()->update();
	}

	if (!_dragStartPos.isNull())
		emit clicked();

	// Reset
	_dragStartPos = QPoint();
	_tabMoving = false;
	hideDropOverlay();
	QFrame::mouseReleaseEvent(ev);
}

void SectionTitleWidget::mouseMoveEvent(QMouseEvent* ev)
{
	ContainerWidget* cw = findParentContainerWidget(this);
	SectionWidget* section = NULL;

	// Move already existing FloatingWidget
	if (_fw)
	{
		const QPoint moveToPos = ev->globalPos() - (_dragStartPos + QPoint(ADS_WINDOW_FRAME_BORDER_WIDTH, ADS_WINDOW_FRAME_BORDER_WIDTH));
		_fw->move(moveToPos);

		// Show drop indicator
		if (true)
		{
			// Mouse is over a SectionWidget
			section = cw->sectionAt(cw->mapFromGlobal(QCursor::pos()));
			if (section)
			{
				showDropOverlay(section);
			}
			// Mouse is at the edge of the ContainerWidget
			// Top, Right, Bottom, Left
			else if (cw->outerTopDropRect().contains(cw->mapFromGlobal(QCursor::pos())))
			{
				showDropOverlay(cw, cw->outerTopDropRect());
			}
			else if (cw->outerRightDropRect().contains(cw->mapFromGlobal(QCursor::pos())))
			{
				showDropOverlay(cw, cw->outerRightDropRect());
			}
			else if (cw->outerBottomDropRect().contains(cw->mapFromGlobal(QCursor::pos())))
			{
				showDropOverlay(cw, cw->outerBottomDropRect());
			}
			else if (cw->outerLeftDropRect().contains(cw->mapFromGlobal(QCursor::pos())))
			{
				showDropOverlay(cw, cw->outerLeftDropRect());
			}
			else
			{
				hideDropOverlay();
			}
		}

		ev->accept();
		return;
	}
	// Begin to drag/float the SectionContent.
	else if (!_fw && !_dragStartPos.isNull() && (ev->buttons() & Qt::LeftButton)
			&& (section = findParentSectionWidget(this)) != NULL
			&& !section->titleAreaGeometry().contains(section->mapFromGlobal(ev->globalPos())))
	{
		// Create floating widget.
		auto data = section->take(_content->uid(), false);

		_fw = new FloatingWidget(cw, data.content, data.titleWidget, data.contentWidget, cw);
		_fw->resize(section->size());
		setActiveTab(false);

		const QPoint moveToPos = ev->globalPos() - (_dragStartPos + QPoint(ADS_WINDOW_FRAME_BORDER_WIDTH, ADS_WINDOW_FRAME_BORDER_WIDTH));
		_fw->move(moveToPos);
		_fw->show();

		// Delete old section, if it is empty now.
		if (section->contents().isEmpty())
		{
			delete section;
			section = NULL;
		}

		// Delete old splitter, if it is empty now
		deleteEmptySplitter(cw);

		ev->accept();
		return;
	}
	// Handle movement of this tab
	else if (_tabMoving
			&& (section = findParentSectionWidget(this)) != NULL)
	{
		int left, top, right, bottom;
		getContentsMargins(&left, &top, &right, &bottom);
		QPoint moveToPos = mapToParent(ev->pos()) - _dragStartPos;
		moveToPos.setY(0 + top);
		move(moveToPos);
		ev->accept();
	}
	// Begin to drag title inside the title area to switch its position inside the SectionWidget.
	else if (!_dragStartPos.isNull() && (ev->buttons() & Qt::LeftButton)
			&& (ev->pos() - _dragStartPos).manhattanLength() >= QApplication::startDragDistance() // Wait a few pixels before start moving
			&& (section = findParentSectionWidget(this)) != NULL
			&& section->titleAreaGeometry().contains(section->mapFromGlobal(ev->globalPos())))
	{
		// Raise current title-widget above other tabs
		_tabMoving = true;
		raise();
		ev->accept();
	}
	QFrame::mouseMoveEvent(ev);
}

ADS_NAMESPACE_END