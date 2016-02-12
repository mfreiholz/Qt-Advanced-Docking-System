#ifndef ADS_INTERNAL_HEADER
#define ADS_INTERNAL_HEADER

#include "ads/API.h"
#include "ads/SectionContent.h"

ADS_NAMESPACE_BEGIN
class SectionContent;
class SectionTitleWidget;
class SectionContentWidget;


class InternalContentData
{
public:
	InternalContentData();
	~InternalContentData();

	SectionContent::RefPtr content;
	SectionTitleWidget* titleWidget;
	SectionContentWidget* contentWidget;
};


ADS_NAMESPACE_END
#endif
