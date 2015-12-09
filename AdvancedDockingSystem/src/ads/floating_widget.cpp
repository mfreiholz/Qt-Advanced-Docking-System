#include "floating_widget.h"

#include <QDebug>
#include <QBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QMouseEvent>
#include <QStyle>

#include "section_title_widget.h"
#include "section_content_widget.h"

ADS_NAMESPACE_BEGIN

FloatingWidget::FloatingWidget(SectionContent::RefPtr sc, SectionTitleWidget* titleWidget, SectionContentWidget* contentWidget, QWidget* parent) :
	QWidget(parent, Qt::CustomizeWindowHint | Qt::Tool),
	_content(sc),
	_titleWidget(titleWidget),
	_contentWidget(contentWidget)
{
	auto l = new QBoxLayout(QBoxLayout::TopToBottom);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	setLayout(l);

	// Title + Controls
	_titleLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	_titleLayout->addWidget(titleWidget, 1);

	auto maximizeButton = new QPushButton();
	maximizeButton->setObjectName("maximizeButton");
	maximizeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
	maximizeButton->setToolTip(tr("Maximize"));
	maximizeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_titleLayout->addWidget(maximizeButton);

	auto closeButton = new QPushButton();
	closeButton->setObjectName("closeButton");
	closeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	closeButton->setToolTip(tr("Close"));
	closeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_titleLayout->addWidget(closeButton);
	QObject::connect(closeButton, &QPushButton::clicked, this, &FloatingWidget::close);

	l->addLayout(_titleLayout, 0);

	// Content
	l->addWidget(contentWidget, 1);
	contentWidget->show();
}

InternalContentData FloatingWidget::takeContent()
{
	InternalContentData data;
	data.content = _content;
	data.titleWidget = _titleWidget;
	data.contentWidget = _contentWidget;

	_titleLayout->removeWidget(_titleWidget);
	_titleWidget = NULL;

	layout()->removeWidget(_contentWidget);
	_contentWidget = NULL;

	return data;
}

void FloatingWidget::closeEvent(QCloseEvent*)
{
}

ADS_NAMESPACE_END