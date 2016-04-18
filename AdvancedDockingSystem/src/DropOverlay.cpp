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

static QPixmap createDropIndicatorPixmap(const QPalette& pal, const QSizeF& size, DropArea dropArea)
{
	const QColor borderColor = pal.color(QPalette::Active, QPalette::Highlight);
	const QColor backgroundColor = pal.color(QPalette::Active, QPalette::Base);
	const QColor areaBackgroundColor = pal.color(QPalette::Active, QPalette::Highlight).lighter(150);

	QPixmap pm(size.width(), size.height());
	pm.fill(QColor(0, 0, 0, 0));

	QPainter p(&pm);
	QPen pen = p.pen();
	QRectF baseRect(pm.rect());

	// Fill
	p.fillRect(baseRect, backgroundColor);

	// Drop area rect.
	if (true)
	{
		p.save();
		QRectF areaRect;
		QLineF areaLine;
		QLinearGradient gradient;
		switch (dropArea)
		{
			case TopDropArea:
				areaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width(), baseRect.height() * .5f);
				areaLine = QLineF(areaRect.bottomLeft(), areaRect.bottomRight());
				gradient.setStart(areaRect.topLeft());
				gradient.setFinalStop(areaRect.bottomLeft());
				gradient.setColorAt(0.f, areaBackgroundColor);
				gradient.setColorAt(1.f, areaBackgroundColor.lighter(120));
				break;
			case RightDropArea:
				areaRect = QRectF(baseRect.width() * .5f, baseRect.y(), baseRect.width() * .5f, baseRect.height());
				areaLine = QLineF(areaRect.topLeft(), areaRect.bottomLeft());
				gradient.setStart(areaRect.topLeft());
				gradient.setFinalStop(areaRect.topRight());
				gradient.setColorAt(0.f, areaBackgroundColor.lighter(120));
				gradient.setColorAt(1.f, areaBackgroundColor);
				break;
			case BottomDropArea:
				areaRect = QRectF(baseRect.x(), baseRect.height() * .5f, baseRect.width(), baseRect.height() * .5f);
				areaLine = QLineF(areaRect.topLeft(), areaRect.topRight());
				gradient.setStart(areaRect.topLeft());
				gradient.setFinalStop(areaRect.bottomLeft());
				gradient.setColorAt(0.f, areaBackgroundColor.lighter(120));
				gradient.setColorAt(1.f, areaBackgroundColor);
				break;
			case LeftDropArea:
				areaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width() * .5f, baseRect.height());
				areaLine = QLineF(areaRect.topRight(), areaRect.bottomRight());
				gradient.setStart(areaRect.topLeft());
				gradient.setFinalStop(areaRect.topRight());
				gradient.setColorAt(0.f, areaBackgroundColor);
				gradient.setColorAt(1.f, areaBackgroundColor.lighter(120));
				break;
			default:
				break;
		}
		if (areaRect.isValid())
		{
			p.fillRect(areaRect, gradient);

			pen = p.pen();
			pen.setColor(borderColor);
			pen.setStyle(Qt::DashLine);
			p.setPen(pen);
			p.drawLine(areaLine);
		}
		p.restore();
	}

	// Border
	if (true)
	{
		p.save();
		pen = p.pen();
		pen.setColor(borderColor);
		pen.setWidth(1);

		p.setPen(pen);
		p.drawRect(baseRect.adjusted(0, 0, -pen.width(), -pen.width()));
		p.restore();
	}
	return pm;
}

