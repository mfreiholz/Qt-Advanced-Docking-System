#include "ads/Serialization.h"

#include <QDebug>

ADS_NAMESPACE_SER_BEGIN

/*
	\namespace ads::serialization

	Serialization of ContainerWidget
	--------------------------------

	# Data Format Header

	quint32                   Magic
	quint32                   Major Version
	quint32                   Minor Version

	# Offsets of available contents

	qint32                    Number of offset headers
	LOOP
		qint32                Type (e.g. Hierachy, SectionIndex)
		qint64                Offset
		qint64                Length

	# Type: Hierachy
	# Used to recreate the GUI geometry and state.

	int                       Number of floating widgets
	LOOP                      Floating widgets
		QString               Unique name of content
		QByteArray            Geometry of floating widget
		bool                  Visibility

	int                       Number of layout items (Valid values: 0, 1)
	IF 0
		int                   Number of hidden contents
		LOOP                  Contents
			QString           Unique name of content
	ELSEIF 1
		... todo ...
	ENDIF

	# Type: SectionIndex
	# Can be used for quick lookups on details for SectionWidgets.
	# It includes sizes and its contents.

	qint32                   Number of section-widgets
	LOOP
		qint32               Width
		qint32               Height
		qint32               Current active tab index
		qint32               Number of contents
		LOOP
			QString          Unique name of content
			bool             Visibility
			qint32           Preferred tab index

*/

///////////////////////////////////////////////////////////////////////////////

qint32 HeaderEntity::MAGIC = 0x00001337;
qint32 HeaderEntity::MAJOR_VERSION = 2;
qint32 HeaderEntity::MINOR_VERSION = 0;

HeaderEntity::HeaderEntity() :
	magic(0), majorVersion(0), minorVersion(0)
{
}

QDataStream& operator<<(QDataStream& out, const HeaderEntity& data)
{
	out << data.magic;
	out << data.majorVersion;
	out << data.minorVersion;
	return out;
}

QDataStream& operator>>(QDataStream& in, HeaderEntity& data)
{
	in >> data.magic;
	in >> data.majorVersion;
	in >> data.minorVersion;
	return in;
}

///////////////////////////////////////////////////////////////////////////////

OffsetsHeaderEntity::OffsetsHeaderEntity() :
	entriesCount(0)
{
}

QDataStream& operator<<(QDataStream& out, const OffsetsHeaderEntity& data)
{
	out << data.entriesCount;
	for (int i = 0; i < data.entriesCount; ++i)
	{
		out << data.entries.at(i);
	}
	return out;
}

QDataStream& operator>>(QDataStream& in, OffsetsHeaderEntity& data)
{
	in >> data.entriesCount;
	for (int i = 0; i < data.entriesCount; ++i)
	{
		OffsetsHeaderEntryEntity entry;
		in >> entry;
		data.entries.append(entry);
	}
	return in;
}

///////////////////////////////////////////////////////////////////////////////

OffsetsHeaderEntryEntity::OffsetsHeaderEntryEntity() :
	type(ET_Unknown), offset(0), contentSize(0)
{
}

QDataStream& operator<<(QDataStream& out, const OffsetsHeaderEntryEntity& data)
{
	out << data.type;
	out << data.offset;
	out << data.contentSize;
	return out;
}

QDataStream& operator>>(QDataStream& in, OffsetsHeaderEntryEntity& data)
{
	in >> data.type;
	in >> data.offset;
	in >> data.contentSize;
	return in;
}

///////////////////////////////////////////////////////////////////////////////

SectionEntity::SectionEntity() :
	x(0), y(0), width(0), height(0), currentIndex(0), sectionContentsCount(0)
{
}

QDataStream& operator<<(QDataStream& out, const SectionEntity& data)
{
	out << data.x;
	out << data.y;
	out << data.width;
	out << data.height;
	out << data.currentIndex;
	out << data.sectionContentsCount;
	for (int i = 0; i < data.sectionContentsCount; ++i)
	{
		out << data.sectionContents.at(i);
	}
	return out;
}

QDataStream& operator>>(QDataStream& in, SectionEntity& data)
{
	in >> data.x;
	in >> data.y;
	in >> data.width;
	in >> data.height;
	in >> data.currentIndex;
	in >> data.sectionContentsCount;
	for (int i = 0; i < data.sectionContentsCount; ++i)
	{
		SectionContentEntity sc;
		in >> sc;
		data.sectionContents.append(sc);
	}
	return in;
}

///////////////////////////////////////////////////////////////////////////////

SectionContentEntity::SectionContentEntity() :
	visible(false), preferredIndex(0)
{
}

QDataStream& operator<<(QDataStream& out, const SectionContentEntity& data)
{
	out << data.uniqueName;
	out << data.visible;
	out << data.preferredIndex;
	return out;
}

QDataStream& operator>>(QDataStream& in, SectionContentEntity& data)
{
	in >> data.uniqueName;
	in >> data.visible;
	in >> data.preferredIndex;
	return in;
}

///////////////////////////////////////////////////////////////////////////////

FloatingContentEntity::FloatingContentEntity() :
	xpos(0), ypos(0), width(0), height(0), visible(false)
{
}

QDataStream& operator<<(QDataStream& out, const FloatingContentEntity& data)
{
	out << data.uniqueName;
	out << data.xpos;
	out << data.ypos;
	out << data.width;
	out << data.height;
	out << data.visible;
	return out;
}

QDataStream& operator>>(QDataStream& in, FloatingContentEntity& data)
{
	in >> data.uniqueName;
	in >> data.xpos;
	in >> data.ypos;
	in >> data.width;
	in >> data.height;
	in >> data.visible;
	return in;
}

///////////////////////////////////////////////////////////////////////////////

