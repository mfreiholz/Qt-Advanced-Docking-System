#include <QString>
#include <QFile>
#include <QApplication>

#include "mainwindow.h"

static void initStyleSheet(QApplication& a)
{
	QFile f(":/stylesheets/default-windows.css");
//	QFile f(":/stylesheets/modern-windows.css");
//	QFile f(":/stylesheets/vendor-partsolutions.css");
	if (f.open(QFile::ReadOnly))
	{
		QByteArray ba = f.readAll();
		f.close();
		a.setStyleSheet(QString(ba));
	}
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//Q_INIT_RESOURCE(ads);
	a.setQuitOnLastWindowClosed(true);
	initStyleSheet(a);

	MainWindow mw;
	mw.show();
	return a.exec();
}
