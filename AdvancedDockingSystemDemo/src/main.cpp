#include <QString>
#include <QFile>
#include <QApplication>

#include "mainwindow.h"

static void initStyleSheet(QApplication& a)
{
	//Q_INIT_RESOURCE(ads); // If static linked.
	QFile f(":ads/stylesheets/default-windows.css");
	if (f.open(QFile::ReadOnly))
	{
		const QByteArray ba = f.readAll();
		f.close();
		a.setStyleSheet(QString(ba));
	}
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(true);
	initStyleSheet(a);

	MainWindow mw;
	mw.show();
	return a.exec();
}
