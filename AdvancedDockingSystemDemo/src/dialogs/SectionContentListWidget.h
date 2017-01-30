#ifndef SECTIONCONTENTLISTWIDGET
#define SECTIONCONTENTLISTWIDGET

#include <QDialog>
#include "../../../AdvancedDockingSystem/include/ads/ContainerWidget.h"
#include "ui_SectionContentListWidget.h"

#include "ads/API.h"
#include "ads/SectionContent.h"

class SectionContentListWidget : public QDialog
{
	Q_OBJECT

public:
	class Values
	{
	public:
		ADS_NS::MainContainerWidget* cw;
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
