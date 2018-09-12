#ifndef DockWidgetH
#define DockWidgetH
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
/// \file   DockWidget.h
/// \author Uwe Kindler
/// \date   26.02.2017
/// \brief  Declaration of CDockWidget class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>

#include "ads_globals.h"

class QToolBar;
class QXmlStreamWriter;

namespace ads
{
struct DockWidgetPrivate;
class CDockWidgetTab;
class CDockManager;
class CDockContainerWidget;
class CDockAreaWidget;
struct DockContainerWidgetPrivate;

/**
 * The QDockWidget class provides a widget that can be docked inside a
 * CDockManager or floated as a top-level window on the desktop.
 */
class ADS_EXPORT CDockWidget : public QFrame
{
	Q_OBJECT
private:
	DockWidgetPrivate* d; ///< private data (pimpl)
	friend struct DockWidgetPrivate;

private slots:
	/**
	 * Adjusts the toolbar icon sizes according to the floating state
	 */
	void setToolbarFloatingStyle(bool topLevel);

protected:
	friend class CDockContainerWidget;
	friend class CDockAreaWidget;
	friend class CFloatingDockContainer;
	friend class CDockManager;
	friend struct DockContainerWidgetPrivate;

	/**
	 * Assigns the dock manager that manages this dock widget
	 */
	void setDockManager(CDockManager* DockManager);

	/**
	 * If this dock widget is inserted into a dock area, the dock area will
	 * be registered on this widget via this function. If a dock widget is
	 * removed from a dock area, this function will be called with nullptr
	 * value.
	 */
	void setDockArea(CDockAreaWidget* DockArea);

	/**
	 * This function changes the toggle view action without emitting any
	 * signal
	 */
	void setToggleViewActionChecked(bool Checked);

	/**
	 * Saves the state into the given stream
	 */
	void saveState(QXmlStreamWriter& Stream) const;

	/**
	 * This is a helper function for the dock manager to flag this widget
	 * as unassigned.
	 * When calling the restore function, it may happen, that the saved state
	 * contains less dock widgets then currently available. All widgets whose
	 * data is not contained in the saved state, are flagged as unassigned
	 * after the restore process. If the user shows an unassigned dock widget,
	 * a floating widget will be created to take up the dock widget.
	 */
	void flagAsUnassigned();

public:
	enum DockWidgetFeature
	{
		DockWidgetClosable = 0x01,
		DockWidgetMovable = 0x02,
		DockWidgetFloatable = 0x04,
		AllDockWidgetFeatures = DockWidgetClosable | DockWidgetMovable | DockWidgetFloatable,
		NoDockWidgetFeatures = 0x00
	};
	Q_DECLARE_FLAGS(DockWidgetFeatures, DockWidgetFeature)

	enum eState
	{
		StateHidden,
		StateDocked,
		StateFloating
	};

	/**
	 * Use the layout flags to configure the layout of the dock widget.
	 * The content of a dock widget should be resizable do a very small size to
	 * prevent the dock widget from blocking the resizing. To ensure, that a
	 * dock widget can be resized very well, it is better to insert the content+
	 * widget into a scroll area. Enable the WithScrollArea
	 * feature to use this feature. If your content widget is already in a
	 * scroll area or if it is a derived class like QTableView, the you should
	 * disable the WithScrollArea flag.
	 * Often dock widgets need a ToolBar for control of operations in the dock
	 * widget. Use the WithToolBar feature to enable a tool bar that is placed
	 * on top of the dock widget content. If this flag is disabled, the toolBar()
	 * function returns a nullptr.
	 */
	enum LayoutFlag
	{
		WithScrollArea = 0x01,
		WithTopToolBar = 0x02
	};
	Q_DECLARE_FLAGS(LayoutFlags, LayoutFlag)

	/**
	 * This mode configures the behavior of the toggle view action.
	 * If the mode if ActionModeToggle, then the toggle view action is
	 * a checkable action to show / hide the dock widget. If the mode
	 * is ActionModeShow, then the action is not checkable an it will
	 * always show the dock widget if clicked. If the mode is ActionModeShow,
	 * the user can only close the DockWidget with the close button.
	 */
	enum eToggleViewActionMode
	{
		ActionModeToggle,//!< ActionModeToggle
		ActionModeShow   //!< ActionModeShow
	};


	/**
	 * This constructor creates a dock widget with the given title.
	 * The title is the text that is shown in the window title when the dock
	 * widget is floating and it is the title that is shown in the titlebar
	 * or the tab of this dock widget if it is tabified.
	 * The object name of the dock widget is also set to the title. The
	 * object name is required by the dock manager to properly save and restore
	 * the state of the dock widget. That means, the title needs to be unique.
	 * If your title is not unique or if you would like to change the title
	 * during runtime, you need to set a unique object name explicitely
	 * by calling setObjectName() after construction.
	 * Use the layoutFlags to configure the layout of the dock widget.
	 */
	CDockWidget(const QString &title, QWidget* parent = 0,
		LayoutFlags layoutFlags = 0);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockWidget();

