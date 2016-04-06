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
