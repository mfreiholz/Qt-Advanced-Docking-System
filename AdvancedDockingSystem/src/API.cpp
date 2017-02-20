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
#include "ads/API.h"

#include <QWidget>
#include <QSplitter>
#include <QLayout>
#include <QVariant>

#include "ads/SectionWidget.h"

ADS_NAMESPACE_BEGIN

static bool splitterContainsSectionWidget(QSplitter* splitter)
{
	for (int i = 0; i < splitter->count(); ++i)
	{
		QWidget* w = splitter->widget(i);
		QSplitter* sp = qobject_cast<QSplitter*>(w);
		SectionWidget* sw = NULL;
		if (sp && splitterContainsSectionWidget(sp))
			return true;
		else if ((sw = qobject_cast<SectionWidget*>(w)) != NULL)
			return true;
	}
	return false;
}

void deleteEmptySplitter(MainContainerWidget* container)
{
	bool doAgain = false;
	do
	{
		doAgain = false;
		QList<QSplitter*> splitters = container->findChildren<QSplitter*>();
		for (int i = 0; i < splitters.count(); ++i)
		{
			QSplitter* sp = splitters.at(i);
			if (!sp->property("ads-splitter").toBool())
				continue;
			if (sp->count() > 0 && splitterContainsSectionWidget(sp))
				continue;
			delete splitters[i];
			doAgain = true;
			break;
		}
	}
	while (doAgain);
}


QSplitter* findParentSplitter(QWidget* w)
{
	QSplitter* splitter = 0;
	QWidget* parentWidget = w;
	do
	{
		if ((splitter = dynamic_cast<QSplitter*>(parentWidget)) != 0)
		{
			break;
		}
		parentWidget = parentWidget->parentWidget();
	}
	while (parentWidget);
	return splitter;
}


QSplitter* findImmediateSplitter(QWidget* w)
{
	QLayout* l = w->layout();
	if (!l || l->count() <= 0)
	{
		return nullptr;
	}

	QSplitter* sp = nullptr;
	for (int i = 0; i < l->count(); ++i)
	{
		QLayoutItem* li = l->itemAt(0);
		if (!li->widget())
		{
			continue;
		}
		if ((sp = dynamic_cast<QSplitter*>(li->widget())) != nullptr)
		{
			break;
		}
	}
	return sp;
}

ADS_NAMESPACE_END
