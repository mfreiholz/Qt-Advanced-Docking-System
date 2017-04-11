//============================================================================
/// \file   DockSplitter.cpp
/// \author Uwe Kindler
/// \date   24.03.2017
/// \brief  Implementation of CDockSplitter
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockSplitter.h"

#include <QDebug>
#include <QChildEvent>

#include "DockAreaWidget.h"

namespace ads
{
/**
 * Private dock splitter data
 */
struct DockSplitterPrivate
{
	CDockSplitter* _this;
	int VisibleContentCount = 0;

	DockSplitterPrivate(CDockSplitter* _public) : _this(_public) {}
};

//============================================================================
CDockSplitter::CDockSplitter(QWidget *parent)
	: QSplitter(parent),
	  d(new DockSplitterPrivate(this))
{

}


//============================================================================
CDockSplitter::CDockSplitter(Qt::Orientation orientation, QWidget *parent)
	: QSplitter(orientation, parent),
	  d(new DockSplitterPrivate(this))
{

}

//============================================================================
CDockSplitter::~CDockSplitter()
{
	qDebug() << "~CDockSplitter";
	delete d;
}


//============================================================================
bool CDockSplitter::hasVisibleContent() const
{
	// TODO Cache or precalculate this to speed up
	for (int i = 0; i < count(); ++i)
	{
		if (widget(i)->isVisibleTo(this))
		{
			return true;
		}
	}

	return false;
}

} // namespace ads

//---------------------------------------------------------------------------
// EOF DockSplitter.cpp
