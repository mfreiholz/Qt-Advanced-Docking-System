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

#include "DockManager.h"
#include "DockWidget.h"
#include "DockAreaWidget.h"


///////////////////////////////////////////////////////////////////////

static int CONTENT_COUNT = 0;

static ads::CDockWidget* createLongTextLabelDockWidget(QMenu* ViewMenu)
{
	static int LabelCount = 0;
	QLabel* l = new QLabel();
	l->setWordWrap(true);
	l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	l->setText(QString("Lorem Ipsum ist ein einfacher Demo-Text für die Print- "
		"und Schriftindustrie. Lorem Ipsum ist in der Industrie bereits der "
		"Standard Demo-Text seit 1500, als ein unbekannter Schriftsteller eine "
		"Hand voll Wörter nahm und diese durcheinander warf um ein Musterbuch zu "
		"erstellen. Es hat nicht nur 5 Jahrhunderte überlebt, sondern auch in "
		"Spruch in die elektronische Schriftbearbeitung geschafft (bemerke, nahezu "
		"unverändert). Bekannt wurde es 1960, mit dem erscheinen von Letrase, "
		"welches Passagen von Lorem Ipsum enhielt, so wie Desktop Software wie "
		"Aldus PageMaker - ebenfalls mit Lorem Ipsum."));

	ads::CDockWidget* DockWidget = new ads::CDockWidget(QString("Label %1").arg(LabelCount++));
	DockWidget->setWidget(l);
	ViewMenu->addAction(DockWidget->toggleViewAction());
	return DockWidget;
}

static ads::CDockWidget* createCalendarDockWidget(QMenu* ViewMenu)
{
	static int CalendarCount = 0;
	QCalendarWidget* w = new QCalendarWidget();
	ads::CDockWidget* DockWidget = new ads::CDockWidget(QString("Calendar %1").arg(CalendarCount++));
	DockWidget->setWidget(w);
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
	ViewMenu->addAction(DockWidget->toggleViewAction());
    return DockWidget;
}



///////////////////////////////////////////////////////////////////////

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
	m_DockManager->addDockWidget(ads::BottomDockWidgetArea, createFileSystemTreeDockWidget(ViewMenu));
	auto TopDockArea = m_DockManager->addDockWidget(ads::TopDockWidgetArea, createFileSystemTreeDockWidget(ViewMenu));
	DockWidget = createCalendarDockWidget(ViewMenu);
	DockWidget->setFeatures(DockWidget->features().setFlag(ads::CDockWidget::DockWidgetClosable, false));
	m_DockManager->addDockWidget(ads::CenterDockWidgetArea, DockWidget, TopDockArea);

	// Test dock area docking
	auto RighDockArea = m_DockManager->addDockWidget(ads::RightDockWidgetArea, createLongTextLabelDockWidget(ViewMenu), TopDockArea);
	m_DockManager->addDockWidget(ads::TopDockWidgetArea, createLongTextLabelDockWidget(ViewMenu), RighDockArea);
	auto BottomDockArea = m_DockManager->addDockWidget(ads::BottomDockWidgetArea, createLongTextLabelDockWidget(ViewMenu), RighDockArea);
	m_DockManager->addDockWidget(ads::RightDockWidgetArea, createLongTextLabelDockWidget(ViewMenu), RighDockArea);
	m_DockManager->addDockWidget(ads::CenterDockWidgetArea, createLongTextLabelDockWidget(ViewMenu), BottomDockArea);
}


