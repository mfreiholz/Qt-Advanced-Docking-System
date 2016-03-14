#include "ads/SectionContentWidget.h"

#include <QDebug>
#include <QBoxLayout>

ADS_NAMESPACE_BEGIN

SectionContentWidget::SectionContentWidget(SectionContent::RefPtr c, QWidget* parent) :
	QFrame(parent),
	_content(c)
{
	qDebug() << Q_FUNC_INFO;

	QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addWidget(_content->contentWidget());
	setLayout(l);
}

SectionContentWidget::~SectionContentWidget()
{
	qDebug() << Q_FUNC_INFO;
	layout()->removeWidget(_content->contentWidget());
}

ADS_NAMESPACE_END
