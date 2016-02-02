#include "ads/SectionContent.h"

#include <QLabel>

ADS_NAMESPACE_BEGIN

int SectionContent::NextUid = 1;
QHash<int, SectionContent*> SectionContent::LookupMap;

SectionContent::SectionContent(QWidget* title, QWidget* content, const QString& uniqueName) :
	_uid(NextUid++),
	_uniqueName(uniqueName)
{
	_title = title;
	_content = content;
	LookupMap.insert(_uid, this);
}

SectionContent::~SectionContent()
{
	LookupMap.remove(_uid);
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

SectionContent::RefPtr SectionContent::newSectionContent(QWidget* title, QWidget* content)
{
	return QSharedPointer<SectionContent>(new SectionContent(title, content));
}

SectionContent::RefPtr SectionContent::newSectionContent(const QString& title, QWidget* content)
{
	return QSharedPointer<SectionContent>(new SectionContent(new QLabel(title), content));
}

ADS_NAMESPACE_END