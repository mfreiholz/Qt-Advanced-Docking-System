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

// DropOverlay paints a translucent rectangle over another widget.
// It can also show different types of drop area indicators.
class DropOverlay : public QFrame
{
	Q_OBJECT
	friend class DropOverlayCross;

public:
	DropOverlay(QWidget* parent);
	virtual ~DropOverlay();

	void setAllowedAreas(DropAreas areas);
	DropAreas allowedAreas() const;

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
	DropOverlayCross* _splitAreas;

	bool _fullAreaDrop;
	QPointer<QWidget> _target;
	QRect _targetRect;
	DropArea _lastLocation;
};

// DropSplitAreas shows a cross with 5 different drop area possibilities.
class DropOverlayCross : public QWidget
{
	Q_OBJECT

public:
	DropOverlayCross(DropOverlay* overlay);
	void reset();
	DropArea cursorLocation() const;
	void setWidgetForArea(QWidget* widget, DropArea area);

protected:
	virtual void showEvent(QShowEvent* e);

private:
	DropOverlay* _overlay;
	QHash<DropArea, QWidget*> _widgets;
	QGridLayout* _grid;
};

ADS_NAMESPACE_END
#endif
