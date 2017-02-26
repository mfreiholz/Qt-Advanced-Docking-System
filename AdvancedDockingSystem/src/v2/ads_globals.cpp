//============================================================================
/// \file   ads_globals.cpp
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Implementation of 
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QSplitter>
#include <QVariant>

#include "ads_globals.h"

namespace ads
{

namespace internal
{
//============================================================================
QSplitter* newSplitter(Qt::Orientation orientation, QWidget* parent)
{
	QSplitter* s = new QSplitter(orientation, parent);
	s->setProperty("ads-splitter", QVariant(true));
	s->setChildrenCollapsible(false);
	s->setOpaqueResize(false);
	return s;
}

//============================================================================
QPair<Qt::Orientation, bool> dockAreaInsertParameters(DockWidgetArea Area)
{
	switch (Area)
    {
	case TopDockWidgetArea: return QPair<Qt::Orientation, bool>(Qt::Vertical, false);
	case RightDockWidgetArea: return QPair<Qt::Orientation, bool>(Qt::Horizontal, true);
	case CenterDockWidgetArea:
	case BottomDockWidgetArea: return QPair<Qt::Orientation, bool>(Qt::Vertical, true);
	case LeftDockWidgetArea: return QPair<Qt::Orientation, bool>(Qt::Horizontal, false);
	default: QPair<Qt::Orientation, bool>(Qt::Vertical, false);
    } // switch (Area)

	return QPair<Qt::Orientation, bool>(Qt::Vertical, false);
}


} // namespace internal
} // namespace ads



//---------------------------------------------------------------------------
// EOF ads_globals.cpp
