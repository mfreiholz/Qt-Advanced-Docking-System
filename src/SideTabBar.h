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
#include <QFrame>
#include "DockWidgetSideTab.h"
#include "ads_globals.h"

namespace ads
{
struct SideTabBarPrivate;
class CDockContainerWidget;
class CDockWidgetSideTab;

/**
 * Side tab widget that is shown at the edges of a dock container.
 */
class ADS_EXPORT CSideTabBar : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(int sideTabBarArea READ sideTabBarArea)
    Q_PROPERTY(Qt::Orientation orientation READ orientation)

private:
    SideTabBarPrivate* d; ///< private data (pimpl)
	friend struct SideTabBarPrivate;
	friend class DockWidgetSideTab;

protected:
    void paintEvent(QPaintEvent* event) override;

public:
    using Super = QFrame;

	/**
	 * Default Constructor
	 */
    CSideTabBar(CDockContainerWidget* parent, CDockWidgetSideTab::SideTabBarArea area);

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

	/**
	 * Returns orientation of side tab.
	 */
	Qt::Orientation orientation() const;

	/*
	 * get the side tab widget at position, returns nullptr if it's out of bounds
	 */
	CDockWidgetSideTab* tabAt(int index) const;

	/*
	 * Gets the count of the tab widgets
	 */
	int tabCount() const;

	/**
	 * Getter for side tab bar area property
	 */
	CDockWidgetSideTab::SideTabBarArea sideTabBarArea() const;

Q_SIGNALS:
	void sideTabAutoHideToggleRequested();
};
}

#endif
