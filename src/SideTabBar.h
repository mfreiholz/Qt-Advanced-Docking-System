#ifndef SideTabBarH
#define SideTabBarH
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
/// \file   DockWidgetTab.h
/// \author Syarif Fakhri
/// \date   05.09.2022
/// \brief  Declaration of CSideTabBar class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QWidget>
#include "ads_globals.h"

namespace ads
{
struct SideTabBarPrivate;
class CDockContainerWidget;
class CDockWidgetSideTab;
class CDockWidgetTab;

/**
 * Side tab widget that is shown at the edges of a dock container.
 */
class ADS_EXPORT CSideTabBar : public QWidget
{
    Q_OBJECT
private:
    SideTabBarPrivate* d; ///< private data (pimpl)
	friend struct SideTabBarPrivate;
	friend class DockWidgetSideTab;

protected:
    void paintEvent(QPaintEvent* event) override;
	
public:
    using Super = QWidget;

	/**
	 * Default Constructor
	 */
    CSideTabBar(CDockContainerWidget* parent, Qt::Orientation orientation);

	/**
	 * Virtual Destructor
	 */
	virtual ~CSideTabBar();

	/**
	 * Inserts the given dock widget tab at the given position.
	 */
	void insertSideTab(int Index, CDockWidgetSideTab* SideTab);

	/**
	 * Removes the given DockWidgetSideTab from the tabbar
	 */
	void removeSideTab(CDockWidgetSideTab* SideTab);

	Q_SIGNALS:
		void sideTabAutoHideToggleRequested();
};
}

#endif
