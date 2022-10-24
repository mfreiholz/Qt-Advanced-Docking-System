//============================================================================
/// \file   ResizeHandle.cpp
/// \author Uwe Kindler
/// \date   24.10.2022
/// \brief  Implementation of CResizeHandle class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "ResizeHandle.h"

#include <QDebug>
#include <QStyle>
#include <QStyleOption>
#include <QMouseEvent>

#include "ads_globals.h"

namespace ads
{
/**
 * Private data class of CResizeHandle class (pimpl)
 */
struct ResizeHandlePrivate
{
	CResizeHandle *_this;
	Qt::Edge HandlePosition = Qt::LeftEdge;
	QWidget* Target = nullptr;
	int MouseOffset = 0;
	bool Pressed = false;
	int MinSize = 0;
	int MaxSize = 1;

	/**
	 * Private data constructor
	 */
	ResizeHandlePrivate(CResizeHandle *_public);

	/**
	 * Pick position component from pos depending on orientation
	 */
	int pick(const QPoint &pos) const
    {
		return _this->orientation() == Qt::Horizontal ? pos.x() : pos.y();
    }

	/**
	 * Returns true, if orientation is horizontal
	 */
	bool isHorizontal() const
	{
		return _this->orientation() == Qt::Horizontal;
	}
};
// struct ResizeHandlePrivate

//============================================================================
ResizeHandlePrivate::ResizeHandlePrivate(CResizeHandle *_public) :
	_this(_public)
{

}

//============================================================================
CResizeHandle::CResizeHandle(Qt::Edge HandlePosition, QWidget* parent) :
	Super(parent),
	d(new ResizeHandlePrivate(this))
{
	d->Target = parent;
	setHandlePosition(HandlePosition);
	setMinResizeSize(48);
	setMaxResizeSize(d->isHorizontal() ? parent->height() : parent->width());

	if (!d->isHorizontal())
	{
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	}
	else
	{
		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	}
}


//============================================================================
CResizeHandle::~CResizeHandle()
{
	delete d;
}


//============================================================================
void CResizeHandle::mouseMoveEvent(QMouseEvent* e)
{
    if (!(e->buttons() & Qt::LeftButton))
    {
        return;
    }
    int pos = d->pick(e->pos()) - d->MouseOffset;
    auto OldGeometry = d->Target->geometry();
	auto NewGeometry = OldGeometry;
	switch (d->HandlePosition)
	{
	case Qt::LeftEdge:
		 {
			 NewGeometry.adjust(pos, 0, 0, 0);
			 int Size = qBound(d->MinSize, NewGeometry.width(), d->MaxSize);
			 NewGeometry.setWidth(Size);
			 NewGeometry.moveTopRight(OldGeometry.topRight());
		 }
		 break;


	case Qt::RightEdge:
		 {
			 NewGeometry.adjust(0, 0, pos, 0);
			 int Size = qBound(d->MinSize, NewGeometry.width(), d->MaxSize);
			 NewGeometry.setWidth(Size);
		 }
		 break;

	case Qt::TopEdge:
	     {
			 NewGeometry.adjust(0, pos, 0, 0);
			 int Size = qBound(d->MinSize, NewGeometry.height(), d->MaxSize);
			 NewGeometry.setHeight(Size);
			 NewGeometry.moveBottomLeft(OldGeometry.bottomLeft());
		 }
		 break;

	case Qt::BottomEdge:
	     {
			 NewGeometry.adjust(0, 0, 0, pos);
			 int Size = qBound(d->MinSize, NewGeometry.height(), d->MaxSize);
			 NewGeometry.setHeight(Size);
		 }
		 break;
	}
	d->Target->setGeometry(NewGeometry);
	//qDebug() << "globalGeometry(): " << internal::globalGeometry(d->Target);
	//qDebug() << "parentGlobalGeometry(): " << internal::globalGeometry(d->Target->parentWidget());


    /*if (opaqueResize())
    {
        moveSplitter(pos);
    }
    else
    {
        //d->s->setRubberBand(closestLegalPosition(pos));
    }*/
}


//============================================================================
void CResizeHandle::mousePressEvent(QMouseEvent* e)
{
	qDebug() << "CResizeHandle::mousePressEvent " << e;
    if (e->button() == Qt::LeftButton)
    {
        d->MouseOffset = d->pick(e->pos());
        d->Pressed = true;
        update();
    }
}


//============================================================================
void CResizeHandle::mouseReleaseEvent(QMouseEvent* e)
{
	qDebug() << "CResizeHandle::mouseReleaseEvent " << e;
    /*if (!opaqueResize() && e->button() == Qt::LeftButton) {
        int pos = d->pick(parentWidget()->mapFromGlobal(e->globalPos()))
                     - d->mouseOffset;
        d->s->setRubberBand(-1);
        moveSplitter(pos);
    }*/
    if (e->button() == Qt::LeftButton)
    {
        d->Pressed = false;
        update();
    }
}


//============================================================================
void CResizeHandle::setHandlePosition(Qt::Edge HandlePosition)
{
	d->HandlePosition = HandlePosition;
	switch (d->HandlePosition)
	{
	case Qt::LeftEdge: // fall through
	case Qt::RightEdge: setCursor(Qt::SizeHorCursor); break;

	case Qt::TopEdge: // fall through
	case Qt::BottomEdge: setCursor(Qt::SizeVerCursor); break;
	}
}


//============================================================================
Qt::Edge CResizeHandle::handlePostion() const
{
	return d->HandlePosition;
}


//============================================================================
Qt::Orientation CResizeHandle::orientation() const
{
	switch (d->HandlePosition)
	{
	case Qt::LeftEdge: // fall through
	case Qt::RightEdge: return Qt::Horizontal;

	case Qt::TopEdge: // fall through
	case Qt::BottomEdge: return Qt::Vertical;
	}

	return Qt::Horizontal;
}


//============================================================================
QSize CResizeHandle::sizeHint() const
{
	QSize Result;
	switch (d->HandlePosition)
	{
	case Qt::LeftEdge: // fall through
	case Qt::RightEdge: Result = QSize(4, d->Target->height()); break;

	case Qt::TopEdge: // fall through
	case Qt::BottomEdge: Result = QSize(d->Target->width(), 4); break;
	}

	qDebug() << "CResizeHandle::sizeHint(): " << Result;
	return Result;
	/*parentWidget()->style()->sizeFromContents(QStyle::CT_Splitter, &opt, QSize(hw, hw), d->s)
			.expandedTo(QApplication::globalStrut());*/
}


//============================================================================
bool CResizeHandle::isResizing() const
{
	return d->Pressed;
}


//============================================================================
void CResizeHandle::setMinResizeSize(int MinSize)
{
	qDebug() << "CResizeHandle::setMinResizeSize " << MinSize;
	d->MinSize = MinSize;
}


//============================================================================
void CResizeHandle::setMaxResizeSize(int MaxSize)
{
	qDebug() << "CResizeHandle::setMaxResizeSize " << MaxSize;
	d->MaxSize = MaxSize;
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF ResizeHandle.cpp
