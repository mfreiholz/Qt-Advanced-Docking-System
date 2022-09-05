//============================================================================
/// \file   DockComponentsFactory.cpp
/// \author Uwe Kindler
/// \date   10.02.2020
/// \brief  Implementation of DockComponentsFactory
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockComponentsFactory.h"

#include <memory>

#include "DockWidgetTab.h"
#include "DockAreaTabBar.h"
#include "DockAreaTitleBar.h"
#include "DockWidget.h"
#include "DockAreaWidget.h"
#include "DockWidgetSideTab.h"

namespace ads
{
static std::unique_ptr<CDockComponentsFactory> DefaultFactory(new CDockComponentsFactory());


//============================================================================
CDockWidgetTab* CDockComponentsFactory::createDockWidgetTab(CDockWidget* DockWidget) const
{
	return new CDockWidgetTab(DockWidget);
}

//============================================================================
CDockWidgetSideTab* CDockComponentsFactory::createDockWidgetSideTab(CDockWidget *DockWidget) const
{
	return new CDockWidgetSideTab(DockWidget);
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
const CDockComponentsFactory* CDockComponentsFactory::factory()
{
	return DefaultFactory.get();
}


//============================================================================
void CDockComponentsFactory::setFactory(CDockComponentsFactory* Factory)
{
	DefaultFactory.reset(Factory);
}


//============================================================================
void CDockComponentsFactory::resetDefaultFactory()
{
	DefaultFactory.reset(new CDockComponentsFactory());
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockComponentsFactory.cpp
