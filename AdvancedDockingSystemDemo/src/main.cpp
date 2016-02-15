#include <QString>
#include <QFile>
#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(true);

	Q_INIT_RESOURCE(ads);

	// Load style sheet
	QFile f(":/stylesheets/default-windows.css");
//	QFile f(":/stylesheets/vendor-partsolutions.css");
	if (f.open(QFile::ReadOnly))
	{
		QByteArray ba = f.readAll();
		f.close();
		a.setStyleSheet(QString(ba));
	}

	MainWindow mw;
	mw.show();
	return a.exec();
}

//	a.setStyleSheet(""
//		" QSplitter::handle { border: 1px solid #000000; background: #000000; } "
//		" ads--ContainerWidget { border: 1px solid #ff0000; background: #FFE6E6; padding: 6px; } "
//		" ads--SectionWidget { border: 1px solid #00ff00; background: #E6FFE6; padding: 6px; } "
//		" ads--SectionTitleWidget { border: 1px solid #0000ff; background: #E6E6FF; padding: 6px; } "
//		" ads--SectionTitleWidget[activeTab=\"true\"] { border: 1px solid #0000ff; background: #9696FF; padding: 6px; } "
//		" ads--SectionContentWidget { border: 1px solid #FFFF00; background: #FFFFE6; padding: 6px; } "
//	);

//static void centerWidget(QWidget* widget)
//{
//	if (widget)
//	{
//		QDesktopWidget deskWidget;
//		const int screenIndex = deskWidget.primaryScreen();
//		const QRect deskRect = deskWidget.availableGeometry(screenIndex);
//		const int x = (deskRect.width() - widget->rect().width()) / 2;
//		const int y = (deskRect.height() - widget->rect().height()) / 2;
//		widget->move(x, y);
//	}
//}

//static void resizeWidgetPerCent(QWidget* widget, qreal widthPC, qreal heightPC)
//{
//	if (widget && widthPC >= 0.0 && heightPC >= 0.0)
//	{
//		QDesktopWidget deskWidget;
//		const int screenIndex = deskWidget.primaryScreen();
//		const QRect deskRect = deskWidget.availableGeometry(screenIndex);
//		const int w = (deskRect.width() / 100) * widthPC;
//		const int h = (deskRect.height() / 100) * heightPC;
//		widget->resize(w, h);
//	}
//}