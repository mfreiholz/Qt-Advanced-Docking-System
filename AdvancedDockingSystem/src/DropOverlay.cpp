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
#include <QtGlobal>
#include <QDebug>

#include <iostream>

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
			break;
		case RightDropArea:
			areaRect = QRectF(baseRect.width() * .5f, baseRect.y(), baseRect.width() * .5f, baseRect.height());
			areaLine = QLineF(areaRect.topLeft(), areaRect.bottomLeft());
			gradient.setStart(areaRect.topLeft());
			gradient.setFinalStop(areaRect.topRight());
			break;
		case BottomDropArea:
			areaRect = QRectF(baseRect.x(), baseRect.height() * .5f, baseRect.width(), baseRect.height() * .5f);
			areaLine = QLineF(areaRect.topLeft(), areaRect.topRight());
			gradient.setStart(areaRect.topLeft());
			gradient.setFinalStop(areaRect.bottomLeft());
			break;
		case LeftDropArea:
			areaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width() * .5f, baseRect.height());
			areaLine = QLineF(areaRect.topRight(), areaRect.bottomRight());
			gradient.setStart(areaRect.topLeft());
			gradient.setFinalStop(areaRect.topRight());
			break;
		default:
			break;
	}
	if (areaRect.isValid())
	{
		gradient.setColorAt(0.f, areaBackgroundColor);
		gradient.setColorAt(1.f, areaBackgroundColor.lighter(120));
		p.fillRect(areaRect, gradient);

		pen = p.pen();
		pen.setColor(borderColor);
		pen.setStyle(Qt::DashLine);
		p.setPen(pen);
		p.drawLine(areaLine);
	}
	p.restore();

	p.save();
	pen = p.pen();
	pen.setColor(borderColor);
	pen.setWidth(1);

	p.setPen(pen);
	p.drawRect(baseRect.adjusted(0, 0, -pen.width(), -pen.width()));
	p.restore();
	return pm;
}

QWidget* DropOverlay::createDropIndicatorWidget(DropArea dropArea)
{
	QLabel* l = new QLabel();
	l->setObjectName("DropAreaLabel");

	const qreal metric = static_cast<qreal>(l->fontMetrics().height()) * 2.f;
	const QSizeF size(metric, metric);

	l->setPixmap(createDropIndicatorPixmap(l->palette(), size, dropArea));
	l->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	l->setAttribute(Qt::WA_TranslucentBackground);
	return l;
}

///////////////////////////////////////////////////////////////////////

DropOverlay::DropOverlay(QWidget* parent, eMode Mode) :
	QFrame(parent),
	_allowedAreas(InvalidDropArea),
	_cross(new DropOverlayCross(this)),
	_lastLocation(InvalidDropArea)
{
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	setWindowOpacity(0.2);
	setWindowTitle("DropOverlay");
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_TranslucentBackground);

	QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
	l->setSpacing(0);
	setLayout(l);

	_cross->setupOverlayCross(Mode);
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


DropArea DropOverlay::dropAreaUnderCursor() const
{
	return _cross->cursorLocation();
}

DropArea DropOverlay::showDropOverlay(QWidget* target)
{
    //std::cout << "DropOverlay::showDropOverlay(QWidget* target)" << std::endl;
	if (_target == target)
	{
        qInfo() << "_target == target";
		// Hint: We could update geometry of overlay here.
		DropArea da = dropAreaUnderCursor();
		if (da != _lastLocation)
		{
            qInfo() << "repaint()";
			repaint();
			_lastLocation = da;
		}
		return da;
	}

	_target = target;
	_targetRect = QRect();
	_lastLocation = InvalidDropArea;

	// Move it over the target.
	resize(target->size());
	move(target->mapToGlobal(target->rect().topLeft()));
	show();
	return dropAreaUnderCursor();
}

