/*******************************************************************************
** QtAdcancedDockingSystem
** Copyright (C) 2017 Uwe Kindler
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program. If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "MainContainerWidget.h"
#include "SectionContent.h"

#include <QWidget>
#include <QLabel>

#include "Internal.h"

namespace ads
{

SectionContent::SectionContent() :
	_uid(GetNextUid()),
	_flags(AllFlags)
{
}

SectionContent::RefPtr SectionContent::newSectionContent(const QString& uniqueName, CMainContainerWidget* container, QWidget* title, QWidget* content)
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
	sc->m_MainContainerWidget = container;
	sc->m_TitleWidgetContent = title;
	sc->m_ContentWidget = content;

	SectionContentIdMap.insert(sc->uid(), sc);
	SectionContentNameMap.insert(sc->uniqueName(), sc);
	return sc;
}

SectionContent::~SectionContent()
{
	auto SectionContentNameMap = m_MainContainerWidget->m_SectionContentNameMap;
	auto SectionContentIdMap = m_MainContainerWidget->m_SectionContentIdMap;

	if (m_MainContainerWidget)
	{
		SectionContentIdMap.remove(_uid);
		SectionContentNameMap.remove(_uniqueName);
	}
	delete m_TitleWidgetContent;
	delete m_ContentWidget;
}

int SectionContent::uid() const
{
	return _uid;
}

QString SectionContent::uniqueName() const
{
	return _uniqueName;
}

CMainContainerWidget* SectionContent::containerWidget() const
{
	return m_MainContainerWidget;
}

QWidget* SectionContent::titleWidgetContent() const
{
	return m_TitleWidgetContent;
}

QWidget* SectionContent::contentWidget() const
{
	return m_ContentWidget;
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

} // namespace ads
