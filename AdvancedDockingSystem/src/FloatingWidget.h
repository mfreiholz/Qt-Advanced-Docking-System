#ifndef FLOATINGWIDGET_H
#define FLOATINGWIDGET_H
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
#include <QWidget>
#include <QFrame>
class QBoxLayout;

#include "API.h"
#include "SectionContent.h"

namespace ads
{

class CMainContainerWidget;
class SectionTitleWidget;
class CSectionContentWidget;
class InternalContentData;
class SectionWidget;
class CContainerWidget;
class FloatingWidget;

class CFloatingTitleWidget : public QFrame
{
	Q_OBJECT
private:
	QPoint m_DragStartPosition;
	QPoint m_DragStartMousePosition;
	FloatingWidget* floatingWidget() const;
	CMainContainerWidget* mainContainerWidget() const;
	void moveFloatingWidget(QMouseEvent* ev);

private slots:
	void onMaximizeButtonClicked();

protected:
	virtual void mousePressEvent(QMouseEvent* ev);
	virtual void mouseReleaseEvent(QMouseEvent* ev);
	virtual void mouseMoveEvent(QMouseEvent* ev);

public:
	CFloatingTitleWidget(SectionContent::Flags Flags, FloatingWidget* Parent);

signals:
	void closeButtonClicked();
};


// FloatingWidget holds and displays SectionContent as a floating window.
// It can be resized, moved and dropped back into a SectionWidget.
class FloatingWidget : public QWidget
{
	Q_OBJECT

	friend class CMainContainerWidget;
	friend class CFloatingTitleWidget;

public:
	FloatingWidget(CMainContainerWidget* container, SectionContent::RefPtr sc, SectionTitleWidget* titleWidget, CSectionContentWidget* contentWidget, QWidget* parent = NULL);
    FloatingWidget(CMainContainerWidget* container, SectionWidget* sectionWidget);
    virtual ~FloatingWidget();

	/**
	 * Returns the current zOrderIndex
	 */
	unsigned int zOrderIndex() const;

	CContainerWidget* containerWidget() const  {return m_ContainerWidget;}
	CMainContainerWidget* mainContainerWidget() const {return m_MainContainerWidget;}

public://private:
	bool takeContent(InternalContentData& data);

	void startFloating(const QPoint& Pos);

protected:
	virtual void changeEvent(QEvent *event) override;
	virtual void moveEvent(QMoveEvent *event) override;
	virtual bool event(QEvent *e);
	void titleMouseReleaseEvent();
	virtual bool eventFilter(QObject *watched, QEvent *event) override;
	void updateDropOverlays(const QPoint& GlobalPos);

private slots:
	void onCloseButtonClicked();

private:
	void setDraggingActive(bool Active);

	CMainContainerWidget* m_MainContainerWidget;
	CContainerWidget* m_ContainerWidget;
	CContainerWidget* m_DropContainer;
	bool m_DraggingActive = false;
	unsigned int m_zOrderIndex = 0;
	QPoint m_DragStartPosition;
	QPoint m_DragStartMousePosition;
	static unsigned int zOrderCounter;
};

} // namespace ads
#endif
