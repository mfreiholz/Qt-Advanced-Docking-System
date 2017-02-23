#ifndef SECTION_WIDGET_H
#define SECTION_WIDGET_H
/*******************************************************************************
** QtAdcancedDockingSystem
** Copyright (C) 2017 Uwe Kindler
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program. If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
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
#include "ads/FloatingWidget.h"

ADS_NAMESPACE_BEGIN
class CMainContainerWidget;
class SectionTitleWidget;
class CSectionContentWidget;

// SectionWidget manages multiple instances of SectionContent.
// It displays a title TAB, which is clickable and will switch to
// the contents associated to the title when clicked.
class ADS_EXPORT_API SectionWidget : public QFrame
{
	Q_OBJECT
	friend class CMainContainerWidget;
	friend class CContainerWidget;

	explicit SectionWidget(CMainContainerWidget* MainContainer, CContainerWidget* parent);

public:
	virtual ~SectionWidget();

	int uid() const;
	CContainerWidget* containerWidget() const;

	QRect titleAreaGeometry() const;
	QRect contentAreaGeometry() const;

	const QList<SectionContent::RefPtr>& contents() const { return m_Contents; }
	void addContent(const SectionContent::RefPtr& c);
	void addContent(const InternalContentData& data, bool autoActivate);
	bool takeContent(int uid, InternalContentData& data);
	bool takeContentAt(int Index, InternalContentData& data);
	int indexOfContent(const SectionContent::RefPtr& c) const;
	int indexOfContentByUid(int uid) const;
	int indexOfContentByTitlePos(const QPoint& pos, QWidget* exclude = NULL) const;

	int currentIndex() const;
	void moveContent(int from, int to);

    virtual bool eventFilter(QObject *watched, QEvent *event);

    inline int contentCount() const {return m_ContentWidgets.size();}

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
	QPointer<CMainContainerWidget> m_MainContainerWidget;
	QList<SectionContent::RefPtr> m_Contents;
	QList<SectionTitleWidget*> m_TitleWidgets;
	QList<CSectionContentWidget*> m_ContentWidgets;

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
    QPoint _dragStartPos;
    QPointer<FloatingWidget> _fw;
	virtual void wheelEvent(QWheelEvent*);
    virtual void mousePressEvent(QMouseEvent* ev);
    virtual void mouseMoveEvent(QMouseEvent* ev);
};

ADS_NAMESPACE_END
#endif
