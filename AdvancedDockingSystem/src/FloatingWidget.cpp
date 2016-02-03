
#include "ads/FloatingWidget.h"

#include <QDebug>
#include <QBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QMouseEvent>
#include <QStyle>

#include "ads/ContainerWidget.h"
#include "ads/SectionTitleWidget.h"
#include "ads/SectionContentWidget.h"

ADS_NAMESPACE_BEGIN

FloatingWidget::FloatingWidget(ContainerWidget* container, SectionContent::RefPtr sc, SectionTitleWidget* titleWidget, SectionContentWidget* contentWidget, QWidget* parent) :
	QWidget(parent, Qt::CustomizeWindowHint | Qt::Tool),
	_container(container),
	_content(sc),
	_titleWidget(titleWidget),
	_contentWidget(contentWidget)
{
	auto l = new QBoxLayout(QBoxLayout::TopToBottom, this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	setLayout(l);

	// Title + Controls
	_titleLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
	_titleLayout->addWidget(titleWidget, 1);
	l->addLayout(_titleLayout, 0);

//	auto maximizeButton = new QPushButton();
//	maximizeButton->setObjectName("maximizeButton");
//	maximizeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
//	maximizeButton->setToolTip(tr("Maximize"));
//	maximizeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//	_titleLayout->addWidget(maximizeButton);

	auto closeButton = new QPushButton();
	closeButton->setObjectName("closeButton");
	closeButton->setFlat(true);
	closeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	closeButton->setToolTip(tr("Close"));
	closeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_titleLayout->addWidget(closeButton);
	QObject::connect(closeButton, &QPushButton::clicked, this, &FloatingWidget::close);

	// Content
	l->addWidget(contentWidget, 1);
	contentWidget->show();

	_container->_floatingWidgets.append(this);
}

FloatingWidget::~FloatingWidget()
{
	qDebug() << Q_FUNC_INFO;
	_container->_floatingWidgets.removeAll(this);
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
