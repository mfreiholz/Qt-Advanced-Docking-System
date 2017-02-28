#ifndef ads_globalsH
#define ads_globalsH
/*******************************************************************************
** QtAdcancedDockingSystem
** Copyright (C) 2017 Uwe Kindler
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program. If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


//============================================================================
/// \file   ads_globals.h
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Declaration of
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QPair>

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

	OuterDockAreas = TopDockWidgetArea | LeftDockWidgetArea | RightDockWidgetArea | BottomDockWidgetArea,
	AllDockAreas = OuterDockAreas | CenterDockWidgetArea
};
Q_DECLARE_FLAGS(DockWidgetAreas, DockWidgetArea)

namespace internal
{
/**
 * Helper function to create new splitter widgets
 */
QSplitter* newSplitter(Qt::Orientation orientation, QWidget* parent = 0);

/**
 * Convenience class for QPair to provide better naming than first and
 * second
 */
class CDockInsertParam : public QPair<Qt::Orientation, bool>
{
public:
	using QPair::QPair;
	Qt::Orientation orientation() const {return this->first;}
	bool append() const {return this->second;}
	int insertOffset() const {return append() ? 1 : 0;}
};

/**
 * Returns the insertion parameters for the given dock area
 */
CDockInsertParam dockAreaInsertParameters(DockWidgetArea Area);

/**
 * Returns the parent splitter of the given widget or 0 if the widget is not
 * child of any splitter
 */
QSplitter* findParentSplitter(QWidget* w);
} // namespace internal
} // namespace ads

//---------------------------------------------------------------------------
#endif // ads_globalsH
