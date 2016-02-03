#ifndef ADS_API_H
#define ADS_API_H

#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QSharedPointer>
class QSplitter;

/*
 * ADS - Advanced Docking System for Qt
 * Developed by Manuel Freiholz
 */

#define ADS_NAMESPACE_BEGIN namespace ads {
#define ADS_NAMESPACE_END }

// Width of the native window frame border (based on OS).
#define ADS_WINDOW_FRAME_BORDER_WIDTH 7

// Indicates whether ADS should include animations.
//#define ADS_ANIMATIONS_ENABLED 1
//#define ADS_ANIMATION_DURATION 150

ADS_NAMESPACE_BEGIN

enum DropArea
{
	TopDropArea,
	RightDropArea,
	BottomDropArea,
	LeftDropArea,
	CenterDropArea,
	InvalidDropArea
};

class InternalContentData
{
public:
	InternalContentData() : titleWidget(0), contentWidget(0) {}
	QSharedPointer<class SectionContent> content;
	class SectionTitleWidget* titleWidget;
	class SectionContentWidget* contentWidget;
};

void deleteEmptySplitter(class ContainerWidget* container);
class ContainerWidget* findParentContainerWidget(class QWidget* w);
class SectionWidget* findParentSectionWidget(class QWidget* w);
QSplitter* findParentSplitter(class QWidget* w);
QSplitter* findImmediateSplitter(class QWidget* w);

ADS_NAMESPACE_END
#endif