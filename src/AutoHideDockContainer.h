#ifndef AutoHideDockContainerH
#define AutoHideDockContainerH
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
/// \file   AutoHideDockContainer.h
/// \author Syarif Fakhri
/// \date   05.09.2022
/// \brief  Declaration of CAutoHideDockContainer class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "ads_globals.h"

#include "DockWidgetSideTab.h"

#include <QSplitter>

class QXmlStreamWriter;

namespace ads
{
struct AutoHideDockContainerPrivate;
class CDockManager;
class CDockWidget;
class CDockContainerWidget;
class CSideTabBar;
class CDockAreaWidget;
class CDockingStateReader;

/**
 * Auto hide container for hosting an auto hide dock widget
 */
class ADS_EXPORT CAutoHideDockContainer : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(ads::SideBarLocation sideTabBarArea READ sideTabBarArea)
private:
	AutoHideDockContainerPrivate* d; ///< private data (pimpl)
	friend struct AutoHideDockContainerPrivate;

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void updateSize();

	CDockContainerWidget* parentContainer() const;

public:
	using Super = QFrame;
	/**
	 * Create Auto Hide widget with a dock manager
	 */
    CAutoHideDockContainer(CDockManager* DockManager, SideBarLocation area, CDockContainerWidget* parent);

    /**
	 * Create Auto Hide widget with the given dock widget
	 */
	CAutoHideDockContainer(CDockWidget* DockWidget, SideBarLocation area, CDockContainerWidget* parent);

	/**
	 * Virtual Destructor
	 */
	virtual ~CAutoHideDockContainer();

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
	 * Returns the side tab bar area of this Auto Hide dock container
	 */
	SideBarLocation sideTabBarArea() const;

	/**
	 * Returns the dock area widget of this Auto Hide dock container
	 */
	CDockAreaWidget* dockAreaWidget() const;

	/**
	 * Moves the contents to the parent container widget
	 * Used before removing this Auto Hide dock container 
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
	 * Toggles the auto Hide dock container widget
	 * This will also hide the side tab widget
	 */
	void toggleView(bool Enable);

	/*
	 * Collapses the auto hide dock container widget
	 * Does not hide the side tab widget
	 */
	void collapseView(bool Enable);

	/**
	 * Toggles the current collapse state
	 */
	void toggleCollapseState();

	/**
	 * Use this instead of resize. This will ensure the size is consistent internally.
	 * E.g. If you set a height less than the parent height when it's vertical
	 * It will simply be rescaled to the parent height while the width will be resized
	 */
	void setSize(int width, int height);
};
}


#endif
