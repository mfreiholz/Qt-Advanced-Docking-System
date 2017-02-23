#ifndef ADS_SECTIONCONTENTMODEL_H
#define ADS_SECTIONCONTENTMODEL_H

#include <QHash>
#include <QList>
#include <QString>
#include <QAbstractTableModel>
#include "MainContainerWidget.h"
#include "API.h"
#include "SectionContent.h"
namespace ads {class CMainContainerWidget;}


class SectionContentListModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	enum Column
	{
		UidColumn,
		UniqueNameColumn,
		TitleColumn,
		VisibleColumn
	};

	SectionContentListModel(QObject* parent);
	virtual ~SectionContentListModel();
	void init(ads::CMainContainerWidget* cw);

	virtual int columnCount(const QModelIndex &parent) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	virtual int rowCount(const QModelIndex &parent) const;
	virtual QVariant data(const QModelIndex &index, int role) const;

	virtual bool removeRows(int row, int count, const QModelIndex &parent);

private:
	QHash<int, QString> _headers;

	ads::CMainContainerWidget* _cw;
	QList<ads::SectionContent::RefPtr> _contents;
};

#endif