HierarchyData::HierarchyData()
{
}

QDataStream& operator<<(QDataStream& out, const HierarchyData& data)
{
	out << data.data;
	return out;
}

QDataStream& operator>>(QDataStream& in, HierarchyData& data)
{
	in >> data.data;
	return in;
}

///////////////////////////////////////////////////////////////////////////////

SectionIndexData::SectionIndexData() :
	sectionsCount(0)
{
}

QDataStream& operator<<(QDataStream& out, const SectionIndexData& data)
{
	out << data.sectionsCount;
	for (int i = 0; i < data.sectionsCount; ++i)
	{
		out << data.sections.at(i);
	}
	return out;
}

QDataStream& operator>>(QDataStream& in, SectionIndexData& data)
{
	in >> data.sectionsCount;
	for (int i = 0; i < data.sectionsCount; ++i)
	{
		SectionEntity s;
		in >> s;
		data.sections.append(s);
	}
	return in;
}

///////////////////////////////////////////////////////////////////////////////

InMemoryWriter::InMemoryWriter()
{
	_contentBuffer.open(QIODevice::ReadWrite);
}

bool InMemoryWriter::write(qint32 entryType, const QByteArray& data)
{
	OffsetsHeaderEntryEntity entry;
	entry.type = entryType;
	entry.offset = _contentBuffer.pos();                    // Relative offset!
	entry.contentSize = data.size();

	_contentBuffer.write(data);

	_offsetsHeader.entries.append(entry);
	_offsetsHeader.entriesCount += 1;

	return true;
}

bool InMemoryWriter::write(const SectionIndexData& data)
{
	OffsetsHeaderEntryEntity entry;
	entry.type = ET_SectionIndex;
	entry.offset = _contentBuffer.pos();                    // Relative offset!

	QDataStream out(&_contentBuffer);
	out.setVersion(QDataStream::Qt_4_5);
	out << data;

	entry.contentSize = _contentBuffer.size() - entry.offset;

	_offsetsHeader.entries.append(entry);
	_offsetsHeader.entriesCount += 1;

	return true;
}

QByteArray InMemoryWriter::toByteArray() const
{
	QByteArray data;
	QDataStream out(&data, QIODevice::ReadWrite);
	out.setVersion(QDataStream::Qt_4_5);

	// Basic format header.
	HeaderEntity header;
	header.magic = HeaderEntity::MAGIC;
	header.majorVersion = HeaderEntity::MAJOR_VERSION;
	header.minorVersion = HeaderEntity::MINOR_VERSION;
	out << header;

	// Offsets-Header
	// - Save begin pos
	// - Write OffsetsHeader
	// - Convert relative- to absolute-offsets
	// - Seek back to begin-pos and write OffsetsHeader again.
	// Use a copy of OffsetsHeader to keep the _offsetsHeader relative.
	const qint64 posOffsetHeaders = out.device()->pos();
	OffsetsHeaderEntity offsetsHeader = _offsetsHeader;
	out << offsetsHeader;

	// Now we know the size of the entire header.
	// We can update the relative- to absolute-offsets now.
	const qint64 allHeaderSize = out.device()->pos();
	for (int i = 0; i < offsetsHeader.entriesCount; ++i)
	{
		offsetsHeader.entries[i].offset += allHeaderSize;   // Absolute offset!
	}

	// Seek back and write again with absolute offsets.
	// TODO Thats not nice, but it works...
	out.device()->seek(posOffsetHeaders);
	out << offsetsHeader;

	// Write contents.
	out.writeRawData(_contentBuffer.data().constData(), _contentBuffer.size());

	return data;
}

///////////////////////////////////////////////////////////////////////////////

InMemoryReader::InMemoryReader(const QByteArray& data) :
	_data(data)
{
}

bool InMemoryReader::initReadHeader()
{
	QDataStream in(_data);
	in.setVersion(QDataStream::Qt_4_5);

	// Basic format header.
	HeaderEntity header;
	in >> header;
	if (header.magic != HeaderEntity::MAGIC)
	{
		qWarning() << QString("invalid format (magic=%1)").arg(header.magic);
		return false;
	}
	if (header.majorVersion != HeaderEntity::MAJOR_VERSION)
	{
		qWarning() << QString("format is too new (major=%1; minor=%2)")
					  .arg(header.majorVersion).arg(header.minorVersion);
		return false;
	}

	// OffsetsHeader.
	in >> _offsetsHeader;

	return !in.atEnd();
}

bool InMemoryReader::read(qint32 entryType, QByteArray& data)
{
	// Find offset for "type".
	int index = -1;
	for (int i = 0; i < _offsetsHeader.entriesCount; ++i)
	{
		if (_offsetsHeader.entries.at(i).type == entryType)
		{
			index = i;
			break;
		}
	}
	if (index < 0)
		return false;
	else if (_offsetsHeader.entries.at(index).offset == 0)
		return false;

	const OffsetsHeaderEntryEntity& entry = _offsetsHeader.entries.at(index);

	QDataStream in(_data);
	in.setVersion(QDataStream::Qt_4_5);
	in.device()->seek(entry.offset);

	char* buff = new char[entry.contentSize];
	in.readRawData(buff, entry.contentSize);
	data.append(buff, entry.contentSize);
	delete[] buff;

	return true;
}

bool InMemoryReader::read(SectionIndexData& sid)
{
	QByteArray sidData;
	if (!read(ET_SectionIndex, sidData) || sidData.isEmpty())
		return false;

	QDataStream in(sidData);
	in.setVersion(QDataStream::Qt_4_5);
	in >> sid;

	return in.atEnd();
}

///////////////////////////////////////////////////////////////////////////////

ADS_NAMESPACE_SER_END
