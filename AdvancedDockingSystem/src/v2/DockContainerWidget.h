#ifndef DockContainerWidgetH
#define DockContainerWidgetH
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

/**
 * Container that manages a number of dock areas with single dock widgets
 * or tabyfied dock widtes in each area
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
	 * Adds dockwidget into the given area.
	 * If DockAreaWidget is not null, then the area parameter indicates the area
	 * into the DockAreaWidget. If DockAreaWidget is null, the Dockwidget will
	 * be dropped into the container.
	 */
	void addDockWidget(DockWidgetArea area, CDockWidget* Dockwidget, CDockAreaWidget* DockAreaWidget = nullptr);

	/**
	 * Returns the current zOrderIndex
	 */
	unsigned int zOrderIndex() const;

	/**
	 * This function returns true if this container widgets z order index is
	 * higher than the index of the container widget given in Other parameter
	 */
	bool isInFrontOf(CDockContainerWidget* Other) const;
}; // class DockContainerWidget
} // namespace ads
//-----------------------------------------------------------------------------
#endif // DockContainerWidgetH
