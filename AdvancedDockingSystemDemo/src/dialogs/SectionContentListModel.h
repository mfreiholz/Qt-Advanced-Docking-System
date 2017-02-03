#ifndef ADS_SECTIONCONTENTMODEL_H
#define ADS_SECTIONCONTENTMODEL_H

#include <QHash>
#include <QList>
#include <QString>
#include <QAbstractTableModel>
#include "../../../AdvancedDockingSystem/include/ads/MainContainerWidget.h"
#include "ads/API.h"
#include "ads/SectionContent.h"
ADS_NAMESPACE_BEGIN
class MainContainerWidget;
ADS_NAMESPACE_END

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
	void init(ADS_NS::MainContainerWidget* cw);

	virtual int columnCount(const QModelIndex &parent) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	virtual int rowCount(const QModelIndex &parent) const;
	virtual QVariant data(const QModelIndex &index, int role) const;

	virtual bool removeRows(int row, int count, const QModelIndex &parent);

private:
	QHash<int, QString> _headers;

	ADS_NS::MainContainerWidget* _cw;
	QList<ADS_NS::SectionContent::RefPtr> _contents;
};

#endif
