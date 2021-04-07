#include "../../examples/dockdepth1/MainWindow.h"
#include "../../examples/dockdepth1/innertabs.h"

#include <QLabel>
#include <QMenuBar>

#include "DockAreaWidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    resize( 400, 400 );

    DockManagerWithInnerTabs* dockManager = new DockManagerWithInnerTabs(this);

    dockManager->attachViewMenu( menuBar()->addMenu( "View" ) );

    ads::CDockAreaWidget* previousDockWidget = NULL;
    for ( int i = 0; i != 3; ++i )
    {
        // Create example content label - this can be any application specific
        // widget
        QLabel* l = new QLabel();
        l->setWordWrap(true);
        l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

        // Create a dock widget with the title Label 1 and set the created label
        // as the dock widget content
        ads::CDockWidget* DockWidget = new ads::CDockWidget("Label " + QString::number(i));
        DockWidget->setWidget(l);

        // Add the dock widget to the top dock widget area
        previousDockWidget = dockManager->addDockWidget(ads::CenterDockWidgetArea, DockWidget, previousDockWidget);
    }

    ads::CDockContainerWidget* groupManager = dockManager->createGroup( "Group", previousDockWidget ).second;

    previousDockWidget = NULL;
    for ( int i = 0; i != 3; ++i )
    {
        // Create example content label - this can be any application specific
        // widget
        QLabel* l = new QLabel();
        l->setWordWrap(true);
        l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

        // Create a dock widget with the title Label 1 and set the created label
        // as the dock widget content
        ads::CDockWidget* DockWidget = new ads::CDockWidget("Inner " + QString::number(i));
        DockWidget->setWidget(l);

        // Add the dock widget to the top dock widget area
        previousDockWidget = groupManager->addDockWidget(ads::CenterDockWidgetArea, DockWidget, previousDockWidget);
    }
}

MainWindow::~MainWindow()
{

}

