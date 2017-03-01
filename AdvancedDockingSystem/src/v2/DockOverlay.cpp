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
/// \file   DropOverlay.cpp
/// \author Uwe Kindler
/// \date   01.03.2017
/// \brief  Implementation of CDropOverlay class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockOverlay.h"

#include <iostream>

namespace ads
{
/**
 * Private data class of CDropOverlay class (pimpl)
 */
struct DockOverlayPrivate
{
	CDockOverlay* _this;

	/**
	 * Private data constructor
	 */
	DockOverlayPrivate(CDockOverlay* _public);
};
// struct DropOverlayPrivate

//============================================================================
DockOverlayPrivate::DockOverlayPrivate(CDockOverlay* _public) :
	_this(_public)
{

}

//============================================================================
CDockOverlay::CDockOverlay(QWidget *parent) :
	QFrame(parent),
	d(new DockOverlayPrivate(this))
{
	setStyleSheet("ads--CDockOverlay {background: palette(highlight);}");
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	setWindowOpacity(0.2);
	setWindowTitle("DockOverlay");
	//setAttribute(Qt::WA_NoSystemBackground);
	//setAttribute(Qt::WA_TranslucentBackground);

	setAttribute(Qt::WA_TransparentForMouseEvents);
	setWindowFlags(windowFlags() | Qt::WindowTransparentForInput);
	setVisible(false);
}

//============================================================================
CDockOverlay::~CDockOverlay()
{
	delete d;
}


//============================================================================
DockWidgetArea CDockOverlay::showOverlay(QWidget* target)
{
	std::cout << "CDockOverlay::showOverlay" << std::endl;
	resize(target->size());
	move(target->mapToGlobal(target->rect().topLeft()));
	this->show();
	return NoDockWidgetArea;
}

//============================================================================
void CDockOverlay::hideOverlay()
{
	this->hide();
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DropOverlay.cpp
