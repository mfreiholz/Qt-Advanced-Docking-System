#ifndef SECTION_TITLE_WIDGET_H
#define SECTION_TITLE_WIDGET_H

#include <QFrame>
#include <QPoint>
#include "ads.h"
#include "section_content.h"

ADS_NAMESPACE_BEGIN

class SectionWidget;
class FloatingWidget;

class SectionTitleWidget : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(bool activeTab MEMBER _activeTab NOTIFY activeTabChanged)

	friend class SectionWidget;

	SectionContent::RefPtr _content;
	QPointer<FloatingWidget> _fw;
	QPoint _dragStartPos;

	bool _activeTab;

public:
	SectionTitleWidget(SectionContent::RefPtr content, QWidget* parent);

	bool isActiveTab() const;
	void setActiveTab(bool active);

protected:
	virtual void mousePressEvent(QMouseEvent* ev);
	virtual void mouseReleaseEvent(QMouseEvent* ev);
	virtual void mouseMoveEvent(QMouseEvent* ev);

signals:
	void activeTabChanged();
	void clicked();
};

ADS_NAMESPACE_END
#endif