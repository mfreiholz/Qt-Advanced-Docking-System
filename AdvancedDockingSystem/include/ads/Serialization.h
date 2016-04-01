#ifndef ADS_SERIALIZATION_H
#define ADS_SERIALIZATION_H

#include <QtGlobal>
#include <QList>
#include <QString>
#include <QDataStream>
#include <QBuffer>

#include "ads/API.h"

ADS_NAMESPACE_SER_BEGIN

class Header
{
public:
	static qint32 MAGIC;
	static qint32 MAJOR_VERSION;
	static qint32 MINOR_VERSION;

	Header();
	qint32 magic;
	qint32 majorVersion;
	qint32 minorVersion;
};
QDataStream& operator<<(QDataStream& out, const Header& data);
QDataStream& operator>>(QDataStream& in, Header& data);


class OffsetsHeader
{
public:
	OffsetsHeader();

	qint64 entriesCount;
	QList<class OffsetsHeaderEntry> entries;
};
QDataStream& operator<<(QDataStream& out, const OffsetsHeader& data);
QDataStream& operator>>(QDataStream& in, OffsetsHeader& data);


class OffsetsHeaderEntry
{
public:
	enum Type
	{
		Unknown       = 0x00000001,
		Hierarchy     = 0x00000002,
		SectionIndex  = 0x00000003
	};

	OffsetsHeaderEntry();
	qint32 type;
	qint64 offset;
	qint64 contentSize;
};
QDataStream& operator<<(QDataStream& out, const OffsetsHeaderEntry& data);
QDataStream& operator>>(QDataStream& in, OffsetsHeaderEntry& data);


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
QDataStream& operator<<(QDataStream& out, const SectionIndexData& data);
QDataStream& operator>>(QDataStream& in, SectionIndexData& data);


/*!
 * \brief The InMemoryWriter class writes into a QByteArray.
 */
class InMemoryWriter
{
public:
	InMemoryWriter();
	bool write(OffsetsHeaderEntry::Type type, const QByteArray& data);
	bool write(const SectionIndexData& data);
	QByteArray toByteArray() const;

private:
	QBuffer _contentBuffer;
	OffsetsHeader _offsetsHeader;
};

/*!
 * \brief The InMemoryReader class
 */
class InMemoryReader
{
public:
	InMemoryReader(const QByteArray& data);
	bool initReadHeader();
	bool read(OffsetsHeaderEntry::Type type, QByteArray &data) const;

private:
	QByteArray _data;
	QBuffer _buff;

	OffsetsHeader _offsetsHeader;
};

ADS_NAMESPACE_SER_END
#endif
