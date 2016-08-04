#ifndef SECTION_WIDGET_H
#define SECTION_WIDGET_H

#include <QDebug>
#include <QPointer>
#include <QList>
#include <QFrame>
#include <QScrollArea>
class QBoxLayout;
class QStackedLayout;
class QPushButton;
class QMenu;

#include "ads/API.h"
#include "ads/Internal.h"
#include "ads/SectionContent.h"

ADS_NAMESPACE_BEGIN
class ContainerWidget;
class SectionTitleWidget;
class SectionContentWidget;

// SectionWidget manages multiple instances of SectionContent.
// It displays a title TAB, which is clickable and will switch to
// the contents associated to the title when clicked.
class ADS_EXPORT_API SectionWidget : public QFrame
{
	Q_OBJECT
	friend class ContainerWidget;

	explicit SectionWidget(ContainerWidget* parent);

public:
	virtual ~SectionWidget();

	int uid() const;
	ContainerWidget* containerWidget() const;

	QRect titleAreaGeometry() const;
	QRect contentAreaGeometry() const;

	const QList<SectionContent::RefPtr>& contents() const { return _contents; }
	void addContent(const SectionContent::RefPtr& c);
	void addContent(const InternalContentData& data, bool autoActivate);
	bool takeContent(int uid, InternalContentData& data);
	int indexOfContent(const SectionContent::RefPtr& c) const;
	int indexOfContentByUid(int uid) const;
	int indexOfContentByTitlePos(const QPoint& pos, QWidget* exclude = NULL) const;

	int currentIndex() const;
	void moveContent(int from, int to);

protected:
	virtual void showEvent(QShowEvent*);

public slots:
	void setCurrentIndex(int index);

private slots:
	void onSectionTitleClicked();
	void onCloseButtonClicked();
	void onTabsMenuActionTriggered(bool);
	void updateTabsMenu();


private:
	const int _uid;

	QPointer<ContainerWidget> _container;
	QList<SectionContent::RefPtr> _contents;
	QList<SectionTitleWidget*> _sectionTitles;
	QList<SectionContentWidget*> _sectionContents;

	QBoxLayout* _topLayout;
	QScrollArea* _tabsScrollArea;
	QWidget* _tabsContainerWidget;
	QBoxLayout* _tabsLayout;
	QPushButton* _tabsMenuButton;
	QPushButton* _closeButton;
	int _tabsLayoutInitCount; // used for calculations on _tabsLayout modification calls.

	QStackedLayout *_contentsLayout;

	QPoint _mousePressPoint;
	SectionContent::RefPtr _mousePressContent;
	SectionTitleWidget* _mousePressTitleWidget;

	static int GetNextUid();
};

/* Custom scrollable implementation for tabs */
class SectionWidgetTabsScrollArea : public QScrollArea
{
public:
	SectionWidgetTabsScrollArea(SectionWidget* sectionWidget, QWidget* parent = NULL);
	virtual ~SectionWidgetTabsScrollArea();

protected:
	virtual void wheelEvent(QWheelEvent*);
};

ADS_NAMESPACE_END
#endif
