#ifndef DockWidgetH
#define DockWidgetH
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

namespace ads
{
struct DockWidgetPrivate;

/**
 * The QDockWidget class provides a widget that can be docked inside a
 * CDockManager or floated as a top-level window on the desktop.
 */
class CDockWidget : public QFrame
{
	Q_OBJECT
private:
	DockWidgetPrivate* d; ///< private data (pimpl)
	friend class DockWidgetPrivate;
protected:
public:
	/**
	 * Default Constructor
	 */
	CDockWidget(const QString &title, QWidget* parent = 0);

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
}; // class DockWidget
}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // DockWidgetH
