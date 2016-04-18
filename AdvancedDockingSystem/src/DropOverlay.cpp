#include "ads/DropOverlay.h"

#include <QPointer>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QPainter>
#include <QGridLayout>
#include <QCursor>
#include <QIcon>
#include <QLabel>

ADS_NAMESPACE_BEGIN

// Helper /////////////////////////////////////////////////////////////

static QWidget* createDropWidget(const QString& img)
{
	QLabel* label = new QLabel();
	label->setObjectName("DropAreaLabel");
	label->setPixmap(QPixmap(img));
	return label;
}

///////////////////////////////////////////////////////////////////////

DropOverlay::DropOverlay(QWidget* parent) :
	QFrame(parent),
	_allowedAreas(InvalidDropArea),
	_cross(new DropOverlayCross(this)),
	_fullAreaDrop(false),
	_lastLocation(InvalidDropArea)
{
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	setWindowOpacity(0.2);
	setWindowTitle("DropOverlay");

	QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	setLayout(l);

	_cross->setVisible(false);
	setVisible(false);
}

DropOverlay::~DropOverlay()
{
}

void DropOverlay::setAllowedAreas(DropAreas areas)
{
	if (areas == _allowedAreas)
		return;
	_allowedAreas = areas;

	_cross->reset();
	_cross->move(pos());
	_cross->resize(size());
}

DropAreas DropOverlay::allowedAreas() const
{
	return _allowedAreas;
}

DropArea DropOverlay::cursorLocation() const
{
	DropArea loc = InvalidDropArea;
	if (_cross)
	{
		loc = _cross->cursorLocation();
	}
	return loc;
}

DropArea DropOverlay::showDropOverlay(QWidget* target)
{
	if (_target == target)
	{
		// Hint: We could update geometry of overlay here.
		DropArea da = cursorLocation();
		if (da != _lastLocation)
		{
			repaint();
			_lastLocation = da;
		}
		return da;
	}

	hideDropOverlay();
	_fullAreaDrop = false;
	_target = target;
	_targetRect = QRect();
	_lastLocation = InvalidDropArea;

	// Move it over the target.
	resize(target->size());
	move(target->mapToGlobal(target->rect().topLeft()));

	show();

	return cursorLocation();
}

void DropOverlay::showDropOverlay(QWidget* target, const QRect& targetAreaRect)
{
	if (_target == target && _targetRect == targetAreaRect)
	{
		return;
	}

	hideDropOverlay();
	_fullAreaDrop = true;
	_target = target;
	_targetRect = targetAreaRect;
	_lastLocation = InvalidDropArea;

	// Move it over the target's area.
	resize(targetAreaRect.size());
	move(target->mapToGlobal(QPoint(targetAreaRect.x(), targetAreaRect.y())));

	show();

	return;
}

void DropOverlay::hideDropOverlay()
{
	hide();
	_fullAreaDrop = false;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	_target.clear();
#else
	_target = 0;
#endif
	_targetRect = QRect();
	_lastLocation = InvalidDropArea;
}

void DropOverlay::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	const QColor areaColor = palette().color(QPalette::Active, QPalette::Highlight);//QColor(0, 100, 255)

	// Always draw drop-rect over the entire rect()
	if (_fullAreaDrop)
	{
		QRect r = rect();
		p.fillRect(r, QBrush(areaColor, Qt::Dense4Pattern));
		p.setBrush(QBrush(areaColor));
		p.drawRect(r);
		return;
	}

	// Draw rect based on location
	QRect r = rect();
	const DropArea da = cursorLocation();
	switch (da)
	{
	case ADS_NS::TopDropArea:
		r.setHeight(r.height() / 2);
		break;
	case ADS_NS::RightDropArea:
		r.setX(r.width() / 2);
		break;
	case ADS_NS::BottomDropArea:
		r.setY(r.height() / 2);
		break;
	case ADS_NS::LeftDropArea:
		r.setWidth(r.width() / 2);
		break;
	case ADS_NS::CenterDropArea:
		r = rect();
		break;
	default:
		r = QRect();
	}
	if (!r.isNull())
	{
		p.fillRect(r, QBrush(areaColor, Qt::Dense4Pattern));
		p.setBrush(QBrush(areaColor));
		p.drawRect(r);
	}

	// Draw rect over the entire size + border.
//	auto r = rect();
//	r.setWidth(r.width() - 1);
//	r.setHeight(r.height() - 1);

//	p.fillRect(r, QBrush(QColor(0, 100, 255), Qt::Dense4Pattern));
//	p.setBrush(QBrush(QColor(0, 100, 255)));
	//	p.drawRect(r);
}

void DropOverlay::showEvent(QShowEvent*)
{
	_cross->show();
}

void DropOverlay::hideEvent(QHideEvent*)
{
	_cross->hide();
}

void DropOverlay::resizeEvent(QResizeEvent* e)
{
	_cross->resize(e->size());
}

void DropOverlay::moveEvent(QMoveEvent* e)
{
	_cross->move(e->pos());
}

///////////////////////////////////////////////////////////////////////

