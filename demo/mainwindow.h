#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DockManager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
protected:
	virtual void closeEvent(QCloseEvent* event) override;
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
