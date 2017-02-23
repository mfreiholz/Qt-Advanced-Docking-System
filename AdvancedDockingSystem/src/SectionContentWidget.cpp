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

#include <ads/MainContainerWidget.h>
#include <QBoxLayout>
#include "../include/ads/SectionContentWidget.h"
#include "../include/ads/SectionTitleWidget.h"

namespace ads
{
struct SectionContentWidgetPrivate
{
	CSectionContentWidget* _this;
	int Uid;
	QString UniqueName;
	QPointer<CMainContainerWidget> MainContainerWidget;
	QPointer<QWidget> TitleWidgetContent;
	QPointer<QWidget> ContentWidget;
	// Optional attributes
	QString Title;
	CSectionContentWidget::Flags Flags = CSectionContentWidget::AllFlags;



	/* Note: This method could be a problem in static build environment
	 * since it may begin with 0 for every module which uses ADS.
	 */
	static int GetNextUid()
	{
		static int NextUid = 0;
		return ++NextUid;
	}


	SectionContentWidgetPrivate(CSectionContentWidget* _public)
		: _this(_public),
		  Uid(GetNextUid())
	{

	}
};


CSectionContentWidget::CSectionContentWidget(SectionContent::RefPtr c, QWidget* parent) :
	QFrame(parent),
	d(new SectionContentWidgetPrivate(this)),
	_content(c)
{
	QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addWidget(_content->contentWidget());
	setLayout(l);
}


CSectionContentWidget* CSectionContentWidget::newSectionContent(const QString& uniqueName,
	CMainContainerWidget* container, QWidget* title, QWidget* content)
{
	auto SectionContentNameMap = container->m_SectionContentNameMap;
	auto SectionContentIdMap = container->m_SectionContentIdMap;

	if (uniqueName.isEmpty())
	{
		qFatal("Can not create SectionContent with empty uniqueName");
		return nullptr;
	}
	else if (SectionContentNameMap.contains(uniqueName))
	{
		qFatal("Can not create SectionContent with already used uniqueName");
		return nullptr;
	}
	else if (!container || !title || !content)
	{
		qFatal("Can not create SectionContent with NULL values");
		return nullptr;
	}

	CSectionContentWidget* sc(new CSectionContentWidget());
	sc->d->UniqueName = uniqueName;
	sc->d->MainContainerWidget = container;
	sc->d->TitleWidgetContent = title;
	sc->d->ContentWidget = content;

	container->m_SectionContentWidgetIdMap.insert(sc->uid(), sc);
	container->m_SectionContentWidgetNameMap.insert(sc->uniqueName(), sc);
	return sc;
}

CSectionContentWidget::~CSectionContentWidget()
{
	layout()->removeWidget(_content->contentWidget());
	delete d;
}


int CSectionContentWidget::uid() const
{
	return d->Uid;
}


QString CSectionContentWidget::uniqueName() const
{
	return d->UniqueName;
}


CMainContainerWidget* CSectionContentWidget::containerWidget() const
{
	return d->MainContainerWidget;
}

QWidget* CSectionContentWidget::titleWidgetContent() const
{
	return d->TitleWidgetContent.data();
}


QWidget* CSectionContentWidget::contentWidget() const
{
	return d->ContentWidget;
}

CSectionContentWidget::Flags CSectionContentWidget::flags() const
{
	return d->Flags;
}


QString CSectionContentWidget::visibleTitle() const
{
	return d->Title.isEmpty() ? d->UniqueName : d->Title;
}


QString CSectionContentWidget::title() const
{
	return d->Title;
}

void CSectionContentWidget::setTitle(const QString& title)
{
	d->Title = title;
}

void CSectionContentWidget::setFlags(const Flags f)
{
	d->Flags = f;
}

} // namespace ads
