#include "ads/SectionContent.h"

#include <QDebug>
#include <QLabel>

ADS_NAMESPACE_BEGIN

int SectionContent::NextUid = 1;
QHash<int, SectionContent::WeakPtr> SectionContent::LookupMap;
QHash<QString, SectionContent::WeakPtr> SectionContent::LookupMapByName;

SectionContent::SectionContent() :
	_uid(NextUid++)
{
}

SectionContent::RefPtr SectionContent::newSectionContent(const QString& uniqueName, ContainerWidget* container, QWidget* title, QWidget* content)
{
	if (uniqueName.isEmpty())
	{
		qFatal("Can not create SectionContent with empty uniqueName");
		return RefPtr();
	}
	else if (LookupMapByName.contains(uniqueName))
	{
		qFatal("Can not create SectionContent with already used uniqueName");
		return RefPtr();
	}
	else if (!container || !title || !content)
	{
		qFatal("Can not create SectionContent with NULL values");
		return RefPtr();
	}

	QSharedPointer<SectionContent> sc(new SectionContent());
	sc->_uniqueName = uniqueName;
	sc->_container = container;
	sc->_title = title;
	sc->_content = content;

	LookupMap.insert(sc->uid(), sc);
	if (!sc->uniqueName().isEmpty())
		LookupMapByName.insert(sc->uniqueName(), sc);

	return sc;
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

ContainerWidget* SectionContent::containerWidget() const
{
	return _container;
}

QWidget* SectionContent::titleWidget() const
{
	return _title;
}

QWidget* SectionContent::contentWidget() const
{
	return _content;
}

ADS_NAMESPACE_END
