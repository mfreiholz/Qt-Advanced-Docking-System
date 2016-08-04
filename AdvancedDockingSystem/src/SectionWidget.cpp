#include "ads/SectionWidget.h"

#include <QApplication>
#include <QBoxLayout>
#include <QStackedLayout>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QPainter>
#include <QStyle>
#include <QSplitter>
#include <QPushButton>
#include <QScrollBar>
#include <QMenu>

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

SectionWidget::SectionWidget(ContainerWidget* parent) :
	QFrame(parent),
	_uid(GetNextUid()),
	_container(parent),
	_tabsLayout(NULL),
	_tabsLayoutInitCount(0),
	_contentsLayout(NULL),
	_mousePressTitleWidget(NULL)
{
	QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	setLayout(l);

	/* top area with tabs and close button */

	_topLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	_topLayout->setContentsMargins(0, 0, 0, 0);
	_topLayout->setSpacing(0);
	l->addLayout(_topLayout);

	_tabsScrollArea = new SectionWidgetTabsScrollArea(this);
	_topLayout->addWidget(_tabsScrollArea, 1);

	_tabsContainerWidget = new QWidget();
	_tabsContainerWidget->setObjectName("tabsContainerWidget");
	_tabsScrollArea->setWidget(_tabsContainerWidget);

	_tabsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	_tabsLayout->setContentsMargins(0, 0, 0, 0);
	_tabsLayout->setSpacing(0);
	_tabsLayout->addStretch(1);
	_tabsContainerWidget->setLayout(_tabsLayout);

	_tabsMenuButton = new QPushButton();
	_tabsMenuButton->setObjectName("tabsMenuButton");
	_tabsMenuButton->setFlat(true);
	_tabsMenuButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarUnshadeButton));
	_tabsMenuButton->setMaximumWidth(_tabsMenuButton->iconSize().width());
	_topLayout->addWidget(_tabsMenuButton, 0);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	//QObject::connect(_tabsMenuButton, &QPushButton::clicked, this, &SectionWidget::onTabsMenuButtonClicked);
#else
	//QObject::connect(_tabsMenuButton, SIGNAL(clicked()), this, SLOT(onTabsMenuButtonClicked()));
#endif

	_closeButton = new QPushButton();
	_closeButton->setObjectName("closeButton");
	_closeButton->setFlat(true);
	_closeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	_closeButton->setToolTip(tr("Close"));
	_closeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_topLayout->addWidget(_closeButton, 0);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	QObject::connect(_closeButton, &QPushButton::clicked, this, &SectionWidget::onCloseButtonClicked);
#else
	QObject::connect(_closeButton, SIGNAL(clicked(bool)), this, SLOT(onCloseButtonClicked()));
#endif

	_tabsLayoutInitCount = _tabsLayout->count();

	/* central area with contents */

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
	return _topLayout->geometry();
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
	_tabsLayout->insertWidget(_tabsLayout->count() - _tabsLayoutInitCount, title);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
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

	updateTabsMenu();
}

void SectionWidget::addContent(const InternalContentData& data, bool autoActivate)
{
	_contents.append(data.content);

	// Add title-widget to tab-bar
	// #FIX: Make it visible, since it is possible that it was hidden previously.
	_sectionTitles.append(data.titleWidget);
	_tabsLayout->insertWidget(_tabsLayout->count() - _tabsLayoutInitCount, data.titleWidget);
	data.titleWidget->setVisible(true);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
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

	updateTabsMenu();
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
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
		title->setAttribute(Qt::WA_WState_Created, false); /* fix: floating rubberband #16 */
#endif
		title->disconnect(this);
		title->setParent(_container);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
		title->setAttribute(Qt::WA_WState_Created, true); /* fix: floating rubberband #16 */
#endif
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

	updateTabsMenu();

	data.content = sc;
	data.titleWidget = title;
	data.contentWidget = content;
	return !data.content.isNull();
}

