#ifndef DockContainerWidgetH
#define DockContainerWidgetH
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
/// \file   DockContainerWidget.h
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Declaration of CDockContainerWidget class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>

#include "ads_globals.h"

namespace ads
{
struct DockContainerWidgetPrivate;
class CDockAreaWidget;
class CDockWidget;
class CDockManager;
class CFloatingDockContainer;

/**
 * Container that manages a number of dock areas with single dock widgets
 * or tabyfied dock widgets in each area
 */
class CDockContainerWidget : public QFrame
{
	Q_OBJECT
private:
	DockContainerWidgetPrivate* d; ///< private data (pimpl)
	friend class DockContainerWidgetPrivate;
protected:
	/**
	 * Handles activation events to update zOrderIndex
	 */
	virtual bool event(QEvent *e) override;

public:
	/**
	 * Default Constructor
	 */
	CDockContainerWidget(CDockManager* DockManager, QWidget* parent = 0);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockContainerWidget();

	/**
	 * Drop floating widget into the container
	 */
	void dropFloatingWidget(CFloatingDockContainer* FloatingWidget, const QPoint& TargetPos);

	/**
	 * Adds dockwidget into the given area.
	 * If DockAreaWidget is not null, then the area parameter indicates the area
	 * into the DockAreaWidget. If DockAreaWidget is null, the Dockwidget will
	 * be dropped into the container.
	 * \return Returns the dock area widget that contains the new DockWidget
	 */
	CDockAreaWidget* addDockWidget(DockWidgetArea area, CDockWidget* Dockwidget,
		CDockAreaWidget* DockAreaWidget = nullptr);

	/**
	 * Adds the given dock area to this container widget
	 */
	void addDockArea(CDockAreaWidget* DockAreaWidget, DockWidgetArea area = CenterDockWidgetArea);

	/**
	 * Removes the given dock area from this container
	 */
	void removeDockArea(CDockAreaWidget* area);

	/**
	 * Returns the current zOrderIndex
	 */
	virtual unsigned int zOrderIndex() const;

	/**
	 * This function returns true if this container widgets z order index is
	 * higher than the index of the container widget given in Other parameter
	 */
	bool isInFrontOf(CDockContainerWidget* Other) const;

	/**
	 * Returns the dock area at teh given global position or 0 if there is no
	 * dock area at this position
	 */
	CDockAreaWidget* dockAreaAt(const QPoint& GlobalPos) const;

	/**
	 * Returns the dock area at the given Index or 0 if the index is out of
	 * range
	 */
	CDockAreaWidget* dockArea(int Index) const;

	/**
	 * Returns the number of dock areas in this container
	 */
	int dockAreaCount() const;

	/**
	 * This function returns true, if this container is in a floating widget
	 */
	bool isFloating() const;

signals:
	/**
	 * This signal is emitted if one or multiple dock areas has been added to
	 * the internal list of dock areas.
	 * If multiple dock areas are inserted, this signal is emitted only once
	 */
	void dockAreasAdded();

	/**
	 * This signal is emitted if one or multiple dock areas has been removed
	 */
	void dockAreasRemoved();
}; // class DockContainerWidget
} // namespace ads
//-----------------------------------------------------------------------------
#endif // DockContainerWidgetH