DropOverlayCross::DropOverlayCross(DropOverlay* overlay) :
	QWidget(overlay->parentWidget()),
	_overlay(overlay),
	_widgets()
{
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	setWindowTitle("DropSplitAreas");
	setAttribute(Qt::WA_TranslucentBackground);

	_grid = new QGridLayout();
	_grid->setContentsMargins(0, 0, 0, 0);
	_grid->setSpacing(6);

	QBoxLayout* bl1 = new QBoxLayout(QBoxLayout::TopToBottom);
	bl1->setContentsMargins(0, 0, 0, 0);
	bl1->setSpacing(0);
	setLayout(bl1);

	QBoxLayout* bl2 = new QBoxLayout(QBoxLayout::LeftToRight);
	bl2->setContentsMargins(0, 0, 0, 0);
	bl2->setSpacing(0);

	bl1->addStretch(1);
	bl1->addLayout(bl2);
	bl2->addStretch(1);
	bl2->addLayout(_grid, 0);
	bl2->addStretch(1);
	bl1->addStretch(1);
}

void DropOverlayCross::reset()
{
	const DropAreas areas = _overlay->allowedAreas();

	if (areas.testFlag(ADS_NS::TopDropArea))
		setWidgetForArea(createDropWidget(":/img/split-top.png"), ADS_NS::TopDropArea);
	else
		setWidgetForArea(NULL, ADS_NS::TopDropArea);

	if (areas.testFlag(ADS_NS::RightDropArea))
		setWidgetForArea(createDropWidget(":/img/split-right.png"), ADS_NS::RightDropArea);
	else
		setWidgetForArea(NULL, ADS_NS::RightDropArea);

	if (areas.testFlag(ADS_NS::BottomDropArea))
		setWidgetForArea(createDropWidget(":/img/split-bottom.png"), ADS_NS::BottomDropArea);
	else
		setWidgetForArea(NULL, ADS_NS::BottomDropArea);

	if (areas.testFlag(ADS_NS::LeftDropArea))
		setWidgetForArea(createDropWidget(":/img/split-left.png"), ADS_NS::LeftDropArea);
	else
		setWidgetForArea(NULL, ADS_NS::LeftDropArea);

	if (areas.testFlag(ADS_NS::CenterDropArea))
		setWidgetForArea(createDropWidget(":/img/dock-center.png"), ADS_NS::CenterDropArea);
	else
		setWidgetForArea(NULL, ADS_NS::CenterDropArea);
}

DropArea DropOverlayCross::cursorLocation() const
{
	const QPoint pos = mapFromGlobal(QCursor::pos());
	QHashIterator<DropArea, QWidget*> i(_widgets);
	while (i.hasNext())
	{
		i.next();
		if (_overlay->allowedAreas().testFlag(i.key())
			&& i.value()
			&& i.value()->isVisible()
			&& i.value()->geometry().contains(pos))
		{
			return i.key();
		}
	}
	return InvalidDropArea;
}

void DropOverlayCross::setWidgetForArea(QWidget* widget, DropArea area)
{
	QWidget* oldWidget = _widgets.take(area);
	if (oldWidget)
	{
		_grid->removeWidget(oldWidget);
		delete oldWidget;
		oldWidget = NULL;
	}
	if (!widget)
		return;

	switch (area)
	{
		case ADS_NS::TopDropArea:
			_grid->addWidget(widget, 0, 1, Qt::AlignHCenter | Qt::AlignBottom);
			break;
		case ADS_NS::RightDropArea:
			_grid->addWidget(widget, 1, 2, Qt::AlignLeft | Qt::AlignVCenter);
			break;
		case ADS_NS::BottomDropArea:
			_grid->addWidget(widget, 2, 1, Qt::AlignHCenter | Qt::AlignTop);
			break;
		case ADS_NS::LeftDropArea:
			_grid->addWidget(widget, 1, 0, Qt::AlignRight | Qt::AlignVCenter);
			break;
		case ADS_NS::CenterDropArea:
			_grid->addWidget(widget, 1, 1, Qt::AlignCenter);
			break;
		default:
			return;
	}
	_widgets.insert(area, widget);

//	if (area == ADS_NS::TopDropArea)
//	{
//		_top = createDropWidget(":/img/split-top.png");
//		_grid->addWidget(_top, 0, 1, Qt::AlignHCenter | Qt::AlignBottom);
//	}
//	else if (area ==(ADS_NS::RightDropArea))
//	{
//		_right = createDropWidget(":/img/split-right.png");
//		_grid->addWidget(_right, 1, 2, Qt::AlignLeft | Qt::AlignVCenter);
//	}
//	if (areas.testFlag(ADS_NS::BottomDropArea))
//	{
//		_bottom = createDropWidget(":/img/split-bottom.png");
//		_grid->addWidget(_bottom, 2, 1, Qt::AlignHCenter | Qt::AlignTop);
//	}
//	if (areas.testFlag(ADS_NS::LeftDropArea))
//	{
//		_left = createDropWidget(":/img/split-left.png");
//		_grid->addWidget(_left, 1, 0, Qt::AlignRight | Qt::AlignVCenter);
//	}
//	if (areas.testFlag(ADS_NS::CenterDropArea))
//	{
//		_center = createDropWidget(":/img/dock-center.png");
//		_grid->addWidget(_center, 1, 1, Qt::AlignCenter);
	//	}
}

void DropOverlayCross::showEvent(QShowEvent*)
{
	resize(_overlay->size());
	move(_overlay->pos());
}

ADS_NAMESPACE_END
