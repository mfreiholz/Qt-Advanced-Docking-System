#ifndef FLOATINGWIDGET_H
#define FLOATINGWIDGET_H

#include <QWidget>
#include <QFrame>

#include "ads.h"
#include "section_content.h"

class QBoxLayout;

ADS_NAMESPACE_BEGIN

class ContainerWidget;
class SectionTitleWidget;
class SectionContentWidget;

// FloatingWidget holds and displays SectionContent as a floating window.
// It can be resized, moved and dropped back into a SectionWidget.
class FloatingWidget : public QWidget
{
	Q_OBJECT

public:
	FloatingWidget(ContainerWidget* container, SectionContent::RefPtr sc, SectionTitleWidget* titleWidget, SectionContentWidget* contentWidget, QWidget* parent = nullptr);
	virtual ~FloatingWidget();

	InternalContentData takeContent();
	SectionContent::RefPtr content() const { return _content; }

protected:
	virtual void closeEvent(QCloseEvent* e);

private:
	ContainerWidget* _container;
	SectionContent::RefPtr _content;
	SectionTitleWidget* _titleWidget;
	SectionContentWidget* _contentWidget;

	QBoxLayout* _titleLayout;
};

ADS_NAMESPACE_END
#endif