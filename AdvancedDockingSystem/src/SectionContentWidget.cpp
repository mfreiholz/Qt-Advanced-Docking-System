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

#include "ads/SectionContentWidget.h"

#include <QBoxLayout>

ADS_NAMESPACE_BEGIN

SectionContentWidget::SectionContentWidget(SectionContent::RefPtr c, QWidget* parent) :
	QFrame(parent),
	_content(c)
{
	QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addWidget(_content->contentWidget());
	setLayout(l);
}

SectionContentWidget::~SectionContentWidget()
{
	layout()->removeWidget(_content->contentWidget());
}

ADS_NAMESPACE_END
