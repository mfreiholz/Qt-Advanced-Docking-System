#ifndef ADS_H
#define ADS_H

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

// DragData contains information about dragged contents (SectionContent).
// e.g. from where is has been dragged.
class DragData
{
public:
	static const QString MIMETYPE;

	QByteArray serialize() const
	{
		QByteArray ba;
		QDataStream out(&ba, QIODevice::WriteOnly);
		out << contentUid;
		out << sectionUid;
		return ba;
	}

	void deserialize(const QByteArray& bytes)
	{
		QDataStream in(bytes);
		in >> contentUid;
		in >> sectionUid;
	}

	QString toString() const
	{
		return QString("content-uid=%1; section-uid=%2").arg(contentUid).arg(sectionUid);
	}

public:
	int contentUid;
	int sectionUid;
};

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

class ContainerWidget* findParentContainerWidget(class QWidget* w);
class SectionWidget* findParentSectionWidget(class QWidget* w);
QSplitter* findParentSplitter(class QWidget* w);
QSplitter* findImmediateSplitter(class QWidget* w);

ADS_NAMESPACE_END

#endif // ADC_H