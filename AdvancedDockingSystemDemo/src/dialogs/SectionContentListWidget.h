#ifndef SECTIONCONTENTLISTWIDGET
#define SECTIONCONTENTLISTWIDGET

#include <QDialog>
#include "../../../AdvancedDockingSystem/src/SectionContent.h"
#include "MainContainerWidget.h"
#include "ui_SectionContentListWidget.h"

#include "API.h"

class SectionContentListWidget : public QDialog
{
	Q_OBJECT

public:
	class Values
	{
	public:
		ads::CMainContainerWidget* cw;
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
