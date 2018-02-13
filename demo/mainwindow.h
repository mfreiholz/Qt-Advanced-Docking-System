#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QWidgetAction>
#include <QComboBox>

#include <QMainWindow>
#include "DockManager.h"

namespace Ui {
class MainWindow;
}

/**
 * Simple main window for demo
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT
private:
	QAction* m_SavePerspectiveAction;
	QWidgetAction* m_PerspectiveListAction;
	QComboBox* m_PerspectiveComboBox;

protected:
	virtual void closeEvent(QCloseEvent* event) override;
;
public:
	explicit MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

private:
	Ui::MainWindow *ui;
	ads::CDockManager* m_DockManager;
	void createContent();

private slots:
	void on_actionSaveState_triggered(bool);
	void on_actionRestoreState_triggered(bool);
};

#endif // MAINWINDOW_H
