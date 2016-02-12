#ifndef SECTIONCONTENT_H
#define SECTIONCONTENT_H

#include <QPointer>
#include <QWidget>
#include <QHash>
#include <QSharedPointer>
#include <QWeakPointer>

#include "ads/API.h"

ADS_NAMESPACE_BEGIN

class SectionContent
{
	friend class ContainerWidget;

private:
	SectionContent(QWidget* title, QWidget* content, const QString& uniqueName = QString()); ///< Do not use!

public:
	typedef QSharedPointer<SectionContent> RefPtr;
	typedef QWeakPointer<SectionContent> WeakPtr;

	virtual ~SectionContent();
	int uid() const;
	QString uniqueName() const;
	QWidget* titleWidget() const;
	QWidget* contentWidget() const;

	static RefPtr newSectionContent(QWidget* title, QWidget* content, const QString& uniqueName = QString());

private:
	const int _uid;
	const QString _uniqueName;
	QPointer<QWidget> _title;
	QPointer<QWidget> _content;

	static int NextUid;
	static QHash<int, SectionContent::WeakPtr> LookupMap;
	static QHash<QString, SectionContent::WeakPtr> LookupMapByName;
};

ADS_NAMESPACE_END
#endif
