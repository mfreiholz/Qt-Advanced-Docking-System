#ifndef SECTION_TITLE_WIDGET_H
#define SECTION_TITLE_WIDGET_H

#include <QPointer>
#include <QPoint>
#include <QFrame>

#include "ads/API.h"
#include "ads/SectionContent.h"

class QPushButton;

ADS_NAMESPACE_BEGIN
class MainContainerWidget;
class SectionWidget;
class FloatingWidget;

class SectionTitleWidget : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(bool activeTab READ isActiveTab WRITE setActiveTab NOTIFY activeTabChanged)

	friend class MainContainerWidget;
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
   void moveFloatingWidget(QMouseEvent* ev, MainContainerWidget* cw);
   void startFloating(QMouseEvent* ev, MainContainerWidget* cw, SectionWidget* sectionwidget);
   void moveTab(QMouseEvent* ev);

signals:
	void activeTabChanged();
	void clicked();
};

ADS_NAMESPACE_END
#endif
