#include "LayoutManagerWidget.h"

#include <QList>

#include "ui_layoutmanagerwidget.h"

LayoutManagerWidget::LayoutManagerWidget(QWidget* parent) :
	QWidget(parent, NULL),
	_ui(new Ui::LayoutManagerWidgetForm())
{
	_ui->setupUi(this);

	QList<int> sizes;
	sizes << 1 << 5;
	_ui->splitter->setSizes(sizes);
}

LayoutManagerWidget::~LayoutManagerWidget()
{
	delete _ui;
}
