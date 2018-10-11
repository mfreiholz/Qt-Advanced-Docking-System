#ifndef DockAreaWidgetH
#define DockAreaWidgetH
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
/// \file   DockAreaWidget.h
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Declaration of CDockAreaWidget class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>

#include "ads_globals.h"

class QXmlStreamWriter;

namespace ads
{
struct DockAreaWidgetPrivate;
class CDockManager;
class CDockContainerWidget;
struct DockContainerWidgetPrivate;
class CDockWidget;


/**
 * DockAreaWidget manages multiple instances of DockWidgets.
 * It displays a title tab, which is clickable and will switch to
 * the contents associated to the title when clicked.
 */
class ADS_EXPORT CDockAreaWidget : public QFrame
{
	Q_OBJECT
private:
	DockAreaWidgetPrivate* d; ///< private data (pimpl)
	friend struct DockAreaWidgetPrivate;
	friend class CDockContainerWidget;
	friend class DockContainerWidgetPrivate;
	friend class CDockWidgetTab;
	friend struct DockWidgetPrivate;
	friend class CDockWidget;

private slots:
	void onTabsMenuActionTriggered(QAction* Action);
	void onCloseButtonClicked();
	void onTabsMenuAboutToShow();

protected:
	/**
	 * Inserts a dock widget into dock area.
	 * All dockwidgets in the dock area tabified in a stacked layout with tabs.
	 * The index indicates the index of the new dockwidget in the tabbar and
	 * in the stacked layout. If the Activate parameter is true, the new
	 * DockWidget will be the active one in the stacked layout
	 */
	void insertDockWidget(int index, CDockWidget* DockWidget, bool Activate = true);

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
	 * Returns the index of contents of the title widget that is located at
	 * mouse position pos
	 */
	int indexOfContentByTitlePos(const QPoint& pos, QWidget* exclude = nullptr) const;

	/**
	 * Reorder the index position of DockWidget at fromIndx to toIndex.
	 */
	void reorderDockWidget(int fromIndex, int toIndex);

	/**
	 * Called from dock widget if it is opened or closed
	 */
	void toggleDockWidgetView(CDockWidget* DockWidget, bool Open);

	/**
	 * This is a helper function to get the next open dock widget to activate
	 * if the given DockWidget will be closed or removed.
	 * The function returns the next widget that should be activated or
	 * nullptr in case there are no more open widgets in this area.
	 */
	CDockWidget* nextOpenDockWidget(CDockWidget* DockWidget) const;

	/**
	 * Returns the index of the given DockWidget in the internal layout
	 */
	int index(CDockWidget* DockWidget);

	/**
	 * Call this function, if you already know, that the dock does not
	 * contain any visible content (any open dock widgets).
	 */
	void hideAreaWithNoVisibleContent();

	/**
	 * This function checks, if the dock area has visible content, that means
	 * if any dock widget is open, and then calls hideAreaWithNoVisibleContent()
	 * if it does not find any visible content
	 */
	void hideAreaIfNoVisibleContent();

	/**
	 * Updates the dock area layout and components visibility
	 */
	void updateTabBarVisibility();

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
	 * Returns the rectangle of the title area
	 */
	QRect titleAreaGeometry() const;

	/**
	 * Returns the rectangle of the content
	 */
	QRect contentAreaGeometry() const;

	/**
	 * Returns the number of dock widgets in this area
	 */
	int dockWidgetsCount() const;

	/**
	 * Returns a list of all dock widgets in this dock area.
	 * This list contains open and closed dock widgets.
	 */
	QList<CDockWidget*> dockWidgets() const;

	/**
	 * Returns the number of dock widgets in this area
	 */
	int openDockWidgetsCount() const;

	/**
	 * Returns a list of dock widgets that are not closed
	 */
	QList<CDockWidget*> openedDockWidgets() const;

	/**
	 * Returns a dock widget by its index
	 */
	CDockWidget* dockWidget(int Index) const;

	/**
	 * Returns the index of the current active dock widget
	 */
	int currentIndex() const;

	/**
	 * Returns the current active dock widget
	 */
	CDockWidget* currentDockWidget() const;

	/**
	 * Shows the tab with the given dock widget
	 */
	void setCurrentDockWidget(CDockWidget* DockWidget);

	/**
	 * Saves the state into the given stream
	 */
	void saveState(QXmlStreamWriter& Stream) const;

public slots:
	/**
	 * This activates the tab for the given tab index.
	 * If the dock widget for the given tab is not visible, the this function
	 * call will make it visible.
	 */
	void setCurrentIndex(int index);

signals:
	/**
	 * This signal is emitted when user clicks on a tab at an index.
	 */
	void tabBarClicked(int index);

    /**
     * This signal is emitted when the tab bar's current tab is about to be changed. The new
     * current has the given index, or -1 if there isn't a new one.
     * @param index
     */
    void currentChanging(int index);

	/**
	 * This signal is emitted when the tab bar's current tab changes. The new
	 * current has the given index, or -1 if there isn't a new one
	 * @param index
	 */
	void currentChanged(int index);
}; // class DockAreaWidget
}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // DockAreaWidgetH