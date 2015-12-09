#include "section_widget.h"

#include <QApplication>
#include <QBoxLayout>
#include <QStackedLayout>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QPainter>
#include <QStyle>
#include <QtWidgets/QSplitter>

#if defined(ADS_ANIMATIONS_ENABLED)
#include <QGraphicsDropShadowEffect>
#endif

#include "drop_overlay.h"
#include "section_content.h"
#include "section_title_widget.h"
#include "section_content_widget.h"
#include "floating_widget.h"
#include "container_widget.h"

ADS_NAMESPACE_BEGIN

int SectionWidget::NextUid = 1;
QHash<int, SectionWidget*> SectionWidget::LookupMap;

SectionWidget::SectionWidget(ContainerWidget* parent) :
	QFrame(parent),
	_uid(NextUid++),
	_container(parent)
{
	//setAttribute(Qt::WA_DeleteOnClose, true);

	auto l = new QBoxLayout(QBoxLayout::TopToBottom);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	setLayout(l);

	_tabsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	_tabsLayout->setContentsMargins(0, 0, 0, 0);
	_tabsLayout->setSpacing(0);
	_tabsLayout->addStretch(1);
	l->addLayout(_tabsLayout);

	_contentsLayout = new QStackedLayout();
	_contentsLayout->setContentsMargins(0, 0, 0, 0);
	_contentsLayout->setSpacing(0);
	l->addLayout(_contentsLayout, 1);

#if defined(ADS_ANIMATIONS_ENABLED)
	auto shadow = new QGraphicsDropShadowEffect(this);
	shadow->setOffset(0, 0);
	shadow->setBlurRadius(8);
	setGraphicsEffect(shadow);
#endif

	LookupMap.insert(_uid, this);
	_container->_sections.append(this);
}

SectionWidget::~SectionWidget()
{
	qDebug() << Q_FUNC_INFO;
	LookupMap.remove(_uid);
	_container->_sections.removeAll(this);

	// Delete empty QSplitter.
	auto splitter = findParentSplitter(this);
	if (splitter && splitter->count() == 0)
	{
		splitter->deleteLater();
		qDebug() << "Delete empty splitter";
	}
}

int SectionWidget::uid() const
{
	return _uid;
}

ContainerWidget* SectionWidget::containerWidget() const
{
	return _container;
}

QRect SectionWidget::titleAreaGeometry() const
{
	return _tabsLayout->geometry();
}

QRect SectionWidget::contentAreaGeometry() const
{
	return _contentsLayout->geometry();
}

void SectionWidget::addContent(SectionContent::RefPtr c)
{
	_contents.append(c);

	auto title = new SectionTitleWidget(c, nullptr);
	_sectionTitles.append(title);
	_tabsLayout->insertWidget(_tabsLayout->count() - 1, title);
	QObject::connect(title, &SectionTitleWidget::clicked, this, &SectionWidget::onSectionTitleClicked);

	auto content = new SectionContentWidget(c, nullptr);
	_sectionContents.append(content);
	_contentsLayout->addWidget(content);

	// Active first TAB.
	if (_contents.size() == 1)
		setCurrentIndex(0);
	// Switch to newest.
//	else
//		setCurrentIndex(_contentsLayout->count() - 1);
}

void SectionWidget::addContent(const InternalContentData& data, bool autoActivate)
{
	_contents.append(data.content);

	_sectionTitles.append(data.titleWidget);
	_tabsLayout->insertWidget(_tabsLayout->count() - 1, data.titleWidget);
	QObject::connect(data.titleWidget, &SectionTitleWidget::clicked, this, &SectionWidget::onSectionTitleClicked);

	_sectionContents.append(data.contentWidget);
	_contentsLayout->addWidget(data.contentWidget);

	// Active first TAB.
	if (_contents.size() == 1)
		setCurrentIndex(0);
	// Switch to newest.
	else if (autoActivate)
		setCurrentIndex(_contentsLayout->count() - 1);
}

// take removes a widget from the SectionWidget but does not delete
// the used SectionTitle- and SectionContent-Widget. Instead it returns
// these objects.
InternalContentData SectionWidget::take(int uid, bool del)
{
	InternalContentData data;

	// Find SectionContent.
	SectionContent::RefPtr sc;
	int index = -1;
	for (int i = 0; i < _contents.count(); i++)
	{
		if (_contents[i]->uid() != uid)
			continue;
		index = i;
		sc = _contents.takeAt(i);
		break;
	}

	// Title wrapper widget (TAB)
	auto title = _sectionTitles.takeAt(index);
	if (title)
	{
		_tabsLayout->removeWidget(title);
		title->disconnect(this);
		if (del)
			title->deleteLater();
	}

	// Content wrapper widget (CONTENT)
	auto content = _sectionContents.takeAt(index);
	if (content)
	{
		_contentsLayout->removeWidget(content);
		content->disconnect(this);
		if (del)
			content->deleteLater();
	}

	// Select the previous tab as activeTab.
	if (_contents.size() > 0 && title->isActiveTab())
	{
		if (index > 0)
			setCurrentIndex(index - 1);
		else
			setCurrentIndex(0);
	}

	data.content = sc;
	data.titleWidget = title;
	data.contentWidget = content;
	return data;
}

void SectionWidget::setCurrentIndex(int index)
{
	// Set active TAB.
	for (int i = 0; i < _tabsLayout->count(); ++i)
	{
		auto item = _tabsLayout->itemAt(i);
		if (item->widget())
		{
			auto stw = dynamic_cast<SectionTitleWidget*>(item->widget());
			if (i == index)
				stw->setActiveTab(true);
			else
				stw->setActiveTab(false);
		}
	}

	// Set active CONTENT.
	_contentsLayout->setCurrentIndex(index);
}

void SectionWidget::paintEvent(QPaintEvent* e)
{
	QFrame::paintEvent(e);

//	QPainter p(this);
//	auto r = rect();
//	p.drawText(rect(), Qt::AlignCenter | Qt::AlignVCenter, QString("x=%1; y=%2; w=%3; h=%4").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()));
}

void SectionWidget::onSectionTitleClicked()
{
	auto stw = qobject_cast<SectionTitleWidget*>(sender());
	if (stw)
	{
		auto index = _tabsLayout->indexOf(stw);
		setCurrentIndex(index);
	}
}

ADS_NAMESPACE_END