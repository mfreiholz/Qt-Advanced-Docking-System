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
struct DockAreaTabBarPrivate;

/**
 * Custom scroll bar implementation for dock area tab bar
 * This scroll area enables floating of a whole dock area including all
 * dock widgets
 */
class CDockAreaTabBar : public QScrollArea
{
	Q_OBJECT
private:
	DockAreaTabBarPrivate* d; ///< private data (pimpl)
	friend class DockAreaTabBarPrivate;

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
	/**
	 * Default Constructor
	 */
	CDockAreaTabBar(CDockAreaWidget* parent);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockAreaTabBar();
}; // class CDockAreaTabBar
} // namespace ads
//-----------------------------------------------------------------------------
#endif // DockAreaTabBarH

