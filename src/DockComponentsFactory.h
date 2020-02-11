#ifndef DockComponentsFactoryH
#define DockComponentsFactoryH
//============================================================================
/// \file   DockComponentsFactory.h
/// \author Uwe Kindler
/// \date   10.02.2020
/// \brief  Declaration of DockComponentsFactory
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
namespace ads
{
class CDockWidgetTab;
class CDockAreaTitleBar;
class CDockAreaTabBar;
class CDockAreaWidget;
class CDockWidget;



/**
 * Factory for creation of certain GUI elements for the docking framework.
 * A default unique instance provided by CDockComponentsFactory is used for
 * creation of all supported components. To inject your custom components,
 * you can create your own derived dock components factory and register
 * it via setDefaultFactory() function.
 * \code
 * CDockComponentsFactory::setDefaultFactory(new MyComponentsFactory()));
 * \endcode
 */
class CDockComponentsFactory
{
public:
	/**
	 * Force virtual destructor
	 */
	virtual ~CDockComponentsFactory() {}

	virtual CDockWidgetTab* createDockWidgetTab(CDockWidget* DockWidget) const;
	virtual CDockAreaTabBar* createDockAreaTabBar(CDockAreaWidget* DockArea) const;
	virtual CDockAreaTitleBar* createDockAreaTitleBar(CDockAreaWidget* DockArea) const;

	/**
	 * Returns the default components factory
	 */
	static const CDockComponentsFactory* defaultFactory();

	/**
	 * Sets a new default factory for creation of GUI elements.
	 * This function takes ownership of the given Factory.
	 */
	static void setDefaultFactory(CDockComponentsFactory* Factory);
};


/**
 * Convenience function to ease factory instance access
 */
inline const CDockComponentsFactory* componentsFactory()
{
	return CDockComponentsFactory::defaultFactory();
}

} // namespace ads

//---------------------------------------------------------------------------
#endif // DockComponentsFactoryH
