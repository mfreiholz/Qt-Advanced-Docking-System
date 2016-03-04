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
#include <QPushButton>

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

//int SectionWidget::NextUid = 1;
//QHash<int, SectionWidget*> SectionWidget::LookupMap;
//QHash<ContainerWidget*, QHash<int, SectionWidget*> > SectionWidget::LookupMapByContainer;

SectionWidget::SectionWidget(ContainerWidget* parent) :
	QFrame(parent),
	_uid(GetNextUid()),
	_container(parent),
	_tabsLayout(NULL),
	_contentsLayout(NULL),
	_mousePressTitleWidget(NULL)
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

	QPushButton* closeButton = new QPushButton();
	closeButton->setObjectName("closeButton");
	closeButton->setFlat(true);
	closeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	closeButton->setToolTip(tr("Close"));
	closeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_tabsLayout->addWidget(closeButton);
#if QT_VERSION >= 0x050000
	QObject::connect(closeButton, &QPushButton::clicked, this, &SectionWidget::onCloseButtonClicked);
#else
	QObject::connect(closeButton, SIGNAL(clicked(bool)), this, SLOT(onCloseButtonClicked()));
#endif

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

	SWLookupMapById(_container).insert(_uid, this);
}

SectionWidget::~SectionWidget()
{
	qDebug() << Q_FUNC_INFO;
	if (_container)
	{
		SWLookupMapById(_container).remove(_uid);
		_container->_sections.removeAll(this); // Note: I don't like this here, but we have to remove it from list...
	}

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

void SectionWidget::addContent(const SectionContent::RefPtr& c)
{
	_contents.append(c);

	SectionTitleWidget* title = new SectionTitleWidget(c, NULL);
	_sectionTitles.append(title);
	_tabsLayout->insertWidget(_tabsLayout->count() - 2, title);
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

	// Add title-widget to tab-bar
	// #FIX: Make it visible, since it is possible that it was hidden previously.
	_sectionTitles.append(data.titleWidget);
	_tabsLayout->insertWidget(_tabsLayout->count() - 2, data.titleWidget);
	data.titleWidget->setVisible(true);
#if QT_VERSION >= 0x050000
	QObject::connect(data.titleWidget, &SectionTitleWidget::clicked, this, &SectionWidget::onSectionTitleClicked);
#else
	QObject::connect(data.titleWidget, SIGNAL(clicked()), this, SLOT(onSectionTitleClicked()));
#endif

	// Add content-widget to stack.
	// Visibility is managed by QStackedWidget.
	_sectionContents.append(data.contentWidget);
	_contentsLayout->addWidget(data.contentWidget);

	// Activate first TAB.
	if (_contents.size() == 1)
		setCurrentIndex(0);
	// Switch to just added TAB.
	else if (autoActivate)
		setCurrentIndex(_contents.count() - 1);
	// Mark it as inactive tab.
	else
		data.titleWidget->setActiveTab(false); // or: setCurrentIndex(currentIndex())
}

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
		title->setParent(_container);
	}

	// Content wrapper widget (CONTENT)
	SectionContentWidget* content = _sectionContents.takeAt(index);
	if (content)
	{
		_contentsLayout->removeWidget(content);
		content->disconnect(this);
		content->setParent(_container);
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

int SectionWidget::indexOfContent(const SectionContent::RefPtr& c) const
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
	if (index < 0 || index > _contents.count() - 1)
	{
		qWarning() << Q_FUNC_INFO << "Invalid index" << index;
		return;
	}
	qDebug() << Q_FUNC_INFO << index << QString("section=%1; content=%2").arg(_uid).arg(_contents.at(index)->uniqueName());

	// Set active TAB
	for (int i = 0; i < _tabsLayout->count(); ++i)
	{
		QLayoutItem* item = _tabsLayout->itemAt(i);
		if (item->widget())
		{
			SectionTitleWidget* stw = dynamic_cast<SectionTitleWidget*>(item->widget());
			if (stw)
			{
				if (i == index)
					stw->setActiveTab(true);
				else
					stw->setActiveTab(false);
			}
		}
	}

	// Set active CONTENT
	_contentsLayout->setCurrentIndex(index);
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

void SectionWidget::onCloseButtonClicked()
{
	qDebug() << Q_FUNC_INFO << currentIndex();
	const int index = currentIndex();
	if (index < 0 || index > _contents.size() - 1)
		return;
	SectionContent::RefPtr sc = _contents.at(index);
	if (sc.isNull())
		return;
	_container->hideSectionContent(sc);
}

int SectionWidget::GetNextUid()
{
	static int NextUid = 0;
	return ++NextUid;
}

//QHash<int, SectionWidget*>& SectionWidget::GetLookupMap()
//{
//	static QHash<int, SectionWidget*> LookupMap;
//	return LookupMap;

//}

//QHash<ContainerWidget*, QHash<int, SectionWidget*> >& SectionWidget::GetLookupMapByContainer()
//{
//	static QHash<ContainerWidget*, QHash<int, SectionWidget*> > LookupMapByContainer;
//	return LookupMapByContainer;
//}

ADS_NAMESPACE_END
