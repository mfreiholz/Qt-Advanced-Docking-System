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
struct SideTabIconLabelPrivate;

/**
 * A dock widget Side tab that shows a title or an icon.
 * The dock widget tab is shown in the side tab bar to switch between
 * pinned dock widgets
 */
class ADS_EXPORT CDockWidgetSideTab : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(SideTabBarArea sideTabBarArea READ sideTabBarArea)
	Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)
	Q_PROPERTY(bool activeTab READ isActiveTab)

private:    
	DockWidgetSideTabPrivate* d; ///< private data (pimpl)
    friend struct DockWidgetSideTabPrivate;
	friend class CDockWidget;
	friend class CAutoHideDockContainer;

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
     * Dock widget side tab bar locations
     */
    enum SideTabBarArea
    {
		Top,
        LeftTop,
        LeftBottom,
        RightTop,
        RightBottom,
		Bottom
    };

	Q_ENUM(SideTabBarArea)

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

	/**
	 * Getter for side tab bar area property
	 */
	SideTabBarArea sideTabBarArea() const;

	/**
	 * Sets the icon to show in title bar
	 */
	void setIcon(const QIcon& Icon);

	/**
	 * Returns the icon size.
	 * If no explicit icon size has been set, the function returns an invalid
	 * QSize
	 */
	QSize iconSize() const;

	/**
	 * Set an explicit icon size.
	 * If no icon size has been set explicitly, than the tab sets the icon size
	 * depending on the style
	 */
	void setIconSize(const QSize& Size);

	/**
	 * Set orientation vertical or horizontal
	 */
	void setOrientation(Qt::Orientation Orientation);

	/**
	 * Update the orientation, visibility and spacing based on the area and the config
	 */
	void updateOrientationAndSpacing(SideTabBarArea area);

	/**
	 * Returns true, if this is the active tab. The tab is active if the auto hide widget is visible
	 */
	bool isActiveTab() const;

	/**
	 * returns the dock widget this belongs to
	 */
	CDockWidget* dockWidget() const;

Q_SIGNALS:
	void elidedChanged(bool elided);
	void clicked();
}; // class DockWidgetSideTab

class SideTabIconLabel : public QWidget
{
private:
	SideTabIconLabelPrivate *d; ///< private data (pimpl)

public:
	SideTabIconLabel(QWidget* parent = nullptr);
	virtual ~SideTabIconLabel();

	void setPixmap(const QPixmap &pixmap);
	void setContentsMargins(int left, int top, int right, int bottom);
}; // class SideTabIconLabel
}
 // namespace ads
//-----------------------------------------------------------------------------

#endif
