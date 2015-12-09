#include "ads.h"

#include <QWidget>
#include <QSplitter>

#include "container_widget.h"

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

ADS_NAMESPACE_END