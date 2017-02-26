#ifndef DockManagerH
#define DockManagerH
//============================================================================
/// \file   DockManager.h
/// \author Uwe Kindler
/// \date   26.02.2017
/// \brief  Declaration of CDockManager class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockContainerWidget.h"

namespace ads
{
struct DockManagerPrivate;

/**
 * The central dock manager that maintains the complete docking system
 **/
class CDockManager : public CDockContainerWidget
{
	Q_OBJECT
private:
	DockManagerPrivate* d; ///< private data (pimpl)
	friend class DockManagerPrivate;
protected:
public:
	/**
	 * Default Constructor.
	 * If the given parent is a QMainWindow, the dck manager sets itself as the
	 * central widget
	 */
	CDockManager(QWidget* parent = 0);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockManager();
}; // class DockManager
} // namespace ads
//-----------------------------------------------------------------------------
#endif // DockManagerH
