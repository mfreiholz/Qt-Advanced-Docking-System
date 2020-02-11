//============================================================================
/// \file   DockComponentsFactory.cpp
/// \author Uwe Kindler
/// \date   10.02.2020
/// \brief  Implementation of DockComponentsFactory
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <DockComponentsFactory.h>
#include <QScopedPointer>

#include "DockWidgetTab.h"
#include "DockAreaTabBar.h"
#include "DockAreaTitleBar.h"
#include "DockWidget.h"
#include "DockAreaWidget.h"

namespace ads
{
static QScopedPointer<CDockComponentsFactory> DefaultFactory(new CDockComponentsFactory());


//============================================================================
CDockWidgetTab* CDockComponentsFactory::createDockWidgetTab(CDockWidget* DockWidget) const
{
	return new CDockWidgetTab(DockWidget);
}


//============================================================================
CDockAreaTabBar* CDockComponentsFactory::createDockAreaTabBar(CDockAreaWidget* DockArea) const
{
	return new CDockAreaTabBar(DockArea);
}


//============================================================================
CDockAreaTitleBar* CDockComponentsFactory::createDockAreaTitleBar(CDockAreaWidget* DockArea) const
{
	return new CDockAreaTitleBar(DockArea);
}


//============================================================================
const CDockComponentsFactory* CDockComponentsFactory::defaultFactory()
{
	return DefaultFactory.get();
}


//============================================================================
void CDockComponentsFactory::setDefaultFactory(CDockComponentsFactory* Factory)
{
	DefaultFactory.reset(Factory);
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockComponentsFactory.cpp