void DropOverlay::showDropOverlay(QWidget* target, const QRect& targetAreaRect)
{
     qInfo() << "DropOverlay::showDropOverlay(QWidget* target, const QRect& targetAreaRect)";
	if (_target == target && _targetRect == targetAreaRect)
	{
		return;
	}
	//hideDropOverlay();
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
    qInfo() << "hideDropOverlay() _fullAreaDrop = false";
	hide();
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
	// Draw rect based on location
	QRect r = rect();
	const DropArea da = dropAreaUnderCursor();
	std::cout << "CursorLocation: " << dropAreaUnderCursor() << std::endl;
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
        return;
	}
	QPainter painter(this);
    QColor Color = palette().color(QPalette::Active, QPalette::Highlight);
	painter.fillRect(r, QBrush(Color, Qt::Dense4Pattern));
	painter.setBrush(QBrush(Color));
	painter.drawRect(r);
}

void DropOverlay::showEvent(QShowEvent*)
{
	_cross->show();
	QWidget* w = parentWidget() ? parentWidget() : _target.data();
	QRect WidgetRect = w->rect();
	QPoint Pos(WidgetRect.left(), WidgetRect.center().y());
}

void DropOverlay::hideEvent(QHideEvent*)
{
	_cross->hide();
}

void DropOverlay::resizeEvent(QResizeEvent* e)
{
    qInfo() << "DropOverlay::resizeEvent" << e->size();
	_cross->resize(e->size());
}

void DropOverlay::moveEvent(QMoveEvent* e)
{
    qInfo() << "DropOverlay::moveEvent" << e->pos();
	_cross->move(e->pos());
}

static int areaAlignment(const DropArea area)
{
	switch (area)
	{
		case ADS_NS::TopDropArea: return (int) Qt::AlignHCenter | Qt::AlignBottom;
		case ADS_NS::RightDropArea: return (int) Qt::AlignLeft | Qt::AlignVCenter;
		case ADS_NS::BottomDropArea: return (int) Qt::AlignHCenter | Qt::AlignTop;
		case ADS_NS::LeftDropArea: return (int) Qt::AlignRight | Qt::AlignVCenter;
		case ADS_NS::CenterDropArea:  return (int) Qt::AlignCenter;
		default: return Qt::AlignCenter;
	}
}


QPoint DropOverlayCross::areaGridPosition(const DropArea area)
{
	if (DropOverlay::ModeSectionOverlay == m_Mode)
	{
		switch (area)
		{
			case ADS_NS::TopDropArea: return QPoint(1, 2);
			case ADS_NS::RightDropArea: return QPoint(2, 3);
			case ADS_NS::BottomDropArea: return QPoint(3, 2);
			case ADS_NS::LeftDropArea: return QPoint(2, 1);
			case ADS_NS::CenterDropArea: return QPoint(2, 2);
			default: return QPoint();
		}
	}
	else
	{
		switch (area)
		{
			case ADS_NS::TopDropArea: return QPoint(0, 2);
			case ADS_NS::RightDropArea: return QPoint(2, 4);
			case ADS_NS::BottomDropArea: return QPoint(4, 2);
			case ADS_NS::LeftDropArea: return QPoint(2, 0);
			case ADS_NS::CenterDropArea: return QPoint(2, 2);
			default: return QPoint();
		}
	}
}


DropOverlayCross::DropOverlayCross(DropOverlay* overlay) :
	QWidget(overlay->parentWidget()),
	m_DropOverlay(overlay)
{
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	setWindowTitle("DropOverlayCross");
	setAttribute(Qt::WA_TranslucentBackground);

	m_GridLayout = new QGridLayout();
	m_GridLayout->setSpacing(6);
	setLayout(m_GridLayout);
}

DropOverlayCross::~DropOverlayCross()
{
}


void DropOverlayCross::setupOverlayCross(DropOverlay::eMode Mode)
{
	m_Mode = Mode;

	QHash<DropArea, QWidget*> areaWidgets;
	areaWidgets.insert(TopDropArea, DropOverlay::createDropIndicatorWidget(TopDropArea));
	areaWidgets.insert(RightDropArea, DropOverlay::createDropIndicatorWidget(RightDropArea));
	areaWidgets.insert(BottomDropArea, DropOverlay::createDropIndicatorWidget(BottomDropArea));
	areaWidgets.insert(LeftDropArea, DropOverlay::createDropIndicatorWidget(LeftDropArea));
	areaWidgets.insert(CenterDropArea, DropOverlay::createDropIndicatorWidget(CenterDropArea));

	setAreaWidgets(areaWidgets);
}


