#ifndef DockOverlayH
#define DockOverlayH
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
/// \file   DockOverlay.h
/// \author Uwe Kindler
/// \date   01.03.2017
/// \brief  Declaration of CDockOverlay class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>

#include "ads_globals.h"

namespace ads
{
struct DockOverlayPrivate;

/**
 * DockOverlay paints a translucent rectangle over another widget. The geometry
 * of the rectangle is based on drop area at the mouse location.
 */
class CDockOverlay : public QFrame
{
	Q_OBJECT
private:
	DockOverlayPrivate* d; ///< private data (pimpl)
	friend class DockOverlayPrivate;
protected:
public:
	/**
	 * Default Constructor
	 */
	CDockOverlay(QWidget* parent = 0);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockOverlay();

	/**
	 * Shows the dock overlay for the given target widget
	 */
	DockWidgetArea showOverlay(QWidget* target);

	/**
	 * Hides this verlay
	 */
	void hideOverlay();
}; // class DockOverlay
}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // DockOverlayH
