#ifndef ADS_SERIALIZATION_H
#define ADS_SERIALIZATION_H

#include <QtGlobal>
#include <QList>
#include <QString>
#include <QDataStream>
#include <QBuffer>

#include "ads/API.h"

ADS_NAMESPACE_SER_BEGIN

enum EntryType
{
	ET_Unknown       = 0x00000000,
	ET_Hierarchy     = 0x00000001,
	ET_SectionIndex  = 0x00000002,

	// Begin of custom entry types (e.g. CustomType + 42)
	ET_Custom        = 0x0000ffff
};

class ADS_EXPORT_API HeaderEntity
{
public:
	static qint32 MAGIC;
	static qint32 MAJOR_VERSION;
	static qint32 MINOR_VERSION;

	HeaderEntity();
	qint32 magic;
	qint32 majorVersion;
	qint32 minorVersion;
};
QDataStream& operator<<(QDataStream& out, const HeaderEntity& data);
QDataStream& operator>>(QDataStream& in, HeaderEntity& data);


class ADS_EXPORT_API OffsetsHeaderEntity
{
public:
	OffsetsHeaderEntity();

	qint64 entriesCount;
	QList<class OffsetsHeaderEntryEntity> entries;
};
QDataStream& operator<<(QDataStream& out, const OffsetsHeaderEntity& data);
QDataStream& operator>>(QDataStream& in, OffsetsHeaderEntity& data);


class ADS_EXPORT_API OffsetsHeaderEntryEntity
{
public:
	OffsetsHeaderEntryEntity();
	qint32 type;
	qint64 offset;
	qint64 contentSize;
};
QDataStream& operator<<(QDataStream& out, const OffsetsHeaderEntryEntity& data);
QDataStream& operator>>(QDataStream& in, OffsetsHeaderEntryEntity& data);


class ADS_EXPORT_API SectionEntity
{
public:
	SectionEntity();
	qint32 x;
	qint32 y;
	qint32 width;
	qint32 height;
	qint32 currentIndex;
	qint32 sectionContentsCount;
	QList<class SectionContentEntity> sectionContents;
};
QDataStream& operator<<(QDataStream& out, const SectionEntity& data);
QDataStream& operator>>(QDataStream& in, SectionEntity& data);


class ADS_EXPORT_API SectionContentEntity
{
public:
	SectionContentEntity();
	QString uniqueName;
	bool visible;
	qint32 preferredIndex;
};
QDataStream& operator<<(QDataStream& out, const SectionContentEntity& data);
QDataStream& operator>>(QDataStream& in, SectionContentEntity& data);


class ADS_EXPORT_API FloatingContentEntity
{
public:
	FloatingContentEntity();
	QString uniqueName;
	qint32 xpos;
	qint32 ypos;
	qint32 width;
	qint32 height;
	bool visible;
};
QDataStream& operator<<(QDataStream& out, const FloatingContentEntity& data);
QDataStream& operator>>(QDataStream& in, FloatingContentEntity& data);


// Type: OffsetHeaderEntry::Hierarchy
class ADS_EXPORT_API HierarchyData
{
public:
	HierarchyData();
	QByteArray data;
};
QDataStream& operator<<(QDataStream& out, const HierarchyData& data);
QDataStream& operator>>(QDataStream& in, HierarchyData& data);


// Type: OffsetHeaderEntry::SectionIndex
class ADS_EXPORT_API SectionIndexData
{
public:
	SectionIndexData();
	qint32 sectionsCount;
	QList<SectionEntity> sections;
};
QDataStream& operator<<(QDataStream& out, const SectionIndexData& data);
QDataStream& operator>>(QDataStream& in, SectionIndexData& data);


/*!
 * \brief The InMemoryWriter class writes into a QByteArray.
 */
class ADS_EXPORT_API InMemoryWriter
{
public:
	InMemoryWriter();
	bool write(qint32 entryType, const QByteArray& data);
	bool write(const SectionIndexData& data);
	QByteArray toByteArray() const;
	qint32 offsetsCount() const { return _offsetsHeader.entriesCount; }

private:
	QBuffer _contentBuffer;
	OffsetsHeaderEntity _offsetsHeader;
};

/*!
 * \brief The InMemoryReader class
 */
class ADS_EXPORT_API InMemoryReader
{
public:
	InMemoryReader(const QByteArray& data);
	bool initReadHeader();
	bool read(qint32 entryType, QByteArray &data);
	bool read(SectionIndexData& sid);
	qint32 offsetsCount() const { return _offsetsHeader.entriesCount; }

private:
	QByteArray _data;
	OffsetsHeaderEntity _offsetsHeader;
};

ADS_NAMESPACE_SER_END
#endif
