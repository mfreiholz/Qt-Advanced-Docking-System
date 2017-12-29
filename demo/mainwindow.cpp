#include "mainwindow.h"

#include "ui_mainwindow.h"


#include <QTime>
#include <QLabel>
#include <QTextEdit>
#include <QCalendarWidget>
#include <QFrame>
#include <QTreeView>
#include <QFileSystemModel>
#include <QBoxLayout>
#include <QSettings>
#include <QDockWidget>
#include <QDebug>

#include "DockManager.h"
#include "DockWidget.h"
#include "DockAreaWidget.h"




static ads::CDockWidget* createLongTextLabelDockWidget(QMenu* ViewMenu)
{
	static int LabelCount = 0;
	QLabel* l = new QLabel();
	l->setWordWrap(true);
	l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	l->setText(QString("Label %1 %2 - Lorem ipsum dolor sit amet, consectetuer adipiscing elit. "
		"Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque "
		"penatibus et magnis dis parturient montes, nascetur ridiculus mus. "
		"Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. "
		"Nulla consequat massa quis enim. Donec pede justo, fringilla vel, "
		"aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, "
		"imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede "
		"mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum "
		"semper nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, "
		"porttitor eu, consequat vitae, eleifend ac, enim. Aliquam lorem ante, "
		"dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla "
		"ut metus varius laoreet.")
		.arg(LabelCount)
		.arg(QTime::currentTime().toString("hh:mm:ss:zzz")));

	ads::CDockWidget* DockWidget = new ads::CDockWidget(QString("Label %1").arg(LabelCount++));
	DockWidget->setWidget(l);
	DockWidget->setObjectName(DockWidget->windowTitle());
	ViewMenu->addAction(DockWidget->toggleViewAction());
	return DockWidget;
}


static ads::CDockWidget* createCalendarDockWidget(QMenu* ViewMenu)
{
	static int CalendarCount = 0;
	QCalendarWidget* w = new QCalendarWidget();
	ads::CDockWidget* DockWidget = new ads::CDockWidget(QString("Calendar %1").arg(CalendarCount++));
	DockWidget->setWidget(w);
	DockWidget->setObjectName(DockWidget->windowTitle());
	ViewMenu->addAction(DockWidget->toggleViewAction());
	return DockWidget;
}

static ads::CDockWidget* createFileSystemTreeDockWidget(QMenu* ViewMenu)
{
	static int FileSystemCount = 0;
	QTreeView* w = new QTreeView();
	w->setFrameShape(QFrame::NoFrame);
	QFileSystemModel* m = new QFileSystemModel(w);
	m->setRootPath(QDir::currentPath());
	w->setModel(m);
	ads::CDockWidget* DockWidget = new ads::CDockWidget(QString("Filesystem %1").arg(FileSystemCount++));
	DockWidget->setWidget(w);
	DockWidget->setObjectName(DockWidget->windowTitle());
	ViewMenu->addAction(DockWidget->toggleViewAction());
    return DockWidget;
}



MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	m_DockManager = new ads::CDockManager(this);
	createContent();
	// Default window geometry
	resize(800, 600);
}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::createContent()
{
	// Test container docking
	QMenu* ViewMenu = this->ui->menuView;
	auto DockWidget = createCalendarDockWidget(ViewMenu);
	DockWidget->setFeatures(DockWidget->features().setFlag(ads::CDockWidget::DockWidgetClosable, false));
	m_DockManager->addDockWidget(ads::LeftDockWidgetArea, DockWidget);
	m_DockManager->addDockWidget(ads::LeftDockWidgetArea, createLongTextLabelDockWidget(ViewMenu));
	/*m_DockManager->addDockWidget(ads::BottomDockWidgetArea, createFileSystemTreeDockWidget(ViewMenu));
	auto TopDockArea = m_DockManager->addDockWidget(ads::TopDockWidgetArea, createFileSystemTreeDockWidget(ViewMenu));
	DockWidget = createCalendarDockWidget(ViewMenu);
	DockWidget->setFeatures(DockWidget->features().setFlag(ads::CDockWidget::DockWidgetClosable, false));
	m_DockManager->addDockWidget(ads::CenterDockWidgetArea, DockWidget, TopDockArea);

	// Test dock area docking
	auto RighDockArea = m_DockManager->addDockWidget(ads::RightDockWidgetArea, createLongTextLabelDockWidget(ViewMenu), TopDockArea);
	m_DockManager->addDockWidget(ads::TopDockWidgetArea, createLongTextLabelDockWidget(ViewMenu), RighDockArea);
	auto BottomDockArea = m_DockManager->addDockWidget(ads::BottomDockWidgetArea, createLongTextLabelDockWidget(ViewMenu), RighDockArea);
	m_DockManager->addDockWidget(ads::RightDockWidgetArea, createLongTextLabelDockWidget(ViewMenu), RighDockArea);
	m_DockManager->addDockWidget(ads::CenterDockWidgetArea, createLongTextLabelDockWidget(ViewMenu), BottomDockArea);*/
}


void MainWindow::closeEvent(QCloseEvent* event)
{
	/*QSettings Settings("Settings.ini", QSettings::IniFormat);
	Settings.setValue("mainWindow/Geometry", saveGeometry());
	Settings.setValue("mainWindow/DockingState", m_DockManager->saveState());*/
	QMainWindow::closeEvent(event);
}


void MainWindow::on_actionSaveState_triggered(bool)
{
	qDebug() << "MainWindow::on_actionSaveState_triggered";
	QSettings Settings("Settings.ini", QSettings::IniFormat);
	Settings.setValue("mainWindow/Geometry", saveGeometry());
	Settings.setValue("mainWindow/DockingState", m_DockManager->saveState());
}


void MainWindow::on_actionRestoreState_triggered(bool)
{
	qDebug() << "MainWindow::on_actionRestoreState_triggered";
	QSettings Settings("Settings.ini", QSettings::IniFormat);
	restoreGeometry(Settings.value("mainWindow/Geometry").toByteArray());
	m_DockManager->restoreState(Settings.value("mainWindow/DockingState").toByteArray());
}

