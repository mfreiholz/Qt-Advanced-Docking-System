#include "innertabs.h"

#include "DockAreaWidget.h"

#include <QInputDialog>
#include <QMenu>

/////////////////////////////////////
// DockManagerWithInnerTabs
/////////////////////////////////////
void deleteAllChildren( ads::CDockContainerWidget* areaWidget )
{
    // fix crash on close by manually deleting children
    // maybe due to this issue: https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System/issues/307
    
    std::vector<ads::CDockAreaWidget*> areas;
    for ( int i = 0; i != areaWidget->dockAreaCount(); ++i )
    {
        areas.push_back( areaWidget->dockArea(i) );
    }

    for ( auto area : areas )
    {
        for ( auto widget : area->dockWidgets() )
        {
            ads::CDockContainerWidget* subArea = dynamic_cast<ads::CDockContainerWidget*>( widget->widget() );
            if ( subArea )
                deleteAllChildren( subArea );
            delete widget;
        }
        
        delete area;
    }
}

DockManagerWithInnerTabs::~DockManagerWithInnerTabs()
{
    deleteAllChildren( this );
}

std::pair<ads::CDockWidget*,ads::CDockContainerWidget*> DockManagerWithInnerTabs::createGroup( const QString& groupName, ads::CDockAreaWidget*& insertPos )
{
    ads::CDockWidget* groupedDockWidget = new ads::CDockWidget(groupName);

    ads::CDockContainerWidget* groupManager = new ads::CDockContainerWidget(this);
    groupedDockWidget->setWidget(groupManager);

    insertPos = this->addDockWidget(ads::CenterDockWidgetArea, groupedDockWidget, insertPos);

    return { groupedDockWidget, groupManager };
}

void DockManagerWithInnerTabs::attachViewMenu( QMenu* menu )
{
    connect( menu, SIGNAL(aboutToShow()), this, SLOT(autoFillAttachedViewMenu()) );
}

void DockManagerWithInnerTabs::autoFillAttachedViewMenu()
{
    QMenu* menu = dynamic_cast<QMenu*>( QObject::sender() );

    if ( menu )
    {
        menu->clear();
        setupViewMenu( menu );
    }
    else
    {
        assert( false );
    }
}

bool SortFunc( ads::CDockWidget* left, ads::CDockWidget* right )
{
    if ( left->windowTitle() == right->windowTitle() )
    {
        assert( false );
        return left < right;
    }
    else
    {
        return left->windowTitle() < right->windowTitle();
    }
}

void DockManagerWithInnerTabs::setupMenu( QMenu* menu, ads::CDockContainerWidget* areaWidget )
{
    std::vector<ads::CDockWidget*> widgets;

    ads::CDockManager* dockManager = dynamic_cast<ads::CDockManager*>( areaWidget );
    if ( dockManager )
    {
        for ( ads::CFloatingDockContainer* floating : dockManager->floatingWidgets() )
        {
            for ( auto floated : floating->dockWidgets() )
                widgets.push_back( floated );
        }
    }

    for ( int i = 0; i != areaWidget->dockAreaCount(); ++i )
    {
        for ( auto docked : areaWidget->dockArea(i)->dockWidgets() )
            widgets.push_back( docked );
    }

    std::sort( widgets.begin(), widgets.end(), SortFunc );

    for ( auto widget : widgets )
    {
        auto action = widget->toggleViewAction();

        ads::CDockContainerWidget* subArea = dynamic_cast<ads::CDockContainerWidget*>( widget->widget() );
        if ( subArea )
        {
            auto subMenu = menu->addMenu( widget->windowTitle() );

            subMenu->addAction( action );
            subMenu->addSeparator();

            setupMenu( subMenu, subArea );
        }
        else
        {
            menu->addAction(action);
        }
    }

    if ( dockManager )
    {
        menu->addSeparator();
        int count = areaWidget->dockAreaCount();
        if ( count == 0 )
        {
            menu->addAction( new CreateGroupAction( this, NULL, menu ) );
        }
        else
        {
            for ( int i = 0; i != count; ++i )
            {
                menu->addAction( new CreateGroupAction( this, areaWidget->dockArea(i), menu ) );
            }
        }
    }
    // else, don't permit to add groups in groups
    // that would be nice, but it's not handled correctly upon drag/drop of a widget, it cannot be dropped in the inner docking area
}

void DockManagerWithInnerTabs::setupViewMenu( QMenu* menu )
{
    setupMenu( menu, this );
}

/////////////////////////////////////
// CreateGroupAction
/////////////////////////////////////
CreateGroupAction::CreateGroupAction( DockManagerWithInnerTabs* manager, ads::CDockAreaWidget* insertPos, QMenu* menu ) :
    QAction("New group...", menu),
    m_manager( manager),
    m_insertPos( insertPos )
{
    connect( this, SIGNAL(triggered()), this, SLOT(createGroup()) );
}

void CreateGroupAction::createGroup()
{
    QString name = QInputDialog::getText( NULL, text(), "Enter group name" );
    if ( !name.isEmpty() )
    {
        m_manager->createGroup( name, m_insertPos );
    }
}
