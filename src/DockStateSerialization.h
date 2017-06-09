#ifndef DockStateSerializationH
#define DockStateSerializationH
/*******************************************************************************
** Qt Advanced Docking System
** Copyright (C) 2017 Uwe Kindler
** 
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public
** License along with this library; If not, see <http://www.gnu.org/licenses/>.
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