int SectionWidget::indexOfContent(const SectionContent::RefPtr& c) const
{
	return _contents.indexOf(c);
}

int SectionWidget::indexOfContentByUid(int uid) const
{
	for (int i = 0; i < _contents.count(); ++i)
	{
		if (_contents[i]->uid() == uid)
			return i;
	}
	return -1;
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
		qDebug() << "Invalid index for tab movement" << from << to;
		_tabsLayout->update();
		return;
	}

	_contents.move(from, to);
	_sectionTitles.move(from, to);
	_sectionContents.move(from, to);

	QLayoutItem* liFrom = NULL;
	liFrom = _tabsLayout->takeAt(from);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	_tabsLayout->insertItem(to, liFrom);
#else
	_tabsLayout->insertWidget(to, liFrom->widget());
	delete liFrom;
	liFrom = NULL;
#endif

	liFrom = _contentsLayout->takeAt(from);
	_contentsLayout->insertWidget(to, liFrom->widget());
	delete liFrom;

	updateTabsMenu();
}

void SectionWidget::showEvent(QShowEvent*)
{
	_tabsScrollArea->ensureWidgetVisible(_sectionTitles.at(currentIndex()));
}

void SectionWidget::setCurrentIndex(int index)
{
	if (index < 0 || index > _contents.count() - 1)
	{
		qWarning() << Q_FUNC_INFO << "Invalid index" << index;
		return;
	}

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
				{
					stw->setActiveTab(true);
					_tabsScrollArea->ensureWidgetVisible(stw);
					if (stw->_content->flags().testFlag(SectionContent::Closeable))
						_closeButton->setEnabled(true);
					else
						_closeButton->setEnabled(false);
				}
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
	const int index = currentIndex();
	if (index < 0 || index > _contents.size() - 1)
		return;
	SectionContent::RefPtr sc = _contents.at(index);
	if (sc.isNull())
		return;
	_container->hideSectionContent(sc);
}

void SectionWidget::onTabsMenuActionTriggered(bool)
{
	QAction* a = qobject_cast<QAction*>(sender());
	if (a)
	{
		const int uid = a->data().toInt();
		const int index = indexOfContentByUid(uid);
		if (index >= 0)
			setCurrentIndex(index);
	}
}

void SectionWidget::updateTabsMenu()
{
	QMenu* m = new QMenu();
	for (int i = 0; i < _contents.count(); ++i)
	{
		const SectionContent::RefPtr& sc = _contents.at(i);
		QAction* a = m->addAction(QIcon(), sc->visibleTitle());
		a->setData(sc->uid());
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
		QObject::connect(a, &QAction::triggered, this, &SectionWidget::onTabsMenuActionTriggered);
#else
		QObject::connect(a, SIGNAL(triggered(bool)), this, SLOT(onTabsMenuActionTriggered(bool)));
#endif
	}
	QMenu* old = _tabsMenuButton->menu();
	_tabsMenuButton->setMenu(m);
	delete old;
}

int SectionWidget::GetNextUid()
{
	static int NextUid = 0;
	return ++NextUid;
}

/*****************************************************************************/

SectionWidgetTabsScrollArea::SectionWidgetTabsScrollArea(SectionWidget*,
		QWidget* parent) :
	QScrollArea(parent)
{
	/* Important: QSizePolicy::Ignored makes the QScrollArea behaves
	like a QLabel and automatically fits into the layout. */
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
	setFrameStyle(QFrame::NoFrame);
	setWidgetResizable(true);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

SectionWidgetTabsScrollArea::~SectionWidgetTabsScrollArea()
{
}

void SectionWidgetTabsScrollArea::wheelEvent(QWheelEvent* e)
{
	e->accept();
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	const int direction = e->angleDelta().y();
#else
	const int direction = e->delta();
#endif
	if (direction < 0)
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() + 20);
	else
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() - 20);
}

ADS_NAMESPACE_END
