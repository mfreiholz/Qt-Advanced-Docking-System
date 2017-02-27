#ifndef DockManagerH
#define DockManagerH
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
/// \file   DockManager.h
/// \author Uwe Kindler
/// \date   26.02.2017
/// \brief  Declaration of CDockManager class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockContainerWidget.h"

namespace ads
{
struct DockManagerPrivate;

/**
 * The central dock manager that maintains the complete docking system
 **/
class CDockManager : public CDockContainerWidget
{
	Q_OBJECT
private:
	DockManagerPrivate* d; ///< private data (pimpl)
	friend class DockManagerPrivate;
protected:
public:
	/**
	 * Default Constructor.
	 * If the given parent is a QMainWindow, the dck manager sets itself as the
	 * central widget
	 */
	CDockManager(QWidget* parent = 0);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockManager();
}; // class DockManager
} // namespace ads
//-----------------------------------------------------------------------------
#endif // DockManagerH
