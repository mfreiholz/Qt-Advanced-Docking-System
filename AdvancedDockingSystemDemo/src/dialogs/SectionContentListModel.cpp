#include "SectionContentListModel.h"

SectionContentListModel::SectionContentListModel(QObject* parent) :
	QAbstractTableModel(parent)
{
	_headers.insert(UidColumn, "UID");
	_headers.insert(UniqueNameColumn, "Unique Name");
	_headers.insert(TitleColumn, "Title");
	_headers.insert(VisibleColumn, "Visible");
}

SectionContentListModel::~SectionContentListModel()
{
}

void SectionContentListModel::init(ADS_NS::ContainerWidget* cw)
{
#if QT_VERSION >= 0x050000
	beginResetModel();
	_cw = cw;
	_contents = _cw->contents();
	endResetModel();
#else
	_cw = cw;
	_contents = _cw->contents();
	reset();
#endif
}

int SectionContentListModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return _headers.count();
}

QVariant SectionContentListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return _headers.value(section);
	return QVariant();
}

int SectionContentListModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return _contents.count();
}

QVariant SectionContentListModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.row() > rowCount(index) - 1)
		return QVariant();

	const ADS_NS::SectionContent::RefPtr sc = _contents.at(index.row());
	if (sc.isNull())
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
		{
			switch (index.column())
			{
				case UidColumn:
					return sc->uid();
				case UniqueNameColumn:
					return sc->uniqueName();
				case TitleColumn:
					return sc->title();
				case VisibleColumn:
					return _cw->isSectionContentVisible(sc);
			}
		}
	}
	return QVariant();
}

bool SectionContentListModel::removeRows(int row, int count, const QModelIndex& parent)
{
	if (row > rowCount(parent) - 1)
		return false;

	const int first = row;
	const int last = row + count - 1;
	beginRemoveRows(parent, first, last);

	for (int i = last; i >= first; --i)
	{
		const ADS_NS::SectionContent::RefPtr sc = _contents.at(i);
		_cw->removeSectionContent(sc);
		_contents.removeAt(i);
	}

	endRemoveRows();
	return true;
}
