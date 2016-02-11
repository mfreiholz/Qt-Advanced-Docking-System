#include "ads/SectionContent.h"

#include <QLabel>

ADS_NAMESPACE_BEGIN

int SectionContent::NextUid = 1;
QHash<int, SectionContent::WeakPtr> SectionContent::LookupMap;
QHash<QString, SectionContent::WeakPtr> SectionContent::LookupMapByName;

SectionContent::SectionContent(QWidget* title, QWidget* content, const QString& uniqueName) :
	_uid(NextUid++),
	_uniqueName(uniqueName),
	_title(title),
	_content(content)
{
}

SectionContent::~SectionContent()
{
	LookupMap.remove(_uid);
	LookupMapByName.remove(_uniqueName);
	delete _title;
	delete _content;
}

int SectionContent::uid() const
{
	return _uid;
}

QString SectionContent::uniqueName() const
{
	return _uniqueName;
}

QWidget* SectionContent::titleWidget() const
{
	return _title;
}

QWidget* SectionContent::contentWidget() const
{
	return _content;
}

SectionContent::RefPtr SectionContent::newSectionContent(QWidget* title, QWidget* content, const QString& uniqueName)
{
	QSharedPointer<SectionContent> sc(new SectionContent(title, content, uniqueName));
	LookupMap.insert(sc->uid(), sc);
	if (!sc->uniqueName().isEmpty())
		LookupMapByName.insert(sc->uniqueName(), sc);
	return sc;
}

ADS_NAMESPACE_END