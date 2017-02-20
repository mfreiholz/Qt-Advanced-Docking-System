#ifndef SECTION_CONTENT_WIDGET_H
#define SECTION_CONTENT_WIDGET_H
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
#include <QFrame>

#include "ads/API.h"
#include "ads/SectionContent.h"

ADS_NAMESPACE_BEGIN
class MainContainerWidget;
class SectionWidget;

class SectionContentWidget : public QFrame
{
	Q_OBJECT

	friend class MainContainerWidget;

public:
	SectionContentWidget(SectionContent::RefPtr c, QWidget* parent = 0);
	virtual ~SectionContentWidget();

private:
	SectionContent::RefPtr _content;
};

ADS_NAMESPACE_END
#endif
