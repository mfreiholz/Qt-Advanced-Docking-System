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

qint32 Header::MAGIC = 0x00001337;
qint32 Header::MAJOR_VERSION = 2;
qint32 Header::MINOR_VERSION = 0;

Header::Header() :
	magic(0), majorVersion(0), minorVersion(0)
{
}

QDataStream& operator<<(QDataStream& out, const Header& data)
{
	out << data.magic;
	out << data.majorVersion;
	out << data.minorVersion;
	return out;
}

QDataStream& operator>>(QDataStream& in, Header& data)
{
	in >> data.magic;
	in >> data.majorVersion;
	in >> data.minorVersion;
	return in;
}

///////////////////////////////////////////////////////////////////////////////

OffsetsHeader::OffsetsHeader() :
	entriesCount(0)
{
}

QDataStream& operator<<(QDataStream& out, const OffsetsHeader& data)
{
	out << data.entriesCount;
	for (int i = 0; i < data.entriesCount; ++i)
	{
		out << data.entries.at(i);
	}
	return out;
}

QDataStream& operator>>(QDataStream& in, OffsetsHeader& data)
{
	in >> data.entriesCount;
	for (int i = 0; i < data.entriesCount; ++i)
	{
		OffsetsHeaderEntry entry;
		in >> entry;
		data.entries.append(entry);
	}
	return in;
}

///////////////////////////////////////////////////////////////////////////////

OffsetsHeaderEntry::OffsetsHeaderEntry() :
	type(Unknown), offset(0), contentSize(0)
{
}

QDataStream& operator<<(QDataStream& out, const OffsetsHeaderEntry& data)
{
	out << data.type;
	out << data.offset;
	out << data.contentSize;
	return out;
}

QDataStream& operator>>(QDataStream& in, OffsetsHeaderEntry& data)
{
	in >> data.type;
	in >> data.offset;
	in >> data.contentSize;
	return in;
}

///////////////////////////////////////////////////////////////////////////////

Section::Section() :
	width(0), height(0), currentIndex(0), sectionContentsCount(0)
{
}

QDataStream& operator<<(QDataStream& out, const Section& data)
{
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

QDataStream& operator>>(QDataStream& in, Section& data)
{
	in >> data.width;
	in >> data.height;
	in >> data.currentIndex;
	in >> data.sectionContentsCount;
	for (int i = 0; i < data.sectionContentsCount; ++i)
	{
		SectionContent sc;
		in >> sc;
		data.sectionContents.append(sc);
	}
	return in;
}

///////////////////////////////////////////////////////////////////////////////

SectionContent::SectionContent() :
	visible(false), preferredIndex(0)
{
}

QDataStream& operator<<(QDataStream& out, const SectionContent& data)
{
	out << data.uniqueName;
	out << data.visible;
	out << data.preferredIndex;
	return out;
}

QDataStream& operator>>(QDataStream& in, SectionContent& data)
{
	in >> data.uniqueName;
	in >> data.visible;
	in >> data.preferredIndex;
	return in;
}

///////////////////////////////////////////////////////////////////////////////

FloatingContent::FloatingContent() :
	xpos(0), ypos(0), width(0), height(0), visible(false)
{
}

QDataStream& operator<<(QDataStream& out, const FloatingContent& data)
{
	out << data.uniqueName;
	out << data.xpos;
	out << data.ypos;
	out << data.width;
	out << data.height;
	out << data.visible;
	return out;
}

QDataStream& operator>>(QDataStream& in, FloatingContent& data)
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
		Section s;
		in >> s;
		data.sections.append(s);
	}
	return in;
}

///////////////////////////////////////////////////////////////////////////////

InMemoryWriter::InMemoryWriter()
{
}

bool InMemoryWriter::write(OffsetsHeaderEntry::Type type, const QByteArray& data)
{
	OffsetsHeaderEntry entry;
	entry.type = type;
	entry.offset = _contentBuffer.pos();                    // Relative offset!
	entry.contentSize = data.size();

	_contentBuffer.write(data);

	_offsetsHeader.entries.append(entry);
	_offsetsHeader.entriesCount += 1;

	return true;
}

bool InMemoryWriter::write(const SectionIndexData& data)
{
	OffsetsHeaderEntry entry;
	entry.type = OffsetsHeaderEntry::SectionIndex;
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
	QBuffer buff(&data);

	QDataStream out(&buff);
	out.setVersion(QDataStream::Qt_4_5);

	// Basic format header.
	Header header;
	header.magic = Header::MAGIC;
	header.majorVersion = Header::MAJOR_VERSION;
	header.minorVersion = Header::MINOR_VERSION;
	out << header;

	// Offsets-Header
	// - Save begin pos
	// - Write OffsetsHeader
	// - Convert relative- to absolute-offsets
	// - Seek back to begin-pos and write OffsetsHeader again.
	// Use a copy of OffsetsHeader to keep the _offsetsHeader relative.
	const qint64 posOffsetHeaders = buff.pos();
	OffsetsHeader offsetsHeader = _offsetsHeader;
	out << offsetsHeader;

	// Now we know the size of the entire header.
	// We can update the relative- to absolute-offsets now.
	const qint64 allHeaderSize = buff.pos();
	for (int i = 0; i < offsetsHeader.entriesCount; ++i)
	{
		offsetsHeader.entries[i].offset += allHeaderSize;
	}

	// Seek back and write again with absolute offsets.
	// TODO Thats not nice, but it works...
	buff.seek(posOffsetHeaders);
	out << offsetsHeader;

	// Write contents.
	buff.write(_contentBuffer.data());

	return data;
}

///////////////////////////////////////////////////////////////////////////////

InMemoryReader::InMemoryReader(const QByteArray& data) :
	_data(data), _buff(&_data)
{
}

bool InMemoryReader::initReadHeader()
{
	QDataStream in(&_buff);
	in.setVersion(QDataStream::Qt_4_5);

	// Basic format header.
	Header header;
	in >> header;
	if (header.magic != Header::MAGIC)
	{
		qWarning() << QString("invalid format (magic=%1)").arg(header.magic);
		return false;
	}
	if (header.majorVersion > Header::MAJOR_VERSION)
	{
		qWarning() << QString("format is too new (major=%1; minor=%2)")
					  .arg(header.majorVersion).arg(header.minorVersion);
		return false;
	}

	// OffsetsHeader.
	in >> _offsetsHeader;

	return !in.atEnd();
}

bool InMemoryReader::read(OffsetsHeaderEntry::Type type, QByteArray& data)
{
	// Find offset for "type".
	int index = -1;
	for (int i = 0; i < _offsetsHeader.entriesCount; ++i)
	{
		if (_offsetsHeader.entries.at(index).type == type)
		{
			index = i;
			break;
		}
	}
	if (index < 0)
		return false;
	else if (_offsetsHeader.entries.at(index).offset == 0)
		return false;

	const OffsetsHeaderEntry& entry = _offsetsHeader.entries.at(index);
	_buff.seek(entry.offset);
	data.append(_buff.read(entry.contentSize));
	return true;
}

///////////////////////////////////////////////////////////////////////////////

ADS_NAMESPACE_SER_END
