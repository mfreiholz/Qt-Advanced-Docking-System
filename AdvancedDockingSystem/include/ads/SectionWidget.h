#ifndef SECTION_WIDGET_H
#define SECTION_WIDGET_H

#include <QDebug>
#include <QList>
#include <QHash>
#include <QFrame>
class QBoxLayout;
class QStackedLayout;

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
class SectionWidget : public QFrame
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
	int indexOfContentByTitlePos(const QPoint& pos, QWidget* exclude = NULL) const;

	int currentIndex() const;
	void moveContent(int from, int to);

public slots:
	void setCurrentIndex(int index);

private slots:
	void onSectionTitleClicked();
	void onCloseButtonClicked();

private:
	const int _uid;

	ContainerWidget* _container;
	QList<SectionContent::RefPtr> _contents;
	QList<SectionTitleWidget*> _sectionTitles;
	QList<SectionContentWidget*> _sectionContents;

	QBoxLayout *_tabsLayout;
	QStackedLayout *_contentsLayout;

	QPoint _mousePressPoint;
	SectionContent::RefPtr _mousePressContent;
	SectionTitleWidget* _mousePressTitleWidget;

	static int NextUid;
	static QHash<int, SectionWidget*> LookupMap;
	static QHash<ContainerWidget*, QHash<int, SectionWidget*> > LookupMapByContainer;
};

ADS_NAMESPACE_END
#endif
