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

#include "SectionWidget.h"
#include "DropOverlay.h"

#include "dialogs/SectionContentListWidget.h"

#include "icontitlewidget.h"

///////////////////////////////////////////////////////////////////////

static int CONTENT_COUNT = 0;

static ads::SectionContent::RefPtr createLongTextLabelSC(ads::CMainContainerWidget* container)
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
	ads::SectionContent::RefPtr sc = ads::SectionContent::newSectionContent(QString("uname-%1").arg(index), container, new IconTitleWidget(QIcon(), QString("Label %1").arg(index)), w);
	sc->setTitle("Ein Label " + QString::number(index));
	return sc;
}

static ads::SectionContent::RefPtr createCalendarSC(ads::CMainContainerWidget* container)
{
	QCalendarWidget* w = new QCalendarWidget();

	const int index = ++CONTENT_COUNT;
	return ads::SectionContent::newSectionContent(QString("uname-%1").arg(index), container, new IconTitleWidget(QIcon(), QString("Calendar %1").arg(index)), w);
}

static ads::SectionContent::RefPtr createFileSystemTreeSC(ads::CMainContainerWidget* container)
{
	QTreeView* w = new QTreeView();
	w->setFrameShape(QFrame::NoFrame);
	//	QFileSystemModel* m = new QFileSystemModel(w);
	//	m->setRootPath(QDir::currentPath());
	//	w->setModel(m);

	const int index = ++CONTENT_COUNT;
	return ads::SectionContent::newSectionContent(QString("uname-%1").arg(index), container, new IconTitleWidget(QIcon(), QString("Filesystem %1").arg(index)), w);
}

static void storeDataHelper(const QString& fname, const QByteArray& ba)
{
	QFile f(fname + QString(".dat"));
	if (f.open(QFile::WriteOnly))
	{
		f.write(ba);
		f.close();
	}
}

static QByteArray loadDataHelper(const QString& fname)
{
	QFile f(fname + QString(".dat"));
	if (f.open(QFile::ReadOnly))
	{
		QByteArray ba = f.readAll();
		f.close();
		return ba;
	}
	return QByteArray();
}

///////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// Setup actions.
	connect(ui->actionContentList, SIGNAL(triggered()), this, SLOT(showSectionContentListDialog()));

	// ADS - Create main container (ContainerWidget).
	_container = new ads::CMainContainerWidget();
	connect(_container, SIGNAL(activeTabChanged(const SectionContent::RefPtr&, bool)), this, SLOT(onActiveTabChanged(const SectionContent::RefPtr&, bool)));
	connect(_container, SIGNAL(sectionContentVisibilityChanged(SectionContent::RefPtr,bool)), this, SLOT(onSectionContentVisibilityChanged(SectionContent::RefPtr,bool)));
	setCentralWidget(_container);
	createContent();
	// Default window geometry
	resize(800, 600);
	restoreGeometry(loadDataHelper("MainWindow"));

	// ADS - Restore geometries and states of contents.
    //_container->restoreState(loadDataHelper("ContainerWidget"));
	_container->dumpLayout();
}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::createContent()
{
	// ADS - Adding some contents.
	// Test #1: Use high-level public API
	ads::CMainContainerWidget* cw = _container;
	ads::SectionWidget* sw = nullptr;

	sw = _container->addSectionContent(createLongTextLabelSC(cw), nullptr, ads::CenterDropArea);
	sw = _container->addSectionContent(createCalendarSC(cw), nullptr, ads::LeftDropArea);
	sw = _container->addSectionContent(createFileSystemTreeSC(cw), nullptr, ads::BottomDropArea);
	sw = _container->addSectionContent(createCalendarSC(cw), nullptr, ads::BottomDropArea);

	/*_container->addSectionContent(createCalendarSC(_container));
	_container->addSectionContent(createLongTextLabelSC(_container));
	_container->addSectionContent(createLongTextLabelSC(_container));
	_container->addSectionContent(createLongTextLabelSC(_container));

	ads::SectionContent::RefPtr sc = createLongTextLabelSC(cw);
	sc->setFlags(ads::SectionContent::AllFlags ^ ads::SectionContent::Closeable);
	_container->addSectionContent(sc);*/

#if 0
		// Issue #2: If the first drop is not into CenterDropArea, the application crashes.
		ads::CMainContainerWidget* cw = _container;
		ads::SectionWidget* sw = NULL;

		sw = _container->addSectionContent(createLongTextLabelSC(cw), sw, ads::LeftDropArea);
		sw = _container->addSectionContent(createCalendarSC(cw), sw, ads::LeftDropArea);
		sw = _container->addSectionContent(createLongTextLabelSC(cw), sw, ads::CenterDropArea);
		sw = _container->addSectionContent(createLongTextLabelSC(cw), sw, ads::CenterDropArea);
		sw = _container->addSectionContent(createLongTextLabelSC(cw), sw, ads::CenterDropArea);
		sw = _container->addSectionContent(createLongTextLabelSC(cw), sw, ads::RightDropArea);
		sw = _container->addSectionContent(createLongTextLabelSC(cw), sw, ads::BottomDropArea);
#endif
}

void MainWindow::showSectionContentListDialog()
{
	SectionContentListWidget::Values v;
	v.cw = _container;

	SectionContentListWidget w(this);
	w.setValues(v);
	w.exec();
}

void MainWindow::onActiveTabChanged(const ads::SectionContent::RefPtr& sc, bool active)
{
	Q_UNUSED(active);
	IconTitleWidget* itw = dynamic_cast<IconTitleWidget*>(sc->titleWidgetContent());
	if (itw)
	{
		itw->polishUpdate();
	}
}

void MainWindow::onSectionContentVisibilityChanged(const ads::SectionContent::RefPtr& sc, bool visible)
{
	qDebug() << Q_FUNC_INFO << sc->uniqueName() << visible;
}

void MainWindow::onActionAddSectionContentTriggered()
{
	return;
}

void MainWindow::contextMenuEvent(QContextMenuEvent* e)
{
	Q_UNUSED(e);
	QMenu* m = _container->createContextMenu();
	m->exec(QCursor::pos());
	delete m;
}

void MainWindow::closeEvent(QCloseEvent* e)
{
	Q_UNUSED(e);
	storeDataHelper("MainWindow", saveGeometry());
	storeDataHelper("ContainerWidget", _container->saveState());
}
