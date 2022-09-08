#ifndef OverlayDockContainerH
#define OverlayDockContainerH
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
/// \brief  Declaration of COverlayDockContainer class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "ads_globals.h"

#include "DockWidgetSideTab.h"

#include <QFrame>

class QXmlStreamWriter;

namespace ads
{
struct OverlayDockContainerPrivate;
class CDockManager;
class CDockWidget;
class CDockContainerWidget;
class CSideTabBar;
class CDockAreaWidget;
class CDockingStateReader;

class ADS_EXPORT COverlayDockContainer : public QFrame
{
    Q_OBJECT

private:
    OverlayDockContainerPrivate* d; ///< private data (pimpl)
    friend struct OverlayDockContainerPrivate;

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
    void updateMask();
	void updateSize();

	CDockContainerWidget* parentContainer() const;

public:
	/**
	 * Create overlay widget with a dock manager
	 */
    COverlayDockContainer(CDockManager* DockManager, CDockWidgetSideTab::SideTabBarArea area, CDockContainerWidget* parent);

    /**
	 * Create overlay widget with the given dock widget
	 */
	COverlayDockContainer(CDockWidget* DockWidget, CDockWidgetSideTab::SideTabBarArea area, CDockContainerWidget* parent);

	/**
	 * Virtual Destructor
	 */
	virtual ~COverlayDockContainer();

	/**
	 * Get's the side tab bar
	 */
	CSideTabBar* sideTabBar() const;

	/**
	 * Get's the dock widget in this dock container
	 */
	CDockWidget* dockWidget() const;

	/**
	 * Adds a dock widget and removes the previous dock widget
	 */
	void addDockWidget(CDockWidget* DockWidget);

	/**
	 * Returns the side tab bar area of this overlay dock container
	 */
	CDockWidgetSideTab::SideTabBarArea sideTabBarArea() const;

	/**
	 * Returns the dock area widget of this overlay dock container
	 */
	CDockAreaWidget* dockAreaWidget() const;

	/**
	 * Moves the contents to the parent container widget
	 * Used before removing this overlay dock container 
	 */
    void moveContentsToParent();

	/**
	 * Cleanups up the side tab widget and then deletes itself
	 */
	void cleanupAndDelete();

	/*
	 * Saves the state and size
	 */
	void saveState(QXmlStreamWriter& Stream);

	/*
	 * Restores the size of the splitter
	 */
	bool restoreState(CDockingStateReader& Stream, bool Testing);

	/*
	 * Convenience function fr determining if area exists in config
	 */
	static bool areaExistsInConfig(CDockWidgetSideTab::SideTabBarArea area);
};
}


#endif
