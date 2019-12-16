#ifndef FloatingOverlayH
#define FloatingOverlayH
//============================================================================
/// \file   FloatingOverlay.h
/// \author Uwe Kindler
/// \date   26.11.2019
/// \brief  Declaration of CFloatingOverlay
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QWidget>
#include "FloatingDockContainer.h"

namespace ads
{
class CDockWidget;
class CDockAreaWidget;
struct FloatingOverlayPrivate;

/**
 * A floating overlay is a temporary floating widget that is just used to
 * indicate the floating widget movement.
 * This widget is used as a placeholder for drag operations for non-opaque
 * docking
 */
class CFloatingOverlay : public QWidget, public IFloatingWidget
{
	Q_OBJECT
private:
	FloatingOverlayPrivate* d;
	friend class FloatingOverlayPrivate;

protected:
	/**
	 * Updates the drop overlays
	 */
	virtual void moveEvent(QMoveEvent *event) override;

	/**
	 * Cares about painting the
	 */
	virtual void paintEvent(QPaintEvent *e) override;

	/**
	 * The content is a DockArea or a DockWidget
	 */
	CFloatingOverlay(QWidget* Content, QWidget* parent);

public:
	using Super = QWidget;

	/**
	 * Creates an instance for undocking the DockWidget in Content parameter
	 */
	CFloatingOverlay(CDockWidget* Content);

	/**
	 * Creates an instance for undocking the DockArea given in Content
	 * parameters
	 */
	CFloatingOverlay(CDockAreaWidget* Content);

	/**
	 * Delete private data
	 */
	~CFloatingOverlay();


public: // implements IFloatingWidget -----------------------------------------
	virtual void startFloating(const QPoint& DragStartMousePos, const QSize& Size,
        eDragState DragState, QWidget* MouseEventHandler) override;

	/**
	 * Moves the widget to a new position relative to the position given when
	 * startFloating() was called
	 */
	virtual void moveFloating() override;

	/**
	 * Finishes dragging.
	 * Hides the dock overlays and executes the real undocking and docking
	 * of the assigned Content widget
	 */
	virtual void finishDragging() override;
};


} // namespace ads

//---------------------------------------------------------------------------
#endif // FloatingOverlayH

