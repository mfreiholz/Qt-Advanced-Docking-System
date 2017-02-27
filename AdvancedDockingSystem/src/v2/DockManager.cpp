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
/// \file   DockManager.cpp
/// \author Uwe Kindler
/// \date   26.02.2017
/// \brief  Implementation of CDockManager class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockManager.h"

#include <QMainWindow>

namespace ads
{
/**
 * Private data class of CDockManager class (pimpl)
 */
struct DockManagerPrivate
{
	CDockManager* _this;

	/**
	 * Private data constructor
	 */
	DockManagerPrivate(CDockManager* _public);
};
// struct DockManagerPrivate

//============================================================================
DockManagerPrivate::DockManagerPrivate(CDockManager* _public) :
	_this(_public)
{

}


//============================================================================
CDockManager::CDockManager(QWidget *parent) :
	CDockContainerWidget(this, parent),
	d(new DockManagerPrivate(this))
{
	QMainWindow* MainWindow = dynamic_cast<QMainWindow*>(parent);
	if (MainWindow)
	{
		MainWindow->setCentralWidget(this);
	}
}

//============================================================================
CDockManager::~CDockManager()
{
	delete d;
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockManager.cpp
