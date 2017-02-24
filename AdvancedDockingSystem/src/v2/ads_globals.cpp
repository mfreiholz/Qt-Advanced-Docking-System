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

namespace ads
{

namespace internal
{
QSplitter* newSplitter(Qt::Orientation orientation, QWidget* parent)
{
	QSplitter* s = new QSplitter(orientation, parent);
	s->setProperty("ads-splitter", QVariant(true));
	s->setChildrenCollapsible(false);
	s->setOpaqueResize(false);
	return s;
}


} // namespace internal
} // namespace ads



//---------------------------------------------------------------------------
// EOF ads_globals.cpp
