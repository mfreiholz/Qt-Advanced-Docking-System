#include <ads/ContainerWidget.h>
#include "ads/FloatingWidget.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QMouseEvent>
#include <QStyle>

#include "ads/SectionTitleWidget.h"
#include "ads/SectionContentWidget.h"
#include "ads/Internal.h"
#include "ads/SectionWidget.h"

ADS_NAMESPACE_BEGIN

unsigned int FloatingWidget::zOrderCounter = 0;

FloatingWidget::FloatingWidget(MainContainerWidget* container, SectionContent::RefPtr sc, SectionTitleWidget* titleWidget, SectionContentWidget* contentWidget, QWidget* parent) :
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
		connect(closeButton, SIGNAL(clicked(bool)), this, SLOT(onCloseButtonClicked()));
	}
	l->addWidget(contentWidget, 1);
	contentWidget->show();

	m_zOrderIndex = ++zOrderCounter;
}


FloatingWidget::FloatingWidget(SectionWidget* sectionWidget)
{
    QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    setLayout(l);

    l->addWidget(sectionWidget);
    m_zOrderIndex = ++zOrderCounter;
}


FloatingWidget::~FloatingWidget()
{
	// maybe we can implement this this via connection to destroyed signal
	_container->m_Floatings.removeAll(this); // Note: I don't like this here, but we have to remove it from list...
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

bool FloatingWidget::isDraggingActive() const
{
	return _titleWidget->isDraggingFloatingWidget();
}


void FloatingWidget::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);
	if (event->type() != QEvent::ActivationChange)
    {
        return;
    }

    if (isActiveWindow())
    {
        m_zOrderIndex = ++zOrderCounter;
    }
}


unsigned int FloatingWidget::zOrderIndex() const
{
	return m_zOrderIndex;
}


ADS_NAMESPACE_END
