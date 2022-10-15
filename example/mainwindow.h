#ifndef DB3ED684_D2C6_4905_B9FE_960614ACF6E6
#define DB3ED684_D2C6_4905_B9FE_960614ACF6E6
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "AdvancedDockingSystem/API.h"
#include "AdvancedDockingSystem/ContainerWidget.h"
#include "AdvancedDockingSystem/SectionContent.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = 0);
  virtual ~MainWindow();

public slots:
  void showSectionContentListDialog();

private slots:
#if QT_VERSION >= 0x050000
  void onActiveTabChanged(const ADS_NS::SectionContent::RefPtr& sc,
                          bool active);
  void onSectionContentVisibilityChanged(
      const ADS_NS::SectionContent::RefPtr& sc, bool visible);
#else
  void onActiveTabChanged(const SectionContent::RefPtr& sc, bool active);
  void onSectionContentVisibilityChanged(const SectionContent::RefPtr& sc,
                                         bool visible);
#endif
  void onActionAddSectionContentTriggered();

protected:
  virtual void contextMenuEvent(QContextMenuEvent* e);
  virtual void closeEvent(QCloseEvent* e);

private:
  Ui::MainWindow* ui;
  ADS_NS::ContainerWidget* _container;
};

#endif  // MAINWINDOW_H


#endif /* DB3ED684_D2C6_4905_B9FE_960614ACF6E6 */
