#include <QString>
#include <QRect>
#include <QApplication>
#include <QDesktopWidget>

#include "mainwindow.h"

static void centerWidget(QWidget* widget)
{
	if (widget)
	{
		QDesktopWidget deskWidget;
		const auto screenIndex = deskWidget.primaryScreen();
		const auto deskRect = deskWidget.availableGeometry(screenIndex);
		const auto x = (deskRect.width() - widget->rect().width()) / 2;
		const auto y = (deskRect.height() - widget->rect().height()) / 2;
		widget->move(x, y);
	}
}

static void resizeWidgetPerCent(QWidget* widget, qreal widthPC, qreal heightPC)
{
	if (widget && widthPC >= 0.0 && heightPC >= 0.0)
	{
		QDesktopWidget deskWidget;
		const auto screenIndex = deskWidget.primaryScreen();
		const auto deskRect = deskWidget.availableGeometry(screenIndex);
		const auto w = (deskRect.width() / 100) * widthPC;
		const auto h = (deskRect.height() / 100) * heightPC;
		widget->resize(w, h);
	}
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(true);

	Q_INIT_RESOURCE(ads);

	// Default style.
	a.setStyleSheet(""
		" QSplitter::handle { background: palette(dark); } "
		" ads--ContainerWidget { background: palette(dark); } "
		" ads--SectionWidget { background: palette(window); } "
		" ads--SectionTitleWidget { background: palette(window); } "
		" ads--SectionTitleWidget[activeTab=\"true\"] { background: palette(light); } "
		" ads--SectionContentWidget { border: 1px solid palette(light); } "
		"  "
	);

	// Development style.
//	a.setStyleSheet(""
//		" QSplitter::handle { border: 1px solid #000000; background: #000000; } "
//		" ads--ContainerWidget { border: 1px solid #ff0000; background: #FFE6E6; padding: 6px; } "
//		" ads--SectionWidget { border: 1px solid #00ff00; background: #E6FFE6; padding: 6px; } "
//		" ads--SectionTitleWidget { border: 1px solid #0000ff; background: #E6E6FF; padding: 6px; } "
//		" ads--SectionTitleWidget[activeTab=\"true\"] { border: 1px solid #0000ff; background: #9696FF; padding: 6px; } "
//		" ads--SectionContentWidget { border: 1px solid #FFFF00; background: #FFFFE6; padding: 6px; } "
//	);

	// PARTsolutions style.
//	a.setStyleSheet(""
//		" QSplitter::handle:vertical { image: url(:/img/splitter-horizontal.png); } "
//		" QSplitter::handle:horizontal { image: url(:/img/splitter-vertical.png); } "
//		" ads--ContainerWidget { border: 0; background: #9ab6ca; } "
//		" ads--SectionWidget { border-width: 1px; border-color: #ffffff; border-style: solid; background: #7c9eb3; padding: 0; margin: 0; } "
//		" ads--SectionTitleWidget { border-right: 1px solid #E7F3F8; background: #7c9eb3; } "
//		" ads--SectionTitleWidget[activeTab=\"true\"] { border: 1px solid #E7F3F8; background: #E7F3F8; } "
//		" ads--SectionTitleWidget IconTitleWidget QLabel { color: #000000; } "
//		" ads--SectionContentWidget { border: 0px solid #E7F3F8; background: #ffffff; } "
//		" ads--FloatingWidget QPushButton { background: #E7F3F8; } "
//	);

	MainWindow mw;
	resizeWidgetPerCent(&mw, 60, 80);
	centerWidget(&mw);
	mw.show();

	return a.exec();
}