#include <ads/MainContainerWidget.h>
#include "ads/FloatingWidget.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QMouseEvent>
#include <QStyle>
#include <QLabel>

#include "ads/SectionTitleWidget.h"
#include "ads/SectionContentWidget.h"
#include "ads/Internal.h"
#include "ads/SectionWidget.h"
#include "ads/ContainerWidget.h"

#include <iostream>

ADS_NAMESPACE_BEGIN

unsigned int FloatingWidget::zOrderCounter = 0;



CFloatingTitleWidget::CFloatingTitleWidget(SectionContent::Flags Flags, FloatingWidget* Parent)
	: QFrame(Parent)
{
	auto Layout = new QHBoxLayout();
	QLabel* Label = new QLabel(this);
	Label->setText("Floating Widget");
	Label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	Label->setAlignment(Qt::AlignLeft);
	Layout->addWidget(Label, 1, Qt::AlignLeft | Qt::AlignVCenter);
	setLayout(Layout);

	if (Flags.testFlag(SectionContent::Closeable))
	{
		QPushButton* closeButton = new QPushButton();
		closeButton->setObjectName("closeButton");
		closeButton->setFlat(true);
		closeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
		closeButton->setToolTip(tr("Close"));
		closeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		Layout->addWidget(closeButton);
		connect(closeButton, SIGNAL(clicked(bool)), this, SIGNAL(closeButtonClicked()));
		Layout->setContentsMargins(6, 0, 0, 0);
	}
	else
	{
		Layout->setContentsMargins(6, 6, 6, 6);
	}
}

FloatingWidget* CFloatingTitleWidget::floatingWidget() const
{
	return dynamic_cast<FloatingWidget*>(parentWidget());
}

MainContainerWidget* CFloatingTitleWidget::mainContainerWidget() const
{
	return floatingWidget()->mainContainerWidget();
}


void CFloatingTitleWidget::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		ev->accept();
        //m_DragStartPosition = ev->pos();
		return;
	}
	QFrame::mousePressEvent(ev);
}


void CFloatingTitleWidget::mouseReleaseEvent(QMouseEvent* ev)
{

}


void CFloatingTitleWidget::mouseMoveEvent(QMouseEvent* ev)
{
    if (!(ev->buttons() & Qt::LeftButton))
    {
        QFrame::mouseMoveEvent(ev);
        return;
    }

    QPoint Pos = QCursor::pos();
    // TODO make a member with the main container widget and assign it on
    // creation
    MainContainerWidget* MainContainerWidget = mainContainerWidget();
    auto Containers = MainContainerWidget->m_Containers;
    CContainerWidget* TopContainer = nullptr;
    for (auto ContainerWidget : Containers)
    {
    	if (!ContainerWidget->isVisible())
    	{
    		continue;
    	}

    	if (floatingWidget()->containerWidget() == ContainerWidget)
    	{
    		continue;
    	}

    	QPoint MappedPos = ContainerWidget->mapFromGlobal(Pos);
    	if (ContainerWidget->rect().contains(MappedPos))
    	{
    		std::cout << "Container " <<  ContainerWidget << " contains maousepos" << std::endl;
    		if (!TopContainer || ContainerWidget->isInFrontOf(TopContainer))
    		{
    			TopContainer = ContainerWidget;
    		}
    	}
    }

    if (TopContainer)
    {
    	MainContainerWidget->dropOverlay()->showDropOverlay(TopContainer);
		MainContainerWidget->dropOverlay()->raise();
    }
    else
    {
    	MainContainerWidget->dropOverlay()->hideDropOverlay();
    }

    ev->accept();
    moveFloatingWidget(ev, MainContainerWidget);
}


void CFloatingTitleWidget::moveFloatingWidget(QMouseEvent* ev, MainContainerWidget* cw)
{
    const QPoint moveToPos = ev->globalPos() - (m_DragStartPosition + QPoint(ADS_WINDOW_FRAME_BORDER_WIDTH, ADS_WINDOW_FRAME_BORDER_WIDTH));
    floatingWidget()->move(moveToPos);
   // cw->moveFloatingWidget(moveToPos);

}



FloatingWidget::FloatingWidget(MainContainerWidget* container, SectionContent::RefPtr sc, SectionTitleWidget* titleWidget, SectionContentWidget* contentWidget, QWidget* parent) :
	QWidget(parent, Qt::CustomizeWindowHint | Qt::Tool),
	m_MainContainerWidget(container),
	_content(sc),
	_titleWidget(titleWidget),
	_contentWidget(contentWidget)
{
	QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	setLayout(l);

	// Title + Controls
	CFloatingTitleWidget* TitleBar = new CFloatingTitleWidget(sc->flags(), this);
	l->insertWidget(0, TitleBar);
	connect(TitleBar, SIGNAL(closeButtonClicked()), this, SLOT(onCloseButtonClicked()));

	//l->addWidget(contentWidget, 1);
	//contentWidget->show();

	m_ContainerWidget = new CContainerWidget(m_MainContainerWidget, this);
	l->addWidget(m_ContainerWidget, 1);
	InternalContentData data;
	data.content = sc;
	data.contentWidget = contentWidget;
	data.titleWidget = titleWidget;
	m_ContainerWidget->dropContent(data, nullptr, CenterDropArea);
	m_ContainerWidget->show();

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
	m_MainContainerWidget->m_Floatings.removeAll(this); // Note: I don't like this here, but we have to remove it from list...
}

bool FloatingWidget::takeContent(InternalContentData& data)
{
	data.content = _content;
	data.titleWidget = _titleWidget;
	data.contentWidget = _contentWidget;

	//_titleLayout->removeWidget(_titleWidget);
	_titleWidget->setParent(m_MainContainerWidget);
	_titleWidget = NULL;

	layout()->removeWidget(_contentWidget);
	_contentWidget->setParent(m_MainContainerWidget);
	_contentWidget = NULL;

	return true;
}

void FloatingWidget::onCloseButtonClicked()
{
	m_MainContainerWidget->hideSectionContent(_content);
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