	/**
	 * Sets the widget for the dock widget to widget.
	 */
	void setWidget(QWidget* widget);

	/**
	 * Returns the widget for the dock widget. This function returns zero if
	 * the widget has not been set.
	 */
	QWidget* widget() const;

	/**
	 * Returns the title bar widget of this dock widget
	 */
	CDockWidgetTab* tabWidget() const;

	/**
	 * Sets, whether the dock widget is movable, closable, and floatable.
	 */
	void setFeatures(DockWidgetFeatures features);

	/**
	 * Sets the feature flag for this dock widget if on is true; otherwise
	 * clears the flag.
	 */
	void setFeature(DockWidgetFeature flag, bool on);

	/**
	 * This property holds whether the dock widget is movable, closable, and
	 * floatable.
	 * By default, this property is set to a combination of DockWidgetClosable,
	 * DockWidgetMovable and DockWidgetFloatable.
	 */
	DockWidgetFeatures features() const;

	/**
	 * Returns the dock manager that manages the dock widget or 0 if the widget
	 * has not been assigned to any dock manager yet
	 */
	CDockManager* dockManager() const;

	/**
	 * Returns the dock container widget this dock area widget belongs to or 0
	 * if this dock widget has not been docked yet
	 */
	CDockContainerWidget* dockContainer() const;

	/**
	 * Returns the dock area widget this dock widget belongs to or 0
	 * if this dock widget has not been docked yet
	 */
	CDockAreaWidget* dockAreaWidget() const;

	/**
	 * This property holds whether the dock widget is floating.
	 * A dock widget is only floating, if it is the one and only widget inside
	 * of a floating container. If there are more than one dock widget in a
	 * floating container, the all dock widgets are docked and not floating.
	 */
	bool isFloating() const;

	/**
	 * This function returns true, if this dock widget is in a floating.
	 * The function returns true, if the dock widget is floating and it also
	 * returns true if it is docked inside of a floating container.
	 */
	bool isInFloatingContainer() const;

	/**
	 * Returns true, if this dock widget is closed.
	 */
	bool isClosed() const;

	/**
	 * Returns a checkable action that can be used to show or close this dock widget.
	 * The action's text is set to the dock widget's window title.
	 */
	QAction* toggleViewAction() const;

	/**
	 * Configures the behavior of the toggle view action.
	 * \see eToggleViewActionMode for a detailed description
	 */
	void setToggleViewActionMode(eToggleViewActionMode Mode);

	/**
	 * Sets the dock widget icon that is shown in tabs and in toggle view
	 * actions
	 */
	void setIcon(const QIcon& Icon);

	/**
	 * Returns tzhe icon that has been assigned to the dock widget
	 */
	QIcon icon() const;

	/**
	 * If the WithToolBar layout flag is enabled, then this function returns
	 * the dock widget toolbar. If the flag is disabled, the function returns
	 * a nullptr.
	 */
	QToolBar* toolBar() const;

	/**
	 * Assign a new tool bar that is shown above the content widget.
	 * The dock widget will become the owner of the tool bar and deletes it
	 * on destruction
	 */
	void setToolBar(QToolBar* ToolBar);

	/**
	 * This function sets the tool button style for the given dock widget state.
	 * It is possible to switch the tool button style depending on the state.
	 * If a dock widget is floating, then here are more space and it is
	 * possible to select a style that requires more space like
	 * Qt::ToolButtonTextUnderIcon. For the docked state Qt::ToolButtonIconOnly
	 * might be better.
	 */
	void setToolBarStyle(Qt::ToolButtonStyle Style, eState State);

	/**
	 * Returns the tool button style for the given docking state.
	 * \see setToolBarStyle()
	 */
	Qt::ToolButtonStyle toolBarStyle(eState State) const;

	/**
	 * This function sets the tool button icon size for the given state.
	 * If a dock widget is floating, there is more space an increasing the
	 * icon size is possible. For docked widgets, small icon sizes, eg. 16 x 16
	 * might be better.
	 */
	void setToolBarIconSize(const QSize& IconSize, eState State);

	/**
	 * Returns the icon size for a given docking state.
	 * \see setToolBarIconSize()
	 */
	QSize toolBarIconSize(eState State) const;


public: // reimplements QFrame -----------------------------------------------
	/**
	 * Emits titleChanged signal if title change event occurs
	 */
	virtual bool event(QEvent *e) override;

public slots:
	/**
	 * This property controls whether the dock widget is open or closed.
	 * The toogleViewAction triggers this slot
	 */
	void toggleView(bool Open = true);

signals:
	/**
	 * This signal is emitted if the dock widget is opened or closed
	 */
	void viewToggled(bool Open);

	/**
	 * This signal is emitted if the dock widget is closed
	 */
	void closed();

	/**
	 * This signal is emitted if the window title of this dock widget
	 * changed
	 */
	void titleChanged(const QString& Title);

	/**
	 * This signal is emitted when the floating property changes.
	 * The topLevel parameter is true if the dock widget is now floating;
	 * otherwise it is false.
	 */
	void topLevelChanged(bool topLevel);
}; // class DockWidget
}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // DockWidgetH
