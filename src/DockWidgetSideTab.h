#ifndef DockWidgetSideTabH
#define DockWidgetSideTabH
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
/// \brief  Declaration of CDockWidgetSideTab class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>

#include "ads_globals.h"

namespace ads
{
struct DockWidgetSideTabPrivate;
class CDockWidget;
class CSideTabBar;
class CDockWidgetTab;

/**
 * A dock widget Side tab that shows a title or an icon.
 * The dock widget tab is shown in the side tab bar to switch between
 * pinned dock widgets
 */
class ADS_EXPORT CDockWidgetSideTab : public QFrame
{
    Q_OBJECT

private:    
	DockWidgetSideTabPrivate* d; ///< private data (pimpl)
    friend struct DockWidgetSideTabPrivate;
	friend class CDockWidget;
	friend class COverlayDockContainer;

protected:
	friend class CSideTabBar;
	friend class CDockAreaWidget;
	friend class CDockContainerWidget;

	void mousePressEvent(QMouseEvent* event) override;

	void setSideTabBar(CSideTabBar *SideTabBar);
	void removeFromSideTabBar();

public:
    using Super = QFrame;
	/**
	 * Default Constructor
	 * param[in] DockWidget The dock widget this title bar belongs to
	 * param[in] Orientation Horizontal or vertical orientation
	 * param[in] parent The parent widget of this title bar
	 */
	CDockWidgetSideTab(CDockWidget* DockWidget, QWidget* parent = nullptr);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockWidgetSideTab();

	/**
	 * Update stylesheet style if a property changes
	 */
	void updateStyle();

Q_SIGNALS:
	void elidedChanged(bool elided);
	void clicked();
}; // class DockWidgetSideTab
}
 // namespace ads
//-----------------------------------------------------------------------------

#endif
