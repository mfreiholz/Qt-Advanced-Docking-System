#include <ads/MainContainerWidget.h>
#include "ads/FloatingWidget.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QMouseEvent>
#include <QStyle>
#include <QLabel>
#include <QGuiApplication>

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
	Layout->setSpacing(0);
	setLayout(Layout);

	if (Flags.testFlag(SectionContent::Maximizable))
	{
		QPushButton* Button = new QPushButton();
		Button->setObjectName("maximizeButton");
		Button->setFlat(true);
		Button->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
		Button->setToolTip(tr("Close"));
		Button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		Layout->addWidget(Button);
		connect(Button, SIGNAL(clicked(bool)), this, SLOT(onMaximizeButtonClicked()));
	}

	if (Flags.testFlag(SectionContent::Closeable))
	{
		QPushButton* closeButton = new QPushButton();
		closeButton->setObjectName("closeButton");
		closeButton->setFlat(true);
		closeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
		closeButton->setToolTip(tr("Maximize"));
		closeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		Layout->addWidget(closeButton);
		connect(closeButton, SIGNAL(clicked(bool)), this, SIGNAL(closeButtonClicked()));
	}

	if (Flags == 0)
	{
		Layout->setContentsMargins(6, 6, 6, 6);
	}
	else
	{
		Layout->setContentsMargins(6, 0, 0, 0);
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
        m_DragStartPosition = floatingWidget()->pos();
        m_DragStartMousePosition = ev->globalPos();
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

    floatingWidget()->updateDropOverlays(ev->globalPos());
    ev->accept();
    moveFloatingWidget(ev);
}


void CFloatingTitleWidget::moveFloatingWidget(QMouseEvent* ev)
{
    const QPoint DragDistance = ev->globalPos() - m_DragStartMousePosition;
	const QPoint moveToPos = m_DragStartPosition + DragDistance;
    floatingWidget()->move(moveToPos);
}


void CFloatingTitleWidget::onMaximizeButtonClicked()
{
	if (floatingWidget()->isMaximized())
	{
		floatingWidget()->showNormal();
	}
	else
	{
		floatingWidget()->showMaximized();
	}
}




FloatingWidget::FloatingWidget(MainContainerWidget* container, SectionContent::RefPtr sc, SectionTitleWidget* titleWidget, SectionContentWidget* contentWidget, QWidget* parent) :
	QWidget(parent, Qt::Window/*Qt::CustomizeWindowHint | Qt::Tool*/),
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
	/*CFloatingTitleWidget* TitleBar = new CFloatingTitleWidget(sc->flags(), this);
	l->insertWidget(0, TitleBar);
	connect(TitleBar, SIGNAL(closeButtonClicked()), this, SLOT(onCloseButtonClicked()));*/

	m_ContainerWidget = new CContainerWidget(m_MainContainerWidget, this);
	m_MainContainerWidget->m_Containers.append(m_ContainerWidget);
	l->addWidget(m_ContainerWidget, 1);
	InternalContentData data;
	data.content = sc;
	data.contentWidget = contentWidget;
	data.titleWidget = titleWidget;
	m_ContainerWidget->dropContent(data, nullptr, CenterDropArea);
	m_ContainerWidget->show();

	m_zOrderIndex = ++zOrderCounter;
	container->m_Floatings.append(this);
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
	std::cout << "FloatingWidget::~FloatingWidget" << std::endl;
	m_MainContainerWidget->m_Floatings.removeAll(this);
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


void FloatingWidget::moveEvent(QMoveEvent *event)
{
	QWidget::moveEvent(event);
	if (m_DraggingActive)
	{
		std::cout << "Dragging" << std::endl;
		updateDropOverlays(QCursor::pos());
	}
}


bool FloatingWidget::event(QEvent *e)
{
	//std::cout << "FloatingWidget::event " << e->type() << std::endl;
	if ((e->type() == QEvent::NonClientAreaMouseButtonPress))
	{
		if (QGuiApplication::mouseButtons() == Qt::LeftButton)
		{
			m_DraggingActive = true;
			qApp->installEventFilter(this);
		}
	}
	else if ((e->type() == QEvent::NonClientAreaMouseButtonRelease) && m_DraggingActive)
	{
		titleMouseReleaseEvent();
	}
	else if (e->type() == QEvent::WindowActivate)
	{
		m_DraggingActive = true;
		qApp->installEventFilter(this);
		std::cout << "QEvent::WindowActivate MouseButtons " << QGuiApplication::mouseButtons() << std::endl;
	}
	return QWidget::event(e);
}


bool FloatingWidget::eventFilter(QObject *watched, QEvent *event)
{
	if (event->type() == QEvent::MouseButtonRelease)
	{
		titleMouseReleaseEvent();
	}
	return false;
}


void FloatingWidget::titleMouseReleaseEvent()
{
	qApp->removeEventFilter(this);
	m_DraggingActive = false;
	if (!m_DropContainer)
	{
		return;
	}

	std::cout << "Dropped" << std::endl;
	MainContainerWidget* MainContainerWidget = mainContainerWidget();
	//MainContainerWidget->dropFloatingWidget(this, QCursor::pos());
	m_DropContainer->dropFloatingWidget(this, QCursor::pos());
	MainContainerWidget->dropOverlay()->hideDropOverlay();
	MainContainerWidget->sectionDropOverlay()->hideDropOverlay();
}


unsigned int FloatingWidget::zOrderIndex() const
{
	return m_zOrderIndex;
}


void FloatingWidget::updateDropOverlays(const QPoint& GlobalPos)
{
    MainContainerWidget* MainContainerWidget = mainContainerWidget();
    auto Containers = MainContainerWidget->m_Containers;
    CContainerWidget* TopContainer = nullptr;
    for (auto ContainerWidget : Containers)
    {
    	if (!ContainerWidget->isVisible())
    	{
    		continue;
    	}

    	if (containerWidget() == ContainerWidget)
    	{
    		continue;
    	}

    	QPoint MappedPos = ContainerWidget->mapFromGlobal(GlobalPos);
    	if (ContainerWidget->rect().contains(MappedPos))
    	{
    		std::cout << "Container " <<  ContainerWidget << " contains mousepos" << std::endl;
    		if (!TopContainer || ContainerWidget->isInFrontOf(TopContainer))
    		{
    			TopContainer = ContainerWidget;
    		}
    	}
    }

    m_DropContainer = TopContainer;
    if (TopContainer)
    {
    	MainContainerWidget->dropOverlay()->showDropOverlay(TopContainer);
		MainContainerWidget->dropOverlay()->raise();
    }
    else
    {
    	MainContainerWidget->dropOverlay()->hideDropOverlay();
    }
}


ADS_NAMESPACE_END
