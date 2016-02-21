#ifndef LAYOUTMANAGERWIDGET_H
#define LAYOUTMANAGERWIDGET_H

#include <QWidget>

namespace Ui {
class LayoutManagerWidgetForm;
}

class LayoutManagerWidget : public QWidget
{
	Q_OBJECT

public:
	LayoutManagerWidget(QWidget* parent);
	virtual ~LayoutManagerWidget();

private:
	Ui::LayoutManagerWidgetForm *_ui;
};

#endif
