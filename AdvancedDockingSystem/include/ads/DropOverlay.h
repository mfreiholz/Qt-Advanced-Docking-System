#ifndef DROP_OVERLAY_H
#define DROP_OVERLAY_H

#include <QPointer>
#include <QRect>
#include <QFrame>

#include "ads/API.h"

ADS_NAMESPACE_BEGIN
class DropSplitAreas;

// DropOverlay paints a translucent rectangle over another widget.
// It can also show different types of drop area indicators.
class DropOverlay : public QFrame
{
	Q_OBJECT

public:
	DropOverlay(QWidget* parent);
	virtual ~DropOverlay();

	void setDropAreas(DropAreas areas);
	void setFullAreaDropEnabled(bool enabled) { _fullAreaDrop = enabled; }
	DropArea cursorLocation() const;

	DropArea showDropOverlay(QWidget* target, DropAreas areas = AllAreas);
	void showDropOverlay(QWidget* target, const QRect& targetAreaRect, DropAreas areas = AllAreas);
	void hideDropOverlay();

protected:
	virtual void paintEvent(QPaintEvent *e);
	virtual void resizeEvent(QResizeEvent* e);
	virtual void moveEvent(QMoveEvent* e);

private:
	DropSplitAreas* _splitAreas;
	bool _fullAreaDrop;

	QPointer<QWidget> _target;
	QRect _targetRect;
	DropArea _lastLocation;
};

// DropSplitAreas shows a cross with 5 different drop area possibilities.
class DropSplitAreas : public QWidget
{
	Q_OBJECT

public:
	DropSplitAreas(DropAreas areas, QWidget* parent);
	DropArea cursorLocation() const;

private:
	QWidget* _top;
	QWidget* _right;
	QWidget* _bottom;
	QWidget* _left;
	QWidget* _center;
};

ADS_NAMESPACE_END
#endif
