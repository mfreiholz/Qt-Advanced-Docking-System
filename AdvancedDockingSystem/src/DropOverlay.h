#ifndef DROP_OVERLAY_H
#define DROP_OVERLAY_H
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
#include <QPointer>
#include <QHash>
#include <QRect>
#include <QFrame>
class QGridLayout;

#include "ads/API.h"

ADS_NAMESPACE_BEGIN
class DropOverlayCross;

/*!
 * DropOverlay paints a translucent rectangle over another widget. The geometry
 * of the rectangle is based on the mouse location.
 */
class ADS_EXPORT_API DropOverlay : public QFrame
{
	Q_OBJECT
	friend class DropOverlayCross;

public:
	enum eMode
	{
		ModeSectionOverlay,
		ModeContainerOverlay
	};

	DropOverlay(QWidget* parent, eMode Mode = ModeSectionOverlay);
	virtual ~DropOverlay();

	void setAllowedAreas(DropAreas areas);

	/**
	 * Returns flags with all allowed drop areas
	 */
	DropAreas allowedAreas() const;

	/**
	 * Returns the drop area under the current cursor location
	 */
	DropArea dropAreaUnderCursor() const;

	DropArea showDropOverlay(QWidget* target);
	void showDropOverlay(QWidget* target, const QRect& targetAreaRect);
	void hideDropOverlay();

	/**
	 * Creates a drop indicator widget for the given drop area
	 */
	static QWidget* createDropIndicatorWidget(DropArea dropArea);

protected:
	virtual void paintEvent(QPaintEvent *e);
	virtual void showEvent(QShowEvent* e);
	virtual void hideEvent(QHideEvent* e);
	virtual void resizeEvent(QResizeEvent* e);
	virtual void moveEvent(QMoveEvent* e);

private:
	DropAreas _allowedAreas;
	DropOverlayCross* _cross;
	QPointer<QWidget> _target;
	QRect _targetRect;
	DropArea _lastLocation;
};

/*!
 * DropOverlayCross shows a cross with 5 different drop area possibilities.
 * I could have handled everything inside DropOverlay, but because of some
 * styling issues it's better to have a separate class for the cross.
 */
class DropOverlayCross : public QWidget
{
	Q_OBJECT
	friend class DropOverlay;

public:
	DropOverlayCross(DropOverlay* overlay);
	virtual ~DropOverlayCross();

	DropArea cursorLocation() const;
	void setupOverlayCross(DropOverlay::eMode Mode);

protected:
	virtual void showEvent(QShowEvent* e);
	void setAreaWidgets(const QHash<DropArea, QWidget*>& widgets);

private:
	void reset();
	QPoint areaGridPosition(const DropArea area);

private:
	DropOverlay::eMode m_Mode = DropOverlay::ModeSectionOverlay;
	DropOverlay* m_DropOverlay;
	QHash<DropArea, QWidget*> m_DropIndicatorWidgets;
	QGridLayout* m_GridLayout;
};

ADS_NAMESPACE_END
#endif
