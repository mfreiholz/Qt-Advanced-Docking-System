#ifndef DockStateSerializationH
#define DockStateSerializationH
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
/// \file   DockStateSerialization.h
/// \author Uwe Kindler
/// \date   26.02.2017
/// \brief  Declaration of serialization related data, constants and stuff
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================


namespace ads
{

namespace internal
{
// sentinel values used to validate state data
enum VersionMarkers
{
	VersionMarker = 0xff,
	ContainerMarker = 0xfe,
	SplitterMarker = 0xfd,
	DockAreaMarker = 0xfc,
	DockWidgetMarker = 0xfb
};

static const bool RestoreTesting = true;
static const bool Restore = false;
} // internal
} // namespace ads
//-----------------------------------------------------------------------------
#endif // DockManagerH
