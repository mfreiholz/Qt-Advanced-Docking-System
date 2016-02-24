#include "ads/SectionContent.h"

#include <QDebug>
#include <QLabel>

ADS_NAMESPACE_BEGIN

SectionContent::SectionContent() :
	_uid(GetNextUid())
{
}

SectionContent::RefPtr SectionContent::newSectionContent(const QString& uniqueName, ContainerWidget* container, QWidget* title, QWidget* content)
{
	if (uniqueName.isEmpty())
	{
		qFatal("Can not create SectionContent with empty uniqueName");
		return RefPtr();
	}
	else if (GetLookupMapByName().contains(uniqueName))
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
	sc->_containerWidget = container;
	sc->_titleWidget = title;
	sc->_contentWidget = content;

	GetLookupMap().insert(sc->uid(), sc);
	if (!sc->uniqueName().isEmpty())
		GetLookupMapByName().insert(sc->uniqueName(), sc);

	return sc;
}

SectionContent::~SectionContent()
{
	GetLookupMap().remove(_uid);
	GetLookupMapByName().remove(_uniqueName);
	delete _titleWidget;
	delete _contentWidget;
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
	return _containerWidget;
}

QWidget* SectionContent::titleWidget() const
{
	return _titleWidget;
}

QWidget* SectionContent::contentWidget() const
{
	return _contentWidget;
}

QString SectionContent::visibleTitle() const
{
	if (_title.isEmpty())
		return _uniqueName;
	return _title;
}

QString SectionContent::title() const
{
	return _title;
}

void SectionContent::setTitle(const QString& title)
{
	_title = title;
}

int SectionContent::GetNextUid()
{
	static int NextUid = 0;
	return ++NextUid;
}

QHash<int, SectionContent::WeakPtr>& SectionContent::GetLookupMap()
{
	static QHash<int, SectionContent::WeakPtr> LookupMap;
	return LookupMap;
}

QHash<QString, SectionContent::WeakPtr>& SectionContent::GetLookupMapByName()
{
	static QHash<QString, SectionContent::WeakPtr> LookupMapByName;
	return LookupMapByName;
}

ADS_NAMESPACE_END
