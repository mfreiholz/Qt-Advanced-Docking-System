#ifndef DockAreaWidgetH
#define DockAreaWidgetH
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
protected:
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
	 * Returns the dock container widget this dock area widget belongs to or 0
	 * if there is no
	 */
	CDockContainerWidget* dockContainerWidget() const;
}; // class DockAreaWidget
}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // DockAreaWidgetH
