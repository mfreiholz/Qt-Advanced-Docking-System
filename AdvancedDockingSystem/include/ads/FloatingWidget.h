#ifndef FLOATINGWIDGET_H
#define FLOATINGWIDGET_H

#include <QWidget>
class QBoxLayout;

#include "ads/API.h"
#include "ads/SectionContent.h"

ADS_NAMESPACE_BEGIN
class ContainerWidget;
class SectionTitleWidget;
class SectionContentWidget;
class InternalContentData;

// FloatingWidget holds and displays SectionContent as a floating window.
// It can be resized, moved and dropped back into a SectionWidget.
class FloatingWidget : public QWidget
{
	Q_OBJECT

	friend class ContainerWidget;

public:
	FloatingWidget(ContainerWidget* container, SectionContent::RefPtr sc, SectionTitleWidget* titleWidget, SectionContentWidget* contentWidget, QWidget* parent = NULL);
	virtual ~FloatingWidget();

	SectionContent::RefPtr content() const { return _content; }

public://private:
	bool takeContent(InternalContentData& data);

private slots:
	void onCloseButtonClicked();

private:
	ContainerWidget* _container;
	SectionContent::RefPtr _content;
	SectionTitleWidget* _titleWidget;
	SectionContentWidget* _contentWidget;

	QBoxLayout* _titleLayout;
};

ADS_NAMESPACE_END
#endif
