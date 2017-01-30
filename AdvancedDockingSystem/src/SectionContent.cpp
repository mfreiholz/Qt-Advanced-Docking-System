#include <ads/ContainerWidget.h>
#include "ads/SectionContent.h"

#include <QWidget>
#include <QLabel>

#include "ads/Internal.h"

ADS_NAMESPACE_BEGIN

SectionContent::SectionContent() :
	_uid(GetNextUid()),
	_flags(AllFlags)
{
}

SectionContent::RefPtr SectionContent::newSectionContent(const QString& uniqueName, MainContainerWidget* container, QWidget* title, QWidget* content)
{
	auto SectionContentNameMap = container->m_SectionContentNameMap;
	auto SectionContentIdMap = container->m_SectionContentIdMap;

	if (uniqueName.isEmpty())
	{
		qFatal("Can not create SectionContent with empty uniqueName");
		return RefPtr();
	}
	else if (SectionContentNameMap.contains(uniqueName))
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

	SectionContentIdMap.insert(sc->uid(), sc);
	SectionContentNameMap.insert(sc->uniqueName(), sc);
	return sc;
}

SectionContent::~SectionContent()
{
	auto SectionContentNameMap = _containerWidget->m_SectionContentNameMap;
	auto SectionContentIdMap = _containerWidget->m_SectionContentIdMap;

	if (_containerWidget)
	{
		SectionContentIdMap.remove(_uid);
		SectionContentNameMap.remove(_uniqueName);
	}
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

MainContainerWidget* SectionContent::containerWidget() const
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

SectionContent::Flags SectionContent::flags() const
{
	return _flags;
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

void SectionContent::setFlags(const Flags f)
{
	_flags = f;
}

int SectionContent::GetNextUid()
{
	static int NextUid = 0;
	return ++NextUid;
}

ADS_NAMESPACE_END
