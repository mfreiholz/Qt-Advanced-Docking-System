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


//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockOverlay.h"

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

#include "DockAreaWidget.h"

namespace ads
{
//============================================================================
static QPixmap createDropIndicatorPixmap(const QPalette& pal, const QSizeF& size, DockWidgetArea DockWidgetArea)
{
	QColor borderColor = pal.color(QPalette::Active, QPalette::Highlight);
	QColor backgroundColor = pal.color(QPalette::Active, QPalette::Base);
	QColor areaBackgroundColor = pal.color(QPalette::Active, QPalette::Highlight).lighter(150);

	QPixmap pm(size.width(), size.height());
	pm.fill(QColor(0, 0, 0, 0));

	QPainter p(&pm);
	QPen pen = p.pen();
	QRectF ShadowRect(pm.rect());
	QRectF baseRect;
	baseRect.setSize(ShadowRect.size() * 0.7);
	baseRect.moveCenter(ShadowRect.center());

	// Fill
	p.fillRect(ShadowRect, QColor(0, 0, 0, 64));
	p.fillRect(baseRect, backgroundColor);

	// Drop area rect.
	p.save();
	QRectF areaRect;
	QLineF areaLine;
	QLinearGradient gradient;
	switch (DockWidgetArea)
	{
		case TopDockWidgetArea:
			areaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width(), baseRect.height() * .5f);
			areaLine = QLineF(areaRect.bottomLeft(), areaRect.bottomRight());
			gradient.setStart(areaRect.topLeft());
			gradient.setFinalStop(areaRect.bottomLeft());
			break;
		case RightDockWidgetArea:
			areaRect = QRectF(ShadowRect.width() * .5f, baseRect.y(), baseRect.width() * .5f, baseRect.height());
			areaLine = QLineF(areaRect.topLeft(), areaRect.bottomLeft());
			gradient.setStart(areaRect.topLeft());
			gradient.setFinalStop(areaRect.topRight());
			break;
		case BottomDockWidgetArea:
			areaRect = QRectF(baseRect.x(), ShadowRect.height() * .5f, baseRect.width(), baseRect.height() * .5f);
			areaLine = QLineF(areaRect.topLeft(), areaRect.topRight());
			gradient.setStart(areaRect.topLeft());
			gradient.setFinalStop(areaRect.bottomLeft());
			break;
		case LeftDockWidgetArea:
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


//============================================================================
QWidget* createDropIndicatorWidget(DockWidgetArea DockWidgetArea)
{
	QLabel* l = new QLabel();
	l->setObjectName("DockWidgetAreaLabel");

	const qreal metric = static_cast<qreal>(l->fontMetrics().height()) * 3.f;
	const QSizeF size(metric, metric);

	l->setPixmap(createDropIndicatorPixmap(l->palette(), size, DockWidgetArea));
	l->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	l->setAttribute(Qt::WA_TranslucentBackground);
	return l;
}


/**
 * Private data class of CDockOverlay
 */
struct DockOverlayPrivate
{
	CDockOverlay* _this;
	DockWidgetAreas AllowedAreas = InvalidDockWidgetArea;
	CDockOverlayCross* Cross;
	QPointer<QWidget> TargetWidget;
	QRect TargetRect;
	DockWidgetArea LastLocation = InvalidDockWidgetArea;
	bool DropPreviewEnabled = true;

	/**
	 * Private data constructor
	 */
	DockOverlayPrivate(CDockOverlay* _public) : _this(_public) {}
};

/**
 * Private data of CDockOverlayCross class
 */
struct DockOverlayCrossPrivate
{
	CDockOverlayCross* _this;
	CDockOverlay::eMode Mode = CDockOverlay::ModeDockAreaOverlay;
	CDockOverlay* DockOverlay;
	QHash<DockWidgetArea, QWidget*> DropIndicatorWidgets;
	QGridLayout* GridLayout;

	/**
	 * Private data constructor
	 */
	DockOverlayCrossPrivate(CDockOverlayCross* _public) : _this(_public) {}

	/**
	 *
	 * @param area
	 * @return
	 */
	QPoint areaGridPosition(const DockWidgetArea area);
};


//============================================================================
CDockOverlay::CDockOverlay(QWidget* parent, eMode Mode) :
	QFrame(parent),
	d(new DockOverlayPrivate(this))
{
	d->Cross = new CDockOverlayCross(this);
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	setWindowOpacity(1);
	setWindowTitle("DockOverlay");
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_TranslucentBackground);

	QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
	l->setSpacing(0);
	setLayout(l);
	l->setContentsMargins(QMargins(0, 0, 0, 0));
	l->addWidget(d->Cross);

	d->Cross->setupOverlayCross(Mode);
	d->Cross->setVisible(false);
	setVisible(false);
}


//============================================================================
CDockOverlay::~CDockOverlay()
{
	delete d;
}


//============================================================================
void CDockOverlay::setAllowedAreas(DockWidgetAreas areas)
{
	if (areas == d->AllowedAreas)
		return;
	d->AllowedAreas = areas;
	d->Cross->reset();
}


//============================================================================
DockWidgetAreas CDockOverlay::allowedAreas() const
{
	return d->AllowedAreas;
}


//============================================================================
DockWidgetArea CDockOverlay::dropAreaUnderCursor() const
{
	DockWidgetArea Result = d->Cross->cursorLocation();
	if (Result != InvalidDockWidgetArea)
	{
		return Result;
	}

	CDockAreaWidget* DockArea = dynamic_cast<CDockAreaWidget*>(d->TargetWidget.data());
	if (!DockArea)
	{
		return Result;
	}

	if (DockArea->titleAreaGeometry().contains(DockArea->mapFromGlobal(QCursor::pos())))
	{
		return CenterDockWidgetArea;
	}

	return Result;
}


//============================================================================
DockWidgetArea CDockOverlay::showOverlay(QWidget* target)
{
	if (d->TargetWidget == target)
	{
        qInfo() << "_target == target";
		// Hint: We could update geometry of overlay here.
		DockWidgetArea da = dropAreaUnderCursor();
		if (da != d->LastLocation)
		{
            qInfo() << "repaint()";
			repaint();
			d->LastLocation = da;
		}
		return da;
	}

	d->TargetWidget = target;
	d->TargetRect = QRect();
	d->LastLocation = InvalidDockWidgetArea;

	// Move it over the target.
	resize(target->size());
	QPoint TopLeft = target->mapToGlobal(target->rect().topLeft());
	move(TopLeft);
	std::cout << "Overlay top: " << TopLeft.x() << " left: " << TopLeft.y()
		<< std::endl;
	show();
	return dropAreaUnderCursor();
}


//============================================================================
void CDockOverlay::hideOverlay()
{
    qInfo() << "hideDockOverlay() _fullAreaDrop = false";
	hide();
	d->TargetWidget.clear();
	d->TargetRect = QRect();
	d->LastLocation = InvalidDockWidgetArea;
}


//============================================================================
void CDockOverlay::enableDropPreview(bool Enable)
{
	d->DropPreviewEnabled = Enable;
	update();
}


//============================================================================
void CDockOverlay::paintEvent(QPaintEvent* event)
{
	// Draw rect based on location
	if (!d->DropPreviewEnabled)
	{
		return;
	}

	QRect r = rect();
	const DockWidgetArea da = dropAreaUnderCursor();
	//std::cout << "CursorLocation: " << dropAreaUnderCursor() << std::endl;
	switch (da)
	{
    case TopDockWidgetArea: r.setHeight(r.height() / 2); break;
	case RightDockWidgetArea: r.setX(r.width() / 2); break;
	case BottomDockWidgetArea: r.setY(r.height() / 2); break;
	case LeftDockWidgetArea: r.setWidth(r.width() / 2); break;
	case CenterDockWidgetArea: r = rect();break;
	default: return;
	}
	QPainter painter(this);
    QColor Color = palette().color(QPalette::Active, QPalette::Highlight);
    Color.setAlpha(64);
    painter.setPen(Qt::NoPen);
	painter.fillRect(r, Color);
}


//============================================================================
void CDockOverlay::showEvent(QShowEvent*)
{
	d->Cross->show();
	QWidget* w = parentWidget() ? parentWidget() : d->TargetWidget.data();
	QRect WidgetRect = w->rect();
	QPoint Pos(WidgetRect.left(), WidgetRect.center().y());
}


//============================================================================
void CDockOverlay::hideEvent(QHideEvent*)
{
	d->Cross->hide();
}


//============================================================================
static int areaAlignment(const DockWidgetArea area)
{
	switch (area)
	{
		case TopDockWidgetArea: return (int) Qt::AlignHCenter | Qt::AlignBottom;
		case RightDockWidgetArea: return (int) Qt::AlignLeft | Qt::AlignVCenter;
		case BottomDockWidgetArea: return (int) Qt::AlignHCenter | Qt::AlignTop;
		case LeftDockWidgetArea: return (int) Qt::AlignRight | Qt::AlignVCenter;
		case CenterDockWidgetArea:  return (int) Qt::AlignCenter;
		default: return Qt::AlignCenter;
	}
}

//============================================================================
// DockOverlayCrossPrivate
//============================================================================
QPoint DockOverlayCrossPrivate::areaGridPosition(const DockWidgetArea area)
{
	if (CDockOverlay::ModeDockAreaOverlay == Mode)
	{
		switch (area)
		{
			case TopDockWidgetArea: return QPoint(1, 2);
			case RightDockWidgetArea: return QPoint(2, 3);
			case BottomDockWidgetArea: return QPoint(3, 2);
			case LeftDockWidgetArea: return QPoint(2, 1);
			case CenterDockWidgetArea: return QPoint(2, 2);
			default: return QPoint();
		}
	}
	else
	{
		switch (area)
		{
			case TopDockWidgetArea: return QPoint(0, 2);
			case RightDockWidgetArea: return QPoint(2, 4);
			case BottomDockWidgetArea: return QPoint(4, 2);
			case LeftDockWidgetArea: return QPoint(2, 0);
			case CenterDockWidgetArea: return QPoint(2, 2);
			default: return QPoint();
		}
	}
}


//============================================================================
CDockOverlayCross::CDockOverlayCross(CDockOverlay* overlay) :
	QWidget(overlay->parentWidget()),
	d(new DockOverlayCrossPrivate(this))
{
	d->DockOverlay = overlay;
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	setWindowTitle("DockOverlayCross");
	setAttribute(Qt::WA_TranslucentBackground);

	d->GridLayout = new QGridLayout();
	d->GridLayout->setSpacing(0);
	setLayout(d->GridLayout);
}


//============================================================================
CDockOverlayCross::~CDockOverlayCross()
{
	delete d;
}


//============================================================================
void CDockOverlayCross::setupOverlayCross(CDockOverlay::eMode Mode)
{
	d->Mode = Mode;

	QHash<DockWidgetArea, QWidget*> areaWidgets;
	areaWidgets.insert(TopDockWidgetArea, createDropIndicatorWidget(TopDockWidgetArea));
	areaWidgets.insert(RightDockWidgetArea, createDropIndicatorWidget(RightDockWidgetArea));
	areaWidgets.insert(BottomDockWidgetArea, createDropIndicatorWidget(BottomDockWidgetArea));
	areaWidgets.insert(LeftDockWidgetArea, createDropIndicatorWidget(LeftDockWidgetArea));
	areaWidgets.insert(CenterDockWidgetArea, createDropIndicatorWidget(CenterDockWidgetArea));

	setAreaWidgets(areaWidgets);
}


//============================================================================
void CDockOverlayCross::setAreaWidgets(const QHash<DockWidgetArea, QWidget*>& widgets)
{
	// Delete old widgets.
	QMutableHashIterator<DockWidgetArea, QWidget*> i(d->DropIndicatorWidgets);
	while (i.hasNext())
	{
		i.next();
		QWidget* widget = i.value();
		d->GridLayout->removeWidget(widget);
		delete widget;
		i.remove();
	}

	// Insert new widgets into grid.
	d->DropIndicatorWidgets = widgets;
	QHashIterator<DockWidgetArea, QWidget*> i2(d->DropIndicatorWidgets);
	while (i2.hasNext())
	{
		i2.next();
		const DockWidgetArea area = i2.key();
		QWidget* widget = i2.value();
		QPoint p = d->areaGridPosition(area);
		d->GridLayout->addWidget(widget, p.x(), p.y(), (Qt::Alignment) areaAlignment(area));
	}

	if (CDockOverlay::ModeDockAreaOverlay == d->Mode)
	{
		d->GridLayout->setContentsMargins(0, 0, 0, 0);
		d->GridLayout->setRowStretch(0, 1);
		d->GridLayout->setRowStretch(1, 0);
		d->GridLayout->setRowStretch(2, 0);
		d->GridLayout->setRowStretch(3, 0);
		d->GridLayout->setRowStretch(4, 1);

		d->GridLayout->setColumnStretch(0, 1);
		d->GridLayout->setColumnStretch(1, 0);
		d->GridLayout->setColumnStretch(2, 0);
		d->GridLayout->setColumnStretch(3, 0);
		d->GridLayout->setColumnStretch(4, 1);
	}
	else
	{
		d->GridLayout->setContentsMargins(4, 4, 4, 4);
		d->GridLayout->setRowStretch(0, 0);
		d->GridLayout->setRowStretch(1, 1);
		d->GridLayout->setRowStretch(2, 1);
		d->GridLayout->setRowStretch(3, 1);
		d->GridLayout->setRowStretch(4, 0);

		d->GridLayout->setColumnStretch(0, 0);
		d->GridLayout->setColumnStretch(1, 1);
		d->GridLayout->setColumnStretch(2, 1);
		d->GridLayout->setColumnStretch(3, 1);
		d->GridLayout->setColumnStretch(4, 0);
	}
	reset();
}


//============================================================================
DockWidgetArea CDockOverlayCross::cursorLocation() const
{
	const QPoint pos = mapFromGlobal(QCursor::pos());
	QHashIterator<DockWidgetArea, QWidget*> i(d->DropIndicatorWidgets);
	while (i.hasNext())
	{
		i.next();
		if (d->DockOverlay->allowedAreas().testFlag(i.key())
			&& i.value()
			&& i.value()->isVisible()
			&& i.value()->geometry().contains(pos))
		{
			return i.key();
		}
	}
	return InvalidDockWidgetArea;
}


//============================================================================
void CDockOverlayCross::showEvent(QShowEvent*)
{
	resize(d->DockOverlay->size());
	QPoint TopLeft = d->DockOverlay->pos();
	QPoint Offest((this->width() - d->DockOverlay->width()) / 2,
		(this->height() - d->DockOverlay->height()) / 2);
	QPoint CrossTopLeft = TopLeft - Offest;
	move(CrossTopLeft);
}


//============================================================================
void CDockOverlayCross::reset()
{
	QList<DockWidgetArea> allAreas;
	allAreas << TopDockWidgetArea << RightDockWidgetArea
		<< BottomDockWidgetArea << LeftDockWidgetArea << CenterDockWidgetArea;
	const DockWidgetAreas allowedAreas = d->DockOverlay->allowedAreas();

	// Update visibility of area widgets based on allowedAreas.
	for (int i = 0; i < allAreas.count(); ++i)
	{
		QPoint p = d->areaGridPosition(allAreas.at(i));
		QLayoutItem* item = d->GridLayout->itemAtPosition(p.x(), p.y());
		QWidget* w = nullptr;
		if (item && (w = item->widget()) != nullptr)
		{
			w->setVisible(allowedAreas.testFlag(allAreas.at(i)));
		}
	}
}



} // namespace ads
//----------------------------------------------------------------------------

