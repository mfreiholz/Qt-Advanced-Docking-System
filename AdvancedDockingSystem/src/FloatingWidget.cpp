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




FloatingWidget::FloatingWidget(MainContainerWidget* MainContainer, SectionContent::RefPtr sc, SectionTitleWidget* titleWidget, SectionContentWidget* contentWidget, QWidget* parent) :
	QWidget(MainContainer, Qt::Window),
	m_MainContainerWidget(MainContainer)
{
	QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	setLayout(l);
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
	m_MainContainerWidget->m_Floatings.append(this);
}


FloatingWidget::FloatingWidget(MainContainerWidget* MainContainer, SectionWidget* sectionWidget)
	: QWidget(MainContainer, Qt::Window),
	  m_MainContainerWidget(MainContainer)
{
    QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    setLayout(l);

    m_ContainerWidget = new CContainerWidget(m_MainContainerWidget, this);
	m_MainContainerWidget->m_Containers.append(m_ContainerWidget);
	l->addWidget(m_ContainerWidget, 1);

    m_ContainerWidget->addSectionWidget(sectionWidget);
    m_ContainerWidget->show();
    m_zOrderIndex = ++zOrderCounter;
    m_MainContainerWidget->m_Floatings.append(this);
}


FloatingWidget::~FloatingWidget()
{
	std::cout << "FloatingWidget::~FloatingWidget" << std::endl;
	m_MainContainerWidget->m_Floatings.removeAll(this);
}

bool FloatingWidget::takeContent(InternalContentData& data)
{
	// TODO remove takeContent function
	/*data.content = _content;
	data.titleWidget = _titleWidget;
	data.contentWidget = _contentWidget;

	//_titleLayout->removeWidget(_titleWidget);
	_titleWidget->setParent(m_MainContainerWidget);
	_titleWidget = NULL;

	layout()->removeWidget(_contentWidget);
	_contentWidget->setParent(m_MainContainerWidget);
	_contentWidget = NULL;*/

	return true;
}

void FloatingWidget::onCloseButtonClicked()
{
	//m_MainContainerWidget->hideSectionContent(_content);
}


void FloatingWidget::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);
	if (event->type() == QEvent::ActivationChange && isActiveWindow())
    {
		std::cout << "FloatingWidget::changeEvent QEvent::ActivationChange " << std::endl;
		m_zOrderIndex = ++zOrderCounter;
        return;
    }
}


void FloatingWidget::moveEvent(QMoveEvent *event)
{
	QWidget::moveEvent(event);
	if (m_DraggingActive && qApp->mouseButtons().testFlag(Qt::LeftButton))
	{
		//std::cout << "Dragging" << std::endl;
		updateDropOverlays(QCursor::pos());
	}
}


void FloatingWidget::setDraggingActive(bool Active)
{
	if (m_DraggingActive == Active)
	{
		return;
	}

	m_DraggingActive = Active;
	if (Active)
	{
		std::cout << "FloatingWidget:: InstallEventFilter" << std::endl;
		qApp->installEventFilter(this);
	}
	else
	{
		std::cout << "FloatingWidget:: RemoveEventFilter" << std::endl;
		qApp->removeEventFilter(this);
	}
}


bool FloatingWidget::event(QEvent *e)
{
	if ((e->type() == QEvent::NonClientAreaMouseButtonPress))
	{
		if (QGuiApplication::mouseButtons() == Qt::LeftButton)
		{
			std::cout << "FloatingWidget::event Event::NonClientAreaMouseButtonPress" << e->type() << std::endl;
			setDraggingActive(true);
		}
	}
	else if (e->type() == QEvent::NonClientAreaMouseButtonDblClick)
	{
		std::cout << "FloatingWidget::event QEvent::NonClientAreaMouseButtonDblClick" << std::endl;
		setDraggingActive(false);
	}
	else if ((e->type() == QEvent::NonClientAreaMouseButtonRelease) && m_DraggingActive)
	{
		std::cout << "FloatingWidget::event QEvent::NonClientAreaMouseButtonRelease" << std::endl;
		titleMouseReleaseEvent();
	}
	return QWidget::event(e);
}


bool FloatingWidget::eventFilter(QObject *watched, QEvent *event)
{
	if (event->type() == QEvent::MouseButtonRelease)
	{
		std::cout << "FloatingWidget::eventFilter QEvent::MouseButtonRelease" << std::endl;
		titleMouseReleaseEvent();
	}
	else if (event->type() == QEvent::MouseMove)
	{
		if (m_DraggingActive)
		{
			QMouseEvent* MouseEvent = dynamic_cast<QMouseEvent*>(event);
			int BorderSize = (frameSize().width() - size().width()) / 2;
			const QPoint moveToPos = QCursor::pos() - m_DragStartMousePosition - QPoint(BorderSize, 0);
			move(moveToPos);
			return true;
		}
	}
	return false;
}


void FloatingWidget::startFloating(const QPoint& Pos)
{
	setDraggingActive(true);
	QPoint TargetPos = QCursor::pos() - Pos;
	move(TargetPos);
    show();
	m_DragStartMousePosition = Pos;
	m_DragStartPosition = this->pos();
}


void FloatingWidget::titleMouseReleaseEvent()
{
	setDraggingActive(false);
	if (!m_DropContainer)
	{
		return;
	}

	std::cout << "Dropped" << std::endl;
	MainContainerWidget* MainContainerWidget = mainContainerWidget();
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
	if (!isVisible())
	{
		return;
	}
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
    DropOverlay* ContainerDropOverlay = MainContainerWidget->dropOverlay();
    DropOverlay* SectionDropOverlay = MainContainerWidget->sectionDropOverlay();

    if (!TopContainer)
    {
    	ContainerDropOverlay->hideDropOverlay();
    	SectionDropOverlay->hideDropOverlay();
    	return;
    }

	ContainerDropOverlay->showDropOverlay(TopContainer);
	ContainerDropOverlay->raise();

    SectionWidget* sectionwidget = TopContainer->sectionWidgetAt(GlobalPos);
    if (sectionwidget)
    {
    	SectionDropOverlay->setAllowedAreas(ADS_NS::AllAreas);
        SectionDropOverlay->showDropOverlay(sectionwidget);
    }
    else
    {
    	SectionDropOverlay->hideDropOverlay();
    }


    if (TopContainer)
    {
    	ContainerDropOverlay->showDropOverlay(TopContainer);
		ContainerDropOverlay->raise();
    }
    else
    {
    	ContainerDropOverlay->hideDropOverlay();
    }
}


ADS_NAMESPACE_END
