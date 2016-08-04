#include "ads/FloatingWidget.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QMouseEvent>
#include <QStyle>

#include "ads/ContainerWidget.h"
#include "ads/SectionTitleWidget.h"
#include "ads/SectionContentWidget.h"
#include "ads/Internal.h"

ADS_NAMESPACE_BEGIN

FloatingWidget::FloatingWidget(ContainerWidget* container, SectionContent::RefPtr sc, SectionTitleWidget* titleWidget, SectionContentWidget* contentWidget, QWidget* parent) :
	QWidget(parent, Qt::CustomizeWindowHint | Qt::Tool),
	_container(container),
	_content(sc),
	_titleWidget(titleWidget),
	_contentWidget(contentWidget)
{
	QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	setLayout(l);

	// Title + Controls
	_titleLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	_titleLayout->addWidget(titleWidget, 1);
	l->addLayout(_titleLayout, 0);
	titleWidget->setActiveTab(false);

	if (sc->flags().testFlag(SectionContent::Closeable))
	{
		QPushButton* closeButton = new QPushButton();
		closeButton->setObjectName("closeButton");
		closeButton->setFlat(true);
		closeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
		closeButton->setToolTip(tr("Close"));
		closeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		_titleLayout->addWidget(closeButton);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
		QObject::connect(closeButton, &QPushButton::clicked, this, &FloatingWidget::onCloseButtonClicked);
#else
		QObject::connect(closeButton, SIGNAL(clicked(bool)), this, SLOT(onCloseButtonClicked()));
#endif
	}

	// Content
	l->addWidget(contentWidget, 1);
	contentWidget->show();

//	_container->_floatingWidgets.append(this);
}

FloatingWidget::~FloatingWidget()
{
	_container->_floatings.removeAll(this); // Note: I don't like this here, but we have to remove it from list...
}

bool FloatingWidget::takeContent(InternalContentData& data)
{
	data.content = _content;
	data.titleWidget = _titleWidget;
	data.contentWidget = _contentWidget;

	_titleLayout->removeWidget(_titleWidget);
	_titleWidget->setParent(_container);
	_titleWidget = NULL;

	layout()->removeWidget(_contentWidget);
	_contentWidget->setParent(_container);
	_contentWidget = NULL;

	return true;
}

void FloatingWidget::onCloseButtonClicked()
{
	_container->hideSectionContent(_content);
}

ADS_NAMESPACE_END
