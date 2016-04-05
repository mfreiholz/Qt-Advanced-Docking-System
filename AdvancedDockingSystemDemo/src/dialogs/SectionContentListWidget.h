#ifndef SECTIONCONTENTLISTWIDGET
#define SECTIONCONTENTLISTWIDGET

#include <QDialog>
#include "ui_SectionContentListWidget.h"

#include "ads/API.h"
#include "ads/ContainerWidget.h"
#include "ads/SectionContent.h"

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
