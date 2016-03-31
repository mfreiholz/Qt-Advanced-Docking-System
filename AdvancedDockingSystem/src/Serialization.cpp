#include "ads/Serialization.h"

ADS_NAMESPACE_SER_BEGIN

/*
	\namespace ads::serialization

	Serialization of ContainerWidget
	--------------------------------

	# Data Format Header

	quint32                   Magic
	quint32                   Version

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

Header::Header() :
	magic(0), version(0)
{
}

QDataStream& operator<<(QDataStream& out, const Header& data)
{
	out << data.magic;
	out << data.version;
	return out;
}

QDataStream& operator>>(QDataStream& in, Header& data)
{
	in >> data.magic;
	in >> data.version;
	return in;
}

///////////////////////////////////////////////////////////////////////////////

OffsetHeader::OffsetHeader() :
	length(0)
{
}

QDataStream& operator<<(QDataStream& out, const OffsetHeader& data)
{
	out << data.length;
	for (int i = 0; i < data.length; ++i)
	{
		out << data.entries.at(i);
	}
	return out;
}

QDataStream& operator>>(QDataStream& in, OffsetHeader& data)
{
	in >> data.length;
	for (int i = 0; i < data.length; ++i)
	{
		OffsetHeaderEntry entry;
		in >> entry;
		data.entries.append(entry);
	}
	return in;
}

///////////////////////////////////////////////////////////////////////////////

OffsetHeaderEntry::OffsetHeaderEntry() :
	type(Unknown), offset(0), length(0)
{
}

QDataStream& operator<<(QDataStream& out, const OffsetHeaderEntry& data)
{
	out << data.type;
	out << data.offset;
	out << data.length;
	return out;
}

QDataStream& operator>>(QDataStream& in, OffsetHeaderEntry& data)
{
	in >> data.type;
	in >> data.offset;
	in >> data.length;
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

ADS_NAMESPACE_SER_END
