#include "SectionContentListWidget.h"
#include "SectionContentListModel.h"


SectionContentListWidget::SectionContentListWidget(QWidget* parent) :
	QDialog(parent)
{
	_ui.setupUi(this);
	connect(_ui.deleteButton, SIGNAL(clicked(bool)), this, SLOT(onDeleteButtonClicked()));
}

void SectionContentListWidget::setValues(const SectionContentListWidget::Values& v)
{
	_v = v;

	// Reset
	QAbstractItemModel* m = _ui.tableView->model();
	if (m)
	{
		_ui.tableView->setModel(NULL);
		delete m;
		m = NULL;
	}

	// Fill.
	SectionContentListModel* sclm = new SectionContentListModel(this);
	sclm->init(_v.cw);
	_ui.tableView->setModel(sclm);
}

void SectionContentListWidget::onDeleteButtonClicked()
{
	const QModelIndex mi = _ui.tableView->currentIndex();
	if (!mi.isValid())
		return;

	_ui.tableView->model()->removeRows(mi.row(), 1, mi.parent());
}
