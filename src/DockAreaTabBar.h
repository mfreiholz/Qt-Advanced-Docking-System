#ifndef DockAreaTabBarH
#define DockAreaTabBarH
//============================================================================
/// \file   DockAreaTabBar.h
/// \author Uwe Kindler
/// \date   24.08.2018
/// \brief  Declaration of CDockAreaTabBar class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QScrollArea>

namespace ads
{
class CDockAreaWidget;
class CDockWidgetTab;
struct DockAreaTabBarPrivate;

/**
 * Custom tabbar implementation for tab area that is shown on top of a
 * dock area widget.
 * The tabbar displays the tab widgets of the contained dock widgets.
 */
class CDockAreaTabBar : public QScrollArea
{
	Q_OBJECT
private:
	DockAreaTabBarPrivate* d; ///< private data (pimpl)
	friend class DockAreaTabBarPrivate;

private slots:
	void onTabClicked();
	void onTabWidgetMoved(const QPoint& GlobalPos);

protected:
	virtual void wheelEvent(QWheelEvent* Event) override;
	/**
	 * Stores mouse position to detect dragging
	 */
	virtual void mousePressEvent(QMouseEvent* ev) override;

	/**
	 * Stores mouse position to detect dragging
	 */
	virtual void mouseReleaseEvent(QMouseEvent* ev) override;

	/**
	 * Starts floating the complete docking area including all dock widgets,
	 * if it is not the last dock area in a floating widget
	 */
	virtual void mouseMoveEvent(QMouseEvent* ev) override;

	/**
	 * Double clicking the title bar also starts floating of the complete area
	 */
	virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

	/**
	 * Starts floating
	 */
	void startFloating(const QPoint& Pos);

public:
	using Super = QScrollArea;
	/**
	 * Default Constructor
	 */
	CDockAreaTabBar(CDockAreaWidget* parent);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockAreaTabBar();

	/**
	 * Inserts the given dock widget tab at the given position.
	 * Inserting a new tab at an index less than or equal to the current index
	 * will increment the current index, but keep the current tab.
	 */
	void insertTab(int Index, CDockWidgetTab* Tab);

	/**
	 * Removes the given DockWidgetTab from the tabbar
	 */
	void removeTab(CDockWidgetTab* Tab);

	/**
	 * Returns the number of tabs in this tabbar
	 */
	int count() const;

	/**
	 * Returns the current index
	 */
	int currentIndex() const;

	/**
	 * Returns the current tab
	 */
	CDockWidgetTab* currentTab() const;

	/**
	 * Returns the tab with the given index
	 */
	CDockWidgetTab* tab(int Index) const;

	/**
	 * Filters the tab widget events
	 */
	virtual bool eventFilter(QObject *watched, QEvent *event) override;

	/**
	 * This function returns true if the tab is open, that means if it is
	 * visible to the user. If the function returns false, the tab is
	 * closed
	 */
	bool isTabOpen(int Index) const;

public slots:
	/**
	 * This property sets the index of the tab bar's visible tab
	 */
	void setCurrentIndex(int Index);

	/**
	 * This function will close the tab given in Index param.
	 * Closing a tab means, the tab will be hidden, it will not be removed
	 */
	void closeTab(int Index);

signals:
    /**
     * This signal is emitted when the tab bar's current tab is about to be changed. The new
     * current has the given index, or -1 if there isn't a new one.
     */
	void currentChanging(int Index);

	/**
	 * This signal is emitted when the tab bar's current tab changes. The new
	 * current has the given index, or -1 if there isn't a new one
	 */
	void currentChanged(int Index);

	/**
	 * This signal is emitted when user clicks on a tab at an index.
	 */
	void tabBarClicked(int index);

	/**
	 * This signal is emitted when the close button on a tab is clicked.
	 * The index is the index that should be closed.
	 */
	void tabCloseRequested(int index);

	/**
	 * This signal is emitted if a tab has been closed
	 */
	void tabClosed(int index);

	/**
	 * This signal is emitted if a tab has been opened.
	 * A tab is opened if it has been made visible
	 */
	void tabOpened(int index);

	/**
	 * This signal is emitted when the tab has moved the tab at index position
	 * from to index position to.
	 */
	void tabMoved(int from, int to);

	/**
	 * This signal is emitted, just before the tab with the given index is
	 * removed
	 */
	void removingTab(int index);

	/**
	 * This signal is emitted if a tab has been inserted
	 */
	void tabInserted(int index);
}; // class CDockAreaTabBar
} // namespace ads
//-----------------------------------------------------------------------------
#endif // DockAreaTabBarH

