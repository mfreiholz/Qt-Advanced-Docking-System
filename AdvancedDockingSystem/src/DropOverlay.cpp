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

DropOverlay::DropOverlay(DropAreas areas, QWidget *parent) :
	QFrame(parent),
	_splitAreas(NULL),
	_fullAreaDrop(false)
{
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	setWindowOpacity(0.2);
	setWindowTitle("DropOverlay");

	QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	setLayout(l);

	_splitAreas = new DropSplitAreas(areas, parent);
	_splitAreas->move(pos());
	_splitAreas->resize(size());
	_splitAreas->setVisible(true);
}

DropOverlay::~DropOverlay()
{
	if (_splitAreas)
	{
		delete _splitAreas;
		_splitAreas = NULL;
	}
}

DropArea DropOverlay::cursorLocation() const
{
	DropArea loc = InvalidDropArea;
	if (_splitAreas)
	{
		loc = _splitAreas->cursorLocation();
	}
	return loc;
}

void DropOverlay::paintEvent(QPaintEvent*)
{
	QPainter p(this);

	// Always draw drop-rect over the entire rect()
	if (_fullAreaDrop)
	{
		QRect r = rect();
		p.fillRect(r, QBrush(QColor(0, 100, 255), Qt::Dense4Pattern));
		p.setBrush(QBrush(QColor(0, 100, 255)));
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
		p.fillRect(r, QBrush(QColor(0, 100, 255), Qt::Dense4Pattern));
		p.setBrush(QBrush(QColor(0, 100, 255)));
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

void DropOverlay::resizeEvent(QResizeEvent* e)
{
	// Keep it in center of DropOverlay
	if (_splitAreas)
	{
		_splitAreas->resize(e->size());
	}
}

void DropOverlay::moveEvent(QMoveEvent* e)
{
	// Keep it in center of DropOverlay
	if (_splitAreas)
	{
		_splitAreas->move(e->pos());
	}
}

///////////////////////////////////////////////////////////////////////

DropSplitAreas::DropSplitAreas(DropAreas areas, QWidget* parent) :
	AbstractDropAreas(parent),
	_top(0),
	_right(0),
	_bottom(0),
	_left(0),
	_center(0)
{
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	setWindowTitle("DropSplitAreas");

	setAttribute(Qt::WA_TranslucentBackground);

	QGridLayout* grid = new QGridLayout();
	grid->setContentsMargins(0, 0, 0, 0);
	grid->setSpacing(6);

	if (areas.testFlag(ADS_NS::TopDropArea))
	{
		_top = createDropWidget(":/img/split-top.png");
		grid->addWidget(_top, 0, 1, Qt::AlignHCenter | Qt::AlignBottom);
	}
	if (areas.testFlag(ADS_NS::RightDropArea))
	{
		_right = createDropWidget(":/img/split-right.png");
		grid->addWidget(_right, 1, 2, Qt::AlignLeft | Qt::AlignVCenter);
	}
	if (areas.testFlag(ADS_NS::BottomDropArea))
	{
		_bottom = createDropWidget(":/img/split-bottom.png");
		grid->addWidget(_bottom, 2, 1, Qt::AlignHCenter | Qt::AlignTop);
	}
	if (areas.testFlag(ADS_NS::LeftDropArea))
	{
		_left = createDropWidget(":/img/split-left.png");
		grid->addWidget(_left, 1, 0, Qt::AlignRight | Qt::AlignVCenter);
	}
	if (areas.testFlag(ADS_NS::CenterDropArea))
	{
		_center = createDropWidget(":/img/dock-center.png");
		grid->addWidget(_center, 1, 1, Qt::AlignCenter);
	}

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
	bl2->addLayout(grid, 0);
	bl2->addStretch(1);
	bl1->addStretch(1);
}

DropArea DropSplitAreas::cursorLocation() const
{
	DropArea loc = InvalidDropArea;
	QPoint pos = mapFromGlobal(QCursor::pos());
	if (_top && _top->geometry().contains(pos))
	{
		loc = TopDropArea;
	}
	else if (_right && _right->geometry().contains(pos))
	{
		loc = RightDropArea;
	}
	else if (_bottom && _bottom->geometry().contains(pos))
	{
		loc = BottomDropArea;
	}
	else if (_left && _left->geometry().contains(pos))
	{
		loc = LeftDropArea;
	}
	else if (_center && _center->geometry().contains(pos))
	{
		loc = CenterDropArea;
	}
	return loc;
}

// Globals ////////////////////////////////////////////////////////////

static QPointer<DropOverlay> MyOverlay;
static QPointer<QWidget> MyOverlayParent;
static QRect MyOverlayParentRect;
static DropArea MyOverlayLastLocation = InvalidDropArea;

DropArea showDropOverlay(QWidget* parent, DropAreas areas)
{
	if (MyOverlay)
	{
		if (MyOverlayParent == parent)
		{
			// Hint: We could update geometry of overlay here.
			DropArea da = MyOverlay->cursorLocation();
			if (da != MyOverlayLastLocation)
			{
				MyOverlay->repaint();
				MyOverlayLastLocation = da;
			}
			return da;
		}
		hideDropOverlay();
	}

	// Create new overlay and move it directly over the "parent" widget.
	MyOverlay = new DropOverlay(areas, parent);
	MyOverlay->resize(parent->size());
	MyOverlay->move(parent->mapToGlobal(parent->rect().topLeft()));
	MyOverlay->show();
	MyOverlayParent = parent;
	return MyOverlay->cursorLocation();
}

void showDropOverlay(QWidget* parent, const QRect& areaRect, DropAreas areas)
{
	if (MyOverlay)
	{
		if (MyOverlayParent == parent && MyOverlayParentRect == areaRect)
		{
			return;
		}
		hideDropOverlay();
	}

	// Create overlay and move it to the parent's areaRect
	MyOverlay = new DropOverlay(areas, parent);
	MyOverlay->setFullAreaDropEnabled(true);
	MyOverlay->resize(areaRect.size());
	MyOverlay->move(parent->mapToGlobal(QPoint(areaRect.x(), areaRect.y())));
	MyOverlay->show();
	MyOverlayParent = parent;
	MyOverlayParentRect = areaRect;
	return;
}

void hideDropOverlay()
{
	if (MyOverlay)
	{
		MyOverlay->hide();
		delete MyOverlay;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
		MyOverlayParent.clear();
#else
		MyOverlayParent = 0;
#endif
		MyOverlayParentRect = QRect();
		MyOverlayLastLocation = InvalidDropArea;
	}
}

ADS_NAMESPACE_END
