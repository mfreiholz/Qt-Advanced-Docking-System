#ifndef ADS_API_H
#define ADS_API_H

#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QSharedPointer>
class QSplitter;

// Use namespace
// Disabled with Qt4!
#ifdef ADS_NAMESPACE_ENABLED
	#define ADS_NAMESPACE_BEGIN namespace ads {
	#define ADS_NAMESPACE_END }
	#define ADS_NS ads
#else
	#define ADS_NAMESPACE_BEGIN
	#define ADS_NAMESPACE_END
	#define ADS_NS
#endif

// Width of the native window frame border (based on OS).
#define ADS_WINDOW_FRAME_BORDER_WIDTH 7

// Indicates whether ADS should include animations.
//#define ADS_ANIMATIONS_ENABLED 1
//#define ADS_ANIMATION_DURATION 150

ADS_NAMESPACE_BEGIN

enum DropArea
{
	InvalidDropArea = 0,
	TopDropArea = 1,
	RightDropArea = 2,
	BottomDropArea = 4,
	LeftDropArea = 8,
	CenterDropArea = 16,

	AllAreas = TopDropArea | RightDropArea | BottomDropArea | LeftDropArea | CenterDropArea
};
Q_DECLARE_FLAGS(DropAreas, DropArea)

void deleteEmptySplitter(class ContainerWidget* container);
class ContainerWidget* findParentContainerWidget(class QWidget* w);
class SectionWidget* findParentSectionWidget(class QWidget* w);
QSplitter* findParentSplitter(class QWidget* w);
QSplitter* findImmediateSplitter(class QWidget* w);

ADS_NAMESPACE_END
#endif
