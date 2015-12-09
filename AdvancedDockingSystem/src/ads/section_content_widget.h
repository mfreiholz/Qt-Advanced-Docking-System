#ifndef SECTION_CONTENT_WIDGET_H
#define SECTION_CONTENT_WIDGET_H

#include <QFrame>

#include "ads.h"
#include "section_content.h"

ADS_NAMESPACE_BEGIN

class SectionWidget;

class SectionContentWidget : public QFrame
{
	Q_OBJECT

public:
	SectionContentWidget(SectionContent::RefPtr c, QWidget* parent = 0);

private:
	SectionContent::RefPtr _content;
};

ADS_NAMESPACE_END
#endif