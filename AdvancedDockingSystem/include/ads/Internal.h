#ifndef ADS_INTERNAL_HEADER
#define ADS_INTERNAL_HEADER
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
#include <QSharedPointer>
#include <QWeakPointer>

#include "ads/API.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#else
#include "ads/SectionContent.h"
#endif


ADS_NAMESPACE_BEGIN
class SectionContent;
class SectionTitleWidget;
class SectionContentWidget;


class InternalContentData
{
public:
	typedef QSharedPointer<InternalContentData> RefPtr;
	typedef QWeakPointer<InternalContentData> WeakPtr;

	InternalContentData();
	~InternalContentData();

	QSharedPointer<SectionContent> content;
	SectionTitleWidget* titleWidget;
	SectionContentWidget* contentWidget;
};


class HiddenSectionItem
{
public:
	HiddenSectionItem() :
		preferredSectionId(-1),
		preferredSectionIndex(-1)
	{}

	int preferredSectionId;
	int preferredSectionIndex;
	InternalContentData data;
};


ADS_NAMESPACE_END
#endif
