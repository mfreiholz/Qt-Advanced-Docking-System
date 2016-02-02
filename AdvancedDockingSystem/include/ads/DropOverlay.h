#ifndef DROP_OVERLAY_H
#define DROP_OVERLAY_H

#include <QFrame>
#include <QRect>

#include "ads/API.h"

ADS_NAMESPACE_BEGIN

// DropOverlay paints a translucent rectangle over another widget.
// It can also show different types of drop area indicators.
class DropOverlay : public QFrame
{
	Q_OBJECT

public:
	DropOverlay(QWidget* parent);
	virtual ~DropOverlay();
	DropArea cursorLocation() const;

protected:
	virtual void paintEvent(QPaintEvent *e);
	virtual void resizeEvent(QResizeEvent* e);
	virtual void moveEvent(QMoveEvent* e);

private:
	class DropSplitAreas* _splitAreas;
};

// AbstractDropAreas is used as base for drop area indicator widgets.
class AbstractDropAreas : public QWidget
{
public:
	AbstractDropAreas(QWidget* parent) : QWidget(parent) {}
	virtual DropArea cursorLocation() const = 0;
};

// DropSplitAreas shows a cross with 5 different drop area possibilities.
class DropSplitAreas : public AbstractDropAreas
{
	Q_OBJECT

public:
	DropSplitAreas(QWidget* parent);
	virtual DropArea cursorLocation() const;

private:
	QWidget* _top;
	QWidget* _right;
	QWidget* _bottom;
	QWidget* _left;
	QWidget* _center;
};


DropArea showDropOverlay(QWidget* parent);
void showDropOverlay(QWidget* parent, const QRect& areaRect);
void hideDropOverlay();

ADS_NAMESPACE_END
#endif

