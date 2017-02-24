#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../../AdvancedDockingSystem/src/MainContainerWidget.h"
#include "../../AdvancedDockingSystem/src/SectionContent.h"
#include "API.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

public slots:
	void showSectionContentListDialog();

private slots:
	void onActiveTabChanged(const ads::SectionContent::RefPtr& sc, bool active);
	void onSectionContentVisibilityChanged(const ads::SectionContent::RefPtr& sc, bool visible);
	void onActionAddSectionContentTriggered();

protected:
	virtual void contextMenuEvent(QContextMenuEvent* e);
	virtual void closeEvent(QCloseEvent* e);

private:
	Ui::MainWindow *ui;
	ads::CMainContainerWidget* _container;
	void createContent();
};

#endif // MAINWINDOW_H
