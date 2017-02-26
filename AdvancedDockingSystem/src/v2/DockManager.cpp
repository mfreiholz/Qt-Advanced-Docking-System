
//============================================================================
/// \file   DockManager.cpp
/// \author Uwe Kindler
/// \date   26.02.2017
/// \brief  Implementation of CDockManager class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockManager.h"

#include <QMainWindow>

namespace ads
{
/**
 * Private data class of CDockManager class (pimpl)
 */
struct DockManagerPrivate
{
	CDockManager* _this;

	/**
	 * Private data constructor
	 */
	DockManagerPrivate(CDockManager* _public);
};
// struct DockManagerPrivate

//============================================================================
DockManagerPrivate::DockManagerPrivate(CDockManager* _public) :
	_this(_public)
{

}


//============================================================================
CDockManager::CDockManager(QWidget *parent) :
	CDockContainerWidget(this, parent),
	d(new DockManagerPrivate(this))
{
	QMainWindow* MainWindow = dynamic_cast<QMainWindow*>(parent);
	if (MainWindow)
	{
		MainWindow->setCentralWidget(this);
	}
}

//============================================================================
CDockManager::~CDockManager()
{
	delete d;
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockManager.cpp
