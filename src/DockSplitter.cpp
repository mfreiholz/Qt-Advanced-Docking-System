//============================================================================
/// \file   DockSplitter.cpp
/// \author Uwe Kindler
/// \date   24.03.2017
/// \brief  Implementation of CDockSplitter
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <DockSplitter.h>

#include <QDebug>

namespace ads
{

//============================================================================
CDockSplitter::~CDockSplitter()
{
	qDebug() << "~CDockSplitter";
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
