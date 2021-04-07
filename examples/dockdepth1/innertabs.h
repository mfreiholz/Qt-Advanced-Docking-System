#pragma once

#include "DockManager.h"

#include <QAction>

class DockManagerWithInnerTabs : public ads::CDockManager
{
    Q_OBJECT

public:
    using ads::CDockManager::CDockManager;

    ~DockManagerWithInnerTabs() override; 

    std::pair<ads::CDockWidget*,ads::CDockContainerWidget*> createGroup( const QString& groupName, ads::CDockAreaWidget*& insertPos );

    /** Manually fill a given view menu */
    void setupViewMenu( QMenu* menu );

    /** Attach a view menu that will be automatically fill */
    void attachViewMenu( QMenu* menu );

private slots:
    void autoFillAttachedViewMenu();

private:
    void setupMenu( QMenu* menu, ads::CDockContainerWidget* areaWidget );
};

class CreateGroupAction : public QAction
{
    Q_OBJECT
public:
    CreateGroupAction( DockManagerWithInnerTabs* manager, ads::CDockAreaWidget* insertIn, QMenu* menu );

public slots:
    void createGroup();

private:
    DockManagerWithInnerTabs* m_manager;
    ads::CDockAreaWidget* m_insertPos;
};


