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


///////////////////////////////////////////////////////////////////////

static int CONTENT_COUNT = 0;

static ads::CDockWidget* createLongTextLabelDockWidget(ads::CDockManager* DockManager)
{
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

	ads::CDockWidget* DockWidget = new ads::CDockWidget("DockWidget1");
	DockWidget->setWidget(l);
	return DockWidget;
}

static ads::CDockWidget* createCalendarDockWidget(ads::CDockManager* DockManager)
{
	QCalendarWidget* w = new QCalendarWidget();
	ads::CDockWidget* DockWidget = new ads::CDockWidget("DockWidget1");
	DockWidget->setWidget(w);
	return DockWidget;
}

static ads::CDockWidget* createFileSystemTreeDockWidget(ads::CDockManager* DockManager)
{
	QTreeView* w = new QTreeView();
	w->setFrameShape(QFrame::NoFrame);
	QFileSystemModel* m = new QFileSystemModel(w);
	m->setRootPath(QDir::currentPath());
	w->setModel(m);
	ads::CDockWidget* DockWidget = new ads::CDockWidget("DockWidget1");
	DockWidget->setWidget(w);
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
	m_DockManager->addDockWidget(ads::LeftDockWidgetArea, createCalendarDockWidget(m_DockManager));
	m_DockManager->addDockWidget(ads::LeftDockWidgetArea, createLongTextLabelDockWidget(m_DockManager));
	m_DockManager->addDockWidget(ads::BottomDockWidgetArea, createFileSystemTreeDockWidget(m_DockManager));
	m_DockManager->addDockWidget(ads::TopDockWidgetArea, createFileSystemTreeDockWidget(m_DockManager));
}


