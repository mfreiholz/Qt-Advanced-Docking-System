#ifndef ads_globalsH
#define ads_globalsH
//============================================================================
/// \file   ads_globals.h
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Declaration of 
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
class QSplitter;

namespace ads
{
enum DockWidgetArea
{
	NoDockWidgetArea = 0x00,
	LeftDockWidgetArea = 0x01,
	RightDockWidgetArea = 0x02,
	TopDockWidgetArea = 0x04,
	BottomDockWidgetArea = 0x08,
	CenterDockWidgetArea = 0x10,

	OuterAreas = TopDockWidgetArea | LeftDockWidgetArea | RightDockWidgetArea | BottomDockWidgetArea,
	AllAreas = OuterAreas | CenterDockWidgetArea
};
Q_DECLARE_FLAGS(DockWidgetAreas, DockWidgetArea)

namespace internal
{
/**
 * Helper function to create new splitter widgets
 */
QSplitter* newSplitter(Qt::Orientation orientation, QWidget* parent);
} // namespace internal
} // namespace ads

//---------------------------------------------------------------------------
#endif // ads_globalsH