static QWidget* createDropIndicatorWidget(DropArea dropArea)
{
	QLabel* l = new QLabel();
	l->setObjectName("DropAreaLabel");

	const qreal metric = static_cast<qreal>(l->fontMetrics().height()) * 2.f;
	const QSizeF size(metric, metric);

	l->setPixmap(createDropIndicatorPixmap(l->palette(), size, dropArea));
	return l;
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

	// Cross with default drop area widgets.
	QHash<DropArea, QWidget*> areaWidgets;
	areaWidgets.insert(ADS_NS::TopDropArea, createDropIndicatorWidget(TopDropArea)); //createDropWidget(":/img/split-top.png"));
	areaWidgets.insert(ADS_NS::RightDropArea, createDropIndicatorWidget(RightDropArea));//createDropWidget(":/img/split-right.png"));
	areaWidgets.insert(ADS_NS::BottomDropArea, createDropIndicatorWidget(BottomDropArea));//createDropWidget(":/img/split-bottom.png"));
	areaWidgets.insert(ADS_NS::LeftDropArea, createDropIndicatorWidget(LeftDropArea));//createDropWidget(":/img/split-left.png"));
	areaWidgets.insert(ADS_NS::CenterDropArea, createDropIndicatorWidget(CenterDropArea));//createDropWidget(":/img/dock-center.png"));
	_cross->setAreaWidgets(areaWidgets);

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
}

DropAreas DropOverlay::allowedAreas() const
{
	return _allowedAreas;
}

void DropOverlay::setAreaWidgets(const QHash<DropArea, QWidget*>& widgets)
{
	_cross->setAreaWidgets(widgets);
}

DropArea DropOverlay::cursorLocation() const
{
	return _cross->cursorLocation();
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

static QPair<QPoint, int> gridPosForArea(const DropArea area)
{
	switch (area)
	{
		case ADS_NS::TopDropArea:
			return qMakePair(QPoint(0, 1), (int) Qt::AlignHCenter | Qt::AlignBottom);
		case ADS_NS::RightDropArea:
			return qMakePair(QPoint(1, 2), (int) Qt::AlignLeft | Qt::AlignVCenter);
		case ADS_NS::BottomDropArea:
			return qMakePair(QPoint(2, 1), (int) Qt::AlignHCenter | Qt::AlignTop);
		case ADS_NS::LeftDropArea:
			return qMakePair(QPoint(1, 0), (int) Qt::AlignRight | Qt::AlignVCenter);
		case ADS_NS::CenterDropArea:
			return qMakePair(QPoint(1, 1), (int) Qt::AlignCenter);
		default:
			return QPair<QPoint, int>();
	}
}

DropOverlayCross::DropOverlayCross(DropOverlay* overlay) :
	QWidget(overlay->parentWidget()),
	_overlay(overlay),
	_widgets()
{
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	setWindowTitle("DropOverlayCross");
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

DropOverlayCross::~DropOverlayCross()
{
}

void DropOverlayCross::setAreaWidgets(const QHash<DropArea, QWidget*>& widgets)
{
	// Delete old widgets.
	QMutableHashIterator<DropArea, QWidget*> i(_widgets);
	while (i.hasNext())
	{
		i.next();
		QWidget* widget = i.value();
		_grid->removeWidget(widget);
		delete widget;
		i.remove();
	}

	// Insert new widgets into grid.
	_widgets = widgets;
	QHashIterator<DropArea, QWidget*> i2(_widgets);
	while (i2.hasNext())
	{
		i2.next();
		const DropArea area = i2.key();
		QWidget* widget = i2.value();
		const QPair<QPoint, int> opts = gridPosForArea(area);
		_grid->addWidget(widget, opts.first.x(), opts.first.y(), (Qt::Alignment) opts.second);
	}
	reset();
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

void DropOverlayCross::showEvent(QShowEvent*)
{
	resize(_overlay->size());
	move(_overlay->pos());
}

void DropOverlayCross::reset()
{
	QList<DropArea> allAreas;
	allAreas << ADS_NS::TopDropArea << ADS_NS::RightDropArea << ADS_NS::BottomDropArea << ADS_NS::LeftDropArea << ADS_NS::CenterDropArea;
	const DropAreas allowedAreas = _overlay->allowedAreas();

	// Update visibility of area widgets based on allowedAreas.
	for (int i = 0; i < allAreas.count(); ++i)
	{
		const QPair<QPoint, int> opts = gridPosForArea(allAreas.at(i));

		QLayoutItem* item = _grid->itemAtPosition(opts.first.x(), opts.first.y());
		QWidget* w = NULL;
		if (item && (w = item->widget()) != NULL)
		{
			w->setVisible(allowedAreas.testFlag(allAreas.at(i)));
		}
	}
}

ADS_NAMESPACE_END
