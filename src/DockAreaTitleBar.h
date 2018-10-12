#ifndef DockAreaTitleBarH
#define DockAreaTitleBarH
//============================================================================
/// \file   DockAreaTitleBar.h
/// \author Uwe Kindler
/// \date   12.10.2018
/// \brief  Declaration of CDockAreaTitleBar class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>

namespace ads
{
class CDockAreaTabBar;
class CDockAreaWidget;
struct DockAreaTitleBarPrivate;

/**
 * Title bar of a dock area
 */
class CDockAreaTitleBar : public QFrame
{
	Q_OBJECT
private:
	DockAreaTitleBarPrivate* d; ///< private data (pimpl)
	friend class DockAreaTitleBarPrivate;

private slots:
	void markTabsMenuOutdated();
	void onTabsMenuAboutToShow();
	void onCloseButtonClicked();
	void onTabsMenuActionTriggered(QAction* Action);

public:
	using Super = QFrame;
	/**
	 * Default Constructor
	 */
	CDockAreaTitleBar(CDockAreaWidget* parent);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockAreaTitleBar();

	/**
	 * Returns the pointer to the tabBar()
	 */
	CDockAreaTabBar* tabBar() const;
}; // class name
}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // DockAreaTitleBarH
