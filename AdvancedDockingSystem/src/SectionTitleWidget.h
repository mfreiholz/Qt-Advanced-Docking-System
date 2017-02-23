#ifndef SECTION_TITLE_WIDGET_H
#define SECTION_TITLE_WIDGET_H
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
#include <QPointer>
#include <QPoint>
#include <QFrame>

#include "API.h"
#include "SectionContent.h"

class QPushButton;

namespace ads
{

class CMainContainerWidget;
class SectionWidget;
class FloatingWidget;

class SectionTitleWidget : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(bool activeTab READ isActiveTab WRITE setActiveTab NOTIFY activeTabChanged)

	friend class CMainContainerWidget;
	friend class SectionWidget;
	friend class CContainerWidget;

    SectionContent::RefPtr m_Content;

	// Drag & Drop (Floating)
    QPoint m_DragStartMousePosition;
    QPoint m_DragStartGlobalMousePosition;
    QPoint m_DragStartPosition;

	// Drag & Drop (Title/Tabs)
    bool m_TabMoving;

	// Property values
    bool m_IsActiveTab;

public:
	SectionTitleWidget(SectionContent::RefPtr content, QWidget* parent);
	virtual ~SectionTitleWidget();

	bool isActiveTab() const;
	void setActiveTab(bool active);

protected:
	virtual void mousePressEvent(QMouseEvent* ev);
	virtual void mouseReleaseEvent(QMouseEvent* ev);
	virtual void mouseMoveEvent(QMouseEvent* ev);

private:
   void startFloating(QMouseEvent* ev, CMainContainerWidget* cw, SectionWidget* sectionwidget);
   void moveTab(QMouseEvent* ev);

signals:
	void activeTabChanged();
	void clicked();
};

} // namepsace ads
#endif
