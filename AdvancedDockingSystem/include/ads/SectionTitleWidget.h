#ifndef SECTION_TITLE_WIDGET_H
#define SECTION_TITLE_WIDGET_H

#include <QPointer>
#include <QPoint>
#include <QFrame>

#include "ads/API.h"
#include "ads/SectionContent.h"

ADS_NAMESPACE_BEGIN
class ContainerWidget;
class SectionWidget;
class FloatingWidget;

class SectionTitleWidget : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(bool activeTab READ isActiveTab WRITE setActiveTab NOTIFY activeTabChanged)

	friend class ContainerWidget;
	friend class SectionWidget;

    SectionContent::RefPtr m_Content;

	// Drag & Drop (Floating)
    QPointer<FloatingWidget> m_FloatingWidget;
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
   void moveFloatingWidget(QMouseEvent* ev, ContainerWidget* cw);
   void startFloating(QMouseEvent* ev, ContainerWidget* cw, SectionWidget* sectionwidget);
   void moveTab(QMouseEvent* ev);

signals:
	void activeTabChanged();
	void clicked();
};

ADS_NAMESPACE_END
#endif
