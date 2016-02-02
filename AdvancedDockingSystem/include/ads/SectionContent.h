#ifndef SECTIONCONTENT_H
#define SECTIONCONTENT_H

#include <QPointer>
#include <QWidget>
#include <QHash>
#include <QSharedPointer>

#include "ads/API.h"

ADS_NAMESPACE_BEGIN

class SectionContent
{
public:
	typedef QSharedPointer<SectionContent> RefPtr;

	SectionContent(QWidget* title, QWidget* content, const QString& uniqueName = QString()); ///< Do not use!
	virtual ~SectionContent();

	int uid() const;
	QString uniqueName() const;
	QWidget* titleWidget() const;
	QWidget* contentWidget() const;

	static RefPtr newSectionContent(QWidget* title, QWidget* content);
	static RefPtr newSectionContent(const QString& title, QWidget* content);

public:
	const int _uid;
	const QString _uniqueName;
	QPointer<QWidget> _title;
	QPointer<QWidget> _content;

	static int NextUid;
	static QHash<int, SectionContent*> LookupMap;
};

ADS_NAMESPACE_END
#endif