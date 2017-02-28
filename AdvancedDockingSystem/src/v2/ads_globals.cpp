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
CDockInsertParam dockAreaInsertParameters(DockWidgetArea Area)
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

	return CDockInsertParam(Qt::Vertical, false);
}


//============================================================================
QSplitter* findParentSplitter(QWidget* w)
{
	QWidget* parentWidget = w;
	do
	{
		QSplitter* splitter = dynamic_cast<QSplitter*>(parentWidget);
		if (splitter)
		{
			return splitter;
		}
		parentWidget = parentWidget->parentWidget();
	}
	while (parentWidget);
	return 0;
}

} // namespace internal
} // namespace ads



//---------------------------------------------------------------------------
// EOF ads_globals.cpp
