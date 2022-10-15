#ifndef SECTIONCONTENTLISTWIDGET
#define SECTIONCONTENTLISTWIDGET

#include <QDialog>

#include "AdvancedDockingSystem/API.h"
#include "AdvancedDockingSystem/ContainerWidget.h"
#include "AdvancedDockingSystem/SectionContent.h"
#include "ui_SectionContentListWidget.h"

class SectionContentListWidget : public QDialog
{
  Q_OBJECT

public:
  class Values
  {
  public:
    ADS_NS::ContainerWidget* cw;
  };

  SectionContentListWidget(QWidget* parent);
  void setValues(const Values& v);

private slots:
  void onDeleteButtonClicked();

private:
  Ui::SectionContentListWidgetForm _ui;
  Values _v;
};

#endif
