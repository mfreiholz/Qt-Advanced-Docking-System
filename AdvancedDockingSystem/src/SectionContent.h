#ifndef SECTIONCONTENT_H
#define SECTIONCONTENT_H
/*******************************************************************************
** QtAdcancedDockingSystem
** Copyright (C) 2017 Uwe Kindler
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program. If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
#include <QSharedPointer>
#include <QWeakPointer>
#include <QPointer>


class QWidget;

#include "API.h"

namespace ads
{

class CMainContainerWidget;

class ADS_EXPORT_API SectionContent
{
	friend class CMainContainerWidget;

private:
	SectionContent();
	SectionContent(const SectionContent&);
	SectionContent& operator=(const SectionContent&);

public:
	typedef QSharedPointer<SectionContent> RefPtr;
	typedef QWeakPointer<SectionContent> WeakPtr;

	enum Flag
	{
		None = 0,
		Closeable = 1,
		Maximizable = 2,
		AllFlags = Closeable | Maximizable
	};
	Q_DECLARE_FLAGS(Flags, Flag)

	/*!
	 * Creates new content, associates it to <em>container</em> and takes ownership of
	 * <em>title</em>- and <em>content</em>- widgets.
	 * \param uniqueName An unique identifier across the entire process.
	 * \param container The parent ContainerWidget in which this content will be active.
	 * \param title The widget to use as title.
	 * \param content The widget to use as content.
	 * \return May return a invalid ref-pointer in case of invalid parameters.
	 */
	static RefPtr newSectionContent(const QString& uniqueName, CMainContainerWidget* container, QWidget* title, QWidget* content);

	virtual ~SectionContent();
	int uid() const;
	QString uniqueName() const;
	CMainContainerWidget* containerWidget() const;
	QWidget* titleWidgetContent() const;
	QWidget* contentWidget() const;
	Flags flags() const;

	QString visibleTitle() const;
	QString title() const;
	void setTitle(const QString& title);
	void setFlags(const Flags f);

private:
	const int _uid;
	QString _uniqueName;

	QPointer<CMainContainerWidget> m_MainContainerWidget;
	QPointer<QWidget> m_TitleWidgetContent;
	QPointer<QWidget> m_ContentWidget;

	// Optional attributes
	QString _title;
	Flags _flags;

	/* Note: This method could be a problem in static build environment
	 * since it may begin with 0 for every module which uses ADS.
	 */
	static int GetNextUid();
};

} // namespace ads
#endif
