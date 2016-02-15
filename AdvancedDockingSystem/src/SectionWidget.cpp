#include "ads/SectionWidget.h"

#include <QApplication>
#include <QBoxLayout>
#include <QStackedLayout>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QPainter>
#include <QStyle>
#include <QSplitter>

#if defined(ADS_ANIMATIONS_ENABLED)
#include <QGraphicsDropShadowEffect>
#endif

#include "ads/Internal.h"
#include "ads/DropOverlay.h"
#include "ads/SectionContent.h"
#include "ads/SectionTitleWidget.h"
#include "ads/SectionContentWidget.h"
#include "ads/FloatingWidget.h"
#include "ads/ContainerWidget.h"

ADS_NAMESPACE_BEGIN

int SectionWidget::NextUid = 1;
QHash<int, SectionWidget*> SectionWidget::LookupMap;

SectionWidget::SectionWidget(ContainerWidget* parent) :
	QFrame(parent),
	_uid(NextUid++),
	_container(parent)
{
	QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
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
	QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
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
	QSplitter* splitter = findParentSplitter(this);
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

	SectionTitleWidget* title = new SectionTitleWidget(c, NULL);
	_sectionTitles.append(title);
	_tabsLayout->insertWidget(_tabsLayout->count() - 1, title);
#if QT_VERSION >= 0x050000
	QObject::connect(title, &SectionTitleWidget::clicked, this, &SectionWidget::onSectionTitleClicked);
#else
	QObject::connect(title, SIGNAL(clicked()), this, SLOT(onSectionTitleClicked()));
#endif

	SectionContentWidget* content = new SectionContentWidget(c, NULL);
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
#if QT_VERSION >= 0x050000
	QObject::connect(data.titleWidget, &SectionTitleWidget::clicked, this, &SectionWidget::onSectionTitleClicked);
#else
	QObject::connect(data.titleWidget, SIGNAL(clicked()), this, SLOT(onSectionTitleClicked()));
#endif

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
bool SectionWidget::takeContent(int uid, InternalContentData& data)
{
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
	if (!sc)
		return false;

	// Title wrapper widget (TAB)
	SectionTitleWidget* title = _sectionTitles.takeAt(index);
	if (title)
	{
		_tabsLayout->removeWidget(title);
		title->disconnect(this);
//		if (del)
//			title->deleteLater();
	}

	// Content wrapper widget (CONTENT)
	SectionContentWidget* content = _sectionContents.takeAt(index);
	if (content)
	{
		_contentsLayout->removeWidget(content);
		content->disconnect(this);
//		if (del)
//			content->deleteLater();
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
	return !data.content.isNull();
}

int SectionWidget::indexOfContent(SectionContent::RefPtr c) const
{
	return _contents.indexOf(c);
}

int SectionWidget::indexOfContentByTitlePos(const QPoint& p, QWidget* exclude) const
{
	int index = -1;
	for (int i = 0; i < _sectionTitles.size(); ++i)
	{
		if (_sectionTitles[i]->geometry().contains(p) && (exclude == NULL || _sectionTitles[i] != exclude))
		{
			index = i;
			break;
		}
	}
	return index;
}

int SectionWidget::currentIndex() const
{
	return _contentsLayout->currentIndex();
}

void SectionWidget::moveContent(int from, int to)
{
	if (from >= _contents.size() || from < 0 || to >= _contents.size() || to < 0 || from == to)
	{
		qCritical() << "Invalid index for tab movement" << from << to;
		return;
	}

	SectionContent::RefPtr sc = _contents.at(from);
	_contents.move(from, to);
	_sectionTitles.move(from, to);
	_sectionContents.move(from, to);

	QLayoutItem* liFrom = NULL;

	liFrom = _tabsLayout->takeAt(from);
#if QT_VERSION >= 0x050000
	_tabsLayout->insertItem(to, liFrom);
#else
	_tabsLayout->insertWidget(to, liFrom->widget());
	delete liFrom;
	liFrom = NULL;
#endif

	liFrom = _contentsLayout->takeAt(from);
	_contentsLayout->insertWidget(to, liFrom->widget());
	delete liFrom;
}

void SectionWidget::setCurrentIndex(int index)
{
	// Set active TAB.
	qDebug() << Q_FUNC_INFO << index;
	for (int i = 0; i < _tabsLayout->count(); ++i)
	{
		QLayoutItem* item = _tabsLayout->itemAt(i);
		if (item->widget())
		{
			SectionTitleWidget* stw = dynamic_cast<SectionTitleWidget*>(item->widget());
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
	SectionTitleWidget* stw = qobject_cast<SectionTitleWidget*>(sender());
	if (stw)
	{
		int index = _tabsLayout->indexOf(stw);
		setCurrentIndex(index);
	}
}

ADS_NAMESPACE_END
