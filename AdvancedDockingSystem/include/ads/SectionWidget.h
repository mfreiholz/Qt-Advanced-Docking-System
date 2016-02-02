#ifndef SECTION_WIDGET_H
#define SECTION_WIDGET_H

#include <QDebug>
#include <QList>
#include <QHash>
#include <QFrame>

#include "ads/API.h"
#include "ads/SectionContent.h"

class QBoxLayout;
class QStackedLayout;

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

public:
	explicit SectionWidget(ContainerWidget* parent);
	virtual ~SectionWidget();

	int uid() const;
	ContainerWidget* containerWidget() const;

	QRect titleAreaGeometry() const;
	QRect contentAreaGeometry() const;

	QList<SectionContent::RefPtr> contents() const { return _contents; }
	void addContent(SectionContent::RefPtr c);
	void addContent(const InternalContentData& data, bool autoActivate);
	InternalContentData take(int uid, bool del = true);
	int indexOfContent(SectionContent::RefPtr c) const;
	int indexOfContentByTitlePos(const QPoint& pos, QWidget* exclude = NULL) const;

	void moveContent(int from, int to);

public slots:
	void setCurrentIndex(int index);

protected:
	virtual void paintEvent(QPaintEvent*);

private slots:
	void onSectionTitleClicked();

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
};

ADS_NAMESPACE_END
#endif