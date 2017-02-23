#ifndef ADS_API_H
#define ADS_API_H
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

#include <QFlags>
class QWidget;
class QSplitter;

// DLL Export API
#ifdef _WIN32
	#if defined(ADS_IMPORT)
		#define ADS_EXPORT_API
	#elif defined(ADS_EXPORT)
		#define ADS_EXPORT_API __declspec(dllexport)
	#else
		#define ADS_EXPORT_API __declspec(dllimport)
	#endif
#else
  #define ADS_EXPORT_API
#endif


// Beautiful C++ stuff.
#define ADS_Expects(cond)
#define ADS_Ensures(cond)

// Indicates whether ADS should include animations.
//#define ADS_ANIMATIONS_ENABLED 1
//#define ADS_ANIMATION_DURATION 150

namespace ads
{
class CMainContainerWidget;
class SectionWidget;

enum DropArea
{
	InvalidDropArea = 0,
	TopDropArea = 1,
	RightDropArea = 2,
	BottomDropArea = 4,
	LeftDropArea = 8,
	CenterDropArea = 16,

	OuterAreas = TopDropArea | RightDropArea | BottomDropArea | LeftDropArea,
	AllAreas = OuterAreas | CenterDropArea
};
Q_DECLARE_FLAGS(DropAreas, DropArea)

void deleteEmptySplitter(CMainContainerWidget* container);
QSplitter* findParentSplitter(QWidget* w);
QSplitter* findImmediateSplitter(QWidget* w);

} // namespace ads
#endif
