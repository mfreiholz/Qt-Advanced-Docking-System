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

// Note: This widget must be a QSplitter, inheriting from QWidget and keeping an
// internal splitter breaks ActiveX widgets
// likely due to layout issues between this widget and the internal splitter
class ADS_EXPORT CAutoHideDockContainer : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(int sideTabBarArea READ sideTabBarArea)
private:
	AutoHideDockContainerPrivate* d; ///< private data (pimpl)
	friend struct AutoHideDockContainerPrivate;

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void updateMask();
	void updateSize();
	virtual bool event(QEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;
	virtual void mousePressEvent(QMouseEvent *event) override;

	CDockContainerWidget* parentContainer() const;

public:
	using Super = QFrame;
	/**
	 * Create Auto Hide widget with a dock manager
	 */
    CAutoHideDockContainer(CDockManager* DockManager, CDockWidgetSideTab::SideTabBarArea area, CDockContainerWidget* parent);

    /**
	 * Create Auto Hide widget with the given dock widget
	 */
	CAutoHideDockContainer(CDockWidget* DockWidget, CDockWidgetSideTab::SideTabBarArea area, CDockContainerWidget* parent);

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

	/*
	 * Set default splitter sizes. Don't use when restoring state
	 * As we want the size from the XML
	 * Takes a float between 0 and 1
	 * E.g. 0.25 is a quarter of the size, 0.5 is half the size and 1 is the entire size of the container
	 */
	void setDockSizeProportion(float SplitterProportion = 0.25);

	/**
	 * Returns the side tab bar area of this Auto Hide dock container
	 */
	CDockWidgetSideTab::SideTabBarArea sideTabBarArea() const;

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
};
}


#endif
