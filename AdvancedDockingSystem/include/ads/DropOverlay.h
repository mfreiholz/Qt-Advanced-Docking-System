#ifndef DROP_OVERLAY_H
#define DROP_OVERLAY_H

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
	DropOverlay(QWidget* parent);
	virtual ~DropOverlay();

	void setAllowedAreas(DropAreas areas);
	DropAreas allowedAreas() const;

	void setAreaWidgets(const QHash<DropArea, QWidget*>& widgets);

	DropArea cursorLocation() const;

	DropArea showDropOverlay(QWidget* target);
	void showDropOverlay(QWidget* target, const QRect& targetAreaRect);
	void hideDropOverlay();

protected:
	virtual void paintEvent(QPaintEvent *e);
	virtual void showEvent(QShowEvent* e);
	virtual void hideEvent(QHideEvent* e);
	virtual void resizeEvent(QResizeEvent* e);
	virtual void moveEvent(QMoveEvent* e);

private:
	DropAreas _allowedAreas;
	DropOverlayCross* _cross;

	bool _fullAreaDrop;
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

	void setAreaWidgets(const QHash<DropArea, QWidget*>& widgets);
	DropArea cursorLocation() const;

protected:
	virtual void showEvent(QShowEvent* e);

private:
	void reset();

private:
	DropOverlay* _overlay;
	QHash<DropArea, QWidget*> _widgets;
	QGridLayout* _grid;
};

ADS_NAMESPACE_END
#endif
