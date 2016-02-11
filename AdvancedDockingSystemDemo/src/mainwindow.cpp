#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTime>
#include <QLabel>
#include <QTextEdit>
#include <QCalendarWidget>
#include <QFrame>
#include <QTreeView>
#include <QFileSystemModel>

#include "ads/ContainerWidget.h"
#include "ads/SectionWidget.h"
#include "ads/SectionContent.h"

#include "icontitlewidget.h"

///////////////////////////////////////////////////////////////////////

static int CONTENT_COUNT = 0;

static ads::SectionContent::RefPtr createLongTextLabelSC()
{
	QWidget* w = new QWidget();
	QBoxLayout* bl = new QBoxLayout(QBoxLayout::TopToBottom);
	w->setLayout(bl);

	QLabel* l = new QLabel();
	l->setWordWrap(true);
	l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	l->setText(QString("Lorem Ipsum ist ein einfacher Demo-Text für die Print- und Schriftindustrie. Lorem Ipsum ist in der Industrie bereits der Standard Demo-Text seit 1500, als ein unbekannter Schriftsteller eine Hand voll Wörter nahm und diese durcheinander warf um ein Musterbuch zu erstellen. Es hat nicht nur 5 Jahrhunderte überlebt, sondern auch in Spruch in die elektronische Schriftbearbeitung geschafft (bemerke, nahezu unverändert). Bekannt wurde es 1960, mit dem erscheinen von Letrase, welches Passagen von Lorem Ipsum enhielt, so wie Desktop Software wie Aldus PageMaker - ebenfalls mit Lorem Ipsum."));
	bl->addWidget(l);

	const int index = ++CONTENT_COUNT;
	return ads::SectionContent::newSectionContent(new IconTitleWidget(QIcon(), QString("Label %1").arg(index)), w, QString("uname-%1").arg(index));
}

static ads::SectionContent::RefPtr createCalendarSC()
{
	QCalendarWidget* w = new QCalendarWidget();

	const int index = ++CONTENT_COUNT;
	return ads::SectionContent::newSectionContent(new IconTitleWidget(QIcon(), QString("Calendar %1").arg(index)), w, QString("uname-%1").arg(index));
}

static ads::SectionContent::RefPtr createFileSystemTreeSC()
{
	QTreeView* w = new QTreeView();
//	QFileSystemModel* m = new QFileSystemModel(w);
//	m->setRootPath(QDir::currentPath());
//	w->setModel(m);

	const int index = ++CONTENT_COUNT;
	return ads::SectionContent::newSectionContent(new IconTitleWidget(QIcon(), QString("Filesystem %1").arg(index)), w, QString("uname-%1").arg(index));
}

///////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->mainToolBar->hide();
	ui->statusBar->hide();
	QObject::connect(ui->actionAddSectionContent, &QAction::triggered, this, &MainWindow::onActionAddSectionContentTriggered);

	// CREATE SOME TESTING DOCKS
	_container = new ads::ContainerWidget();
	_container->setOrientation(Qt::Vertical);
	setCentralWidget(_container);

	ads::SectionWidget* section = NULL;

	section = new ads::SectionWidget(_container);
	section->addContent(createLongTextLabelSC());
	_container->addSection(section);

	section = new ads::SectionWidget(_container);
	section->addContent(createCalendarSC());
	_container->addSection(section);

	section = new ads::SectionWidget(_container);
	section->addContent(createFileSystemTreeSC());
	_container->addSection(section);

	section = new ads::SectionWidget(_container);
	section->addContent(createCalendarSC());
	_container->addSection(section);

	QByteArray ba;
	QFile f("test.dat");
	if (f.open(QFile::ReadOnly))
	{
		ba = f.readAll();
		f.close();
	}
	_container->restoreGeometry(ba);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::onActionAddSectionContentTriggered()
{
	return;

//	auto titleWidget = new IconTitleWidget(QIcon(), QString("Title"));
//	auto contentWidget = createRandomWidget(-1, -1);
//	auto content = ads::SectionContent::newSectionContent(titleWidget, contentWidget);

//	auto section = new ads::SectionWidget(_container);
//	_container->addSection(section);
//	section->addContent(content);
}

void MainWindow::contextMenuEvent(QContextMenuEvent* e)
{
	QMenu* m = _container->createContextMenu();
	m->exec(QCursor::pos());
	delete m;
}

void MainWindow::closeEvent(QCloseEvent* e)
{
	QByteArray ba = _container->saveGeometry();
	QFile f("test.dat");
	if (f.open(QFile::WriteOnly))
	{
		f.write(ba);
		f.close();
	}
}
