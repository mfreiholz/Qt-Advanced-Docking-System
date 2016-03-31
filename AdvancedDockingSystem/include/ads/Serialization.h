#ifndef ADS_SERIALIZATION_H
#define ADS_SERIALIZATION_H

#include <QtGlobal>
#include <QList>
#include <QString>
#include <QDataStream>

#include "ads/API.h"

ADS_NAMESPACE_SER_BEGIN

class Header
{
public:
	Header();
	qint32 magic;
	qint32 version;
};
QDataStream& operator<<(QDataStream& out, const Header& data);
QDataStream& operator>>(QDataStream& in, Header& data);


class OffsetHeader
{
public:
	OffsetHeader();
	qint64 length;
	QList<class OffsetHeaderEntry> entries;
};
QDataStream& operator<<(QDataStream& out, const OffsetHeader& data);
QDataStream& operator>>(QDataStream& in, OffsetHeader& data);


class OffsetHeaderEntry
{
public:
	enum Type
	{
		Unknown       = 0x00000001,
		Hierarchy     = 0x00000002,
		SectionIndex  = 0x00000003
	};

	OffsetHeaderEntry();
	qint32 type;
	qint64 offset;
	qint64 length;
};
QDataStream& operator<<(QDataStream& out, const OffsetHeaderEntry& data);
QDataStream& operator>>(QDataStream& in, OffsetHeaderEntry& data);


class Section
{
public:
	Section();
	qint32 width;
	qint32 height;
	qint32 currentIndex;
	qint32 sectionContentsCount;
	QList<class SectionContent> sectionContents;
};
QDataStream& operator<<(QDataStream& out, const Section& data);
QDataStream& operator>>(QDataStream& in, Section& data);


class SectionContent
{
public:
	SectionContent();
	QString uniqueName;
	bool visible;
	qint32 preferredIndex;
};
QDataStream& operator<<(QDataStream& out, const SectionContent& data);
QDataStream& operator>>(QDataStream& in, SectionContent& data);


class FloatingContent
{
public:
	FloatingContent();
	QString uniqueName;
	qint32 xpos;
	qint32 ypos;
	qint32 width;
	qint32 height;
	bool visible;
};
QDataStream& operator<<(QDataStream& out, const FloatingContent& data);
QDataStream& operator>>(QDataStream& in, FloatingContent& data);


// Type: OffsetHeaderEntry::Hierarchy
class HierarchyData
{
public:
	HierarchyData();
	QByteArray data;
};
QDataStream& operator<<(QDataStream& out, const HierarchyData& data);
QDataStream& operator>>(QDataStream& in, HierarchyData& data);


// Type: OffsetHeaderEntry::SectionIndex
class SectionIndexData
{
public:
	SectionIndexData();
	qint32 sectionsCount;
	QList<Section> sections;
};

ADS_NAMESPACE_SER_END
#endif
