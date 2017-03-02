#ifndef DockAreaWidgetH
#define DockAreaWidgetH
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
/// \file   DockAreaWidget.h
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Declaration of CDockAreaWidget class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>

namespace ads
{
struct DockAreaWidgetPrivate;
class CDockManager;
class CDockContainerWidget;
class CDockWidget;

/**
 * DockAreaWidget manages multiple instances of DckWidgets.
 * It displays a title tab, which is clickable and will switch to
 * the contents associated to the title when clicked.
 */
class CDockAreaWidget : public QFrame
{
	Q_OBJECT
private:
	DockAreaWidgetPrivate* d; ///< private data (pimpl)
	friend class DockAreaWidgetPrivate;

private slots:
	void onDockWidgetTitleClicked();
	void onTabsMenuActionTriggered(QAction* Action);

public:
	/**
	 * Default Constructor
	 */
	CDockAreaWidget(CDockManager* DockManager, CDockContainerWidget* parent);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockAreaWidget();

	/**
	 * Returns the dock manager object this dock area belongs to
	 */
	CDockManager* dockManager() const;

	/**
	 * Returns the dock container widget this dock area widget belongs to or 0
	 * if there is no
	 */
	CDockContainerWidget* dockContainer() const;

	/**
	 * Add a new dock widget to dock area.
	 * All dockwidgets in the dock area tabified in a stacked layout with tabs
	 */
	void addDockWidget(CDockWidget* DockWidget);

	/**
	 * Removes the given dock widget from the dock area
	 */
	void removeDockWidget(CDockWidget* DockWidget);

	/**
	 * Returns the rectangle of the title area
	 */
	QRect titleAreaGeometry() const;

	/**
	 * Returns the rectangle of the content
	 */
	QRect contentAreaGeometry() const;

	/**
	 * Returns the tab index of the given DockWidget
	 */
	int tabIndex(CDockWidget* DockWidget);

	/**
	 * Returns the index of contents of the title widget that is located at
	 * mouse position pos
	 */
	int indexOfContentByTitlePos(const QPoint& pos, QWidget* exclude = nullptr) const;

	/**
	 * Returns a list of all dock widgets in this dock area
	 */
	QList<CDockWidget*> dockWidgets() const;

	/**
	 * Returns the number of dock widgets in this area
	 */
	int count() const;

	/**
	 * Returns a dock widget by its index
	 */
	CDockWidget* dockWidget(int Index) const;

	/**
	 * Reorder the index position of DockWidget at fromIndx to toIndex.
	 */
	void reorderDockWidget(int fromIndex, int toIndex);

public slots:
	/**
	 * This sets the index position of the current tab page.
	 */
	void setCurrentIndex(int index);

	/**
	 * Updates the dock area layout and components visibility
	 */
	void updateDockArea();
}; // class DockAreaWidget
}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // DockAreaWidgetH