void DropOverlayCross::setAreaWidgets(const QHash<DropArea, QWidget*>& widgets)
{
	// Delete old widgets.
	QMutableHashIterator<DropArea, QWidget*> i(m_DropIndicatorWidgets);
	while (i.hasNext())
	{
		i.next();
		QWidget* widget = i.value();
		m_GridLayout->removeWidget(widget);
		delete widget;
		i.remove();
	}

	// Insert new widgets into grid.
	m_DropIndicatorWidgets = widgets;
	QHashIterator<DropArea, QWidget*> i2(m_DropIndicatorWidgets);
	while (i2.hasNext())
	{
		i2.next();
		const DropArea area = i2.key();
		QWidget* widget = i2.value();
		QPoint p = areaGridPosition(area);
		m_GridLayout->addWidget(widget, p.x(), p.y(), (Qt::Alignment) areaAlignment(area));
	}

	if (DropOverlay::ModeSectionOverlay == m_Mode)
	{
		m_GridLayout->setContentsMargins(0, 0, 0, 0);
		m_GridLayout->setRowStretch(0, 1);
		m_GridLayout->setRowStretch(1, 0);
		m_GridLayout->setRowStretch(2, 0);
		m_GridLayout->setRowStretch(3, 0);
		m_GridLayout->setRowStretch(4, 1);

		m_GridLayout->setColumnStretch(0, 1);
		m_GridLayout->setColumnStretch(1, 0);
		m_GridLayout->setColumnStretch(2, 0);
		m_GridLayout->setColumnStretch(3, 0);
		m_GridLayout->setColumnStretch(4, 1);
	}
	else
	{
		m_GridLayout->setContentsMargins(4, 4, 4, 4);
		m_GridLayout->setRowStretch(0, 0);
		m_GridLayout->setRowStretch(1, 1);
		m_GridLayout->setRowStretch(2, 1);
		m_GridLayout->setRowStretch(3, 1);
		m_GridLayout->setRowStretch(4, 0);

		m_GridLayout->setColumnStretch(0, 0);
		m_GridLayout->setColumnStretch(1, 1);
		m_GridLayout->setColumnStretch(2, 1);
		m_GridLayout->setColumnStretch(3, 1);
		m_GridLayout->setColumnStretch(4, 0);
	}
	reset();
}

DropArea DropOverlayCross::cursorLocation() const
{
	const QPoint pos = mapFromGlobal(QCursor::pos());
	QHashIterator<DropArea, QWidget*> i(m_DropIndicatorWidgets);
	while (i.hasNext())
	{
		i.next();
		if (m_DropOverlay->allowedAreas().testFlag(i.key())
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
	resize(m_DropOverlay->size());
	move(m_DropOverlay->pos());
}

void DropOverlayCross::reset()
{
	QList<DropArea> allAreas;
	allAreas << ADS_NS::TopDropArea << ADS_NS::RightDropArea
		<< ADS_NS::BottomDropArea << ADS_NS::LeftDropArea << ADS_NS::CenterDropArea;
	const DropAreas allowedAreas = m_DropOverlay->allowedAreas();

	// Update visibility of area widgets based on allowedAreas.
	for (int i = 0; i < allAreas.count(); ++i)
	{
		QPoint p = areaGridPosition(allAreas.at(i));
		QLayoutItem* item = m_GridLayout->itemAtPosition(p.x(), p.y());
		QWidget* w = nullptr;
		if (item && (w = item->widget()) != nullptr)
		{
			w->setVisible(allowedAreas.testFlag(allAreas.at(i)));
		}
	}
}

ADS_NAMESPACE_END
