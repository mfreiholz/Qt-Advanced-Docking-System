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

#include "ads/SectionWidget.h"

#include "icontitlewidget.h"

///////////////////////////////////////////////////////////////////////

static int CONTENT_COUNT = 0;

static ADS_NS::SectionContent::RefPtr createLongTextLabelSC(ADS_NS::ContainerWidget* container)
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
	ADS_NS::SectionContent::RefPtr sc = ADS_NS::SectionContent::newSectionContent(QString("uname-%1").arg(index), container, new IconTitleWidget(QIcon(), QString("Label %1").arg(index)), w);
	sc->setTitle("Ein Label " + QString::number(index));
	return sc;
}

static ADS_NS::SectionContent::RefPtr createCalendarSC(ADS_NS::ContainerWidget* container)
{
	QCalendarWidget* w = new QCalendarWidget();

	const int index = ++CONTENT_COUNT;
	return ADS_NS::SectionContent::newSectionContent(QString("uname-%1").arg(index), container, new IconTitleWidget(QIcon(), QString("Calendar %1").arg(index)), w);
}

static ADS_NS::SectionContent::RefPtr createFileSystemTreeSC(ADS_NS::ContainerWidget* container)
{
	QTreeView* w = new QTreeView();
	w->setFrameShape(QFrame::NoFrame);
	//	QFileSystemModel* m = new QFileSystemModel(w);
	//	m->setRootPath(QDir::currentPath());
	//	w->setModel(m);

	const int index = ++CONTENT_COUNT;
	return ADS_NS::SectionContent::newSectionContent(QString("uname-%1").arg(index), container, new IconTitleWidget(QIcon(), QString("Filesystem %1").arg(index)), w);
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

	// ADS - Create main container (ContainerWidget).
	_container = new ADS_NS::ContainerWidget();
	_container->setOrientation(Qt::Vertical);
#if QT_VERSION >= 0x050000
	QObject::connect(_container, &ADS_NS::ContainerWidget::activeTabChanged, this, &MainWindow::onActiveTabChanged);
#else
	QObject::connect(_container, SIGNAL(activeTabChanged(const SectionContent::RefPtr&, bool)), this, SLOT(onActiveTabChanged(const SectionContent::RefPtr&, bool)));
#endif
	setCentralWidget(_container);

	// ADS - Adding some contents.
	// Test #1: Use high-level public API
	if (true)
	{
		ADS_NS::ContainerWidget* cw = _container;
		ADS_NS::SectionWidget* sw = NULL;

		sw = _container->addSectionContent(createLongTextLabelSC(cw), sw, ADS_NS::CenterDropArea);
		sw = _container->addSectionContent(createCalendarSC(cw), sw, ADS_NS::RightDropArea);
		sw = _container->addSectionContent(createFileSystemTreeSC(cw), sw, ADS_NS::CenterDropArea);

		_container->addSectionContent(createCalendarSC(_container));
		_container->addSectionContent(createLongTextLabelSC(_container));
		_container->addSectionContent(createLongTextLabelSC(_container));
		_container->addSectionContent(createLongTextLabelSC(_container));
	}

	// Default window geometry
	resize(800, 600);
	restoreGeometry(loadDataHelper("MainWindow"));

	// ADS - Restore geometries and states of contents.
	_container->restoreState(loadDataHelper("ContainerWidget"));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::onActiveTabChanged(const ADS_NS::SectionContent::RefPtr& sc, bool active)
{
	Q_UNUSED(active);
	IconTitleWidget* itw = dynamic_cast<IconTitleWidget*>(sc->titleWidget());
	if (itw)
	{
		itw->polishUpdate();
	}
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
