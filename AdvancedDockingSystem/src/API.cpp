#include "ads/API.h"

#include <QWidget>
#include <QSplitter>

#include "ads/ContainerWidget.h"

const QString DragData::MIMETYPE = QString("qt/ads-dragdata");

ADS_NAMESPACE_BEGIN

ContainerWidget* findParentContainerWidget(QWidget* w)
{
	ContainerWidget* cw = 0;
	QWidget* next = w;
	do
	{
		if ((cw = dynamic_cast<ContainerWidget*>(next)) != 0)
		{
			break;
		}
		next = next->parentWidget();
	}
	while (next);
	return cw;
}

SectionWidget* findParentSectionWidget(class QWidget* w)
{
	SectionWidget* cw = 0;
	QWidget* next = w;
	do
	{
		if ((cw = dynamic_cast<SectionWidget*>(next)) != 0)
		{
			break;
		}
		next = next->parentWidget();
	}
	while (next);
	return cw;
}

QSplitter* findParentSplitter(class QWidget* w)
{
	QSplitter* cw = 0;
	QWidget* next = w;
	do
	{
		if ((cw = dynamic_cast<QSplitter*>(next)) != 0)
		{
			break;
		}
		next = next->parentWidget();
	}
	while (next);
	return cw;
}

QSplitter* findImmediateSplitter(class QWidget* w)
{
	QSplitter* sp = NULL;
	QLayout* l = w->layout();
	if (!l || l->count() <= 0)
		return sp;
	for (int i = 0; i < l->count(); ++i)
	{
		QLayoutItem* li = l->itemAt(0);
		if (!li->widget())
			continue;
		if ((sp = dynamic_cast<QSplitter*>(li->widget())) != NULL)
			break;
	}
	return sp;
}

ADS_NAMESPACE_END