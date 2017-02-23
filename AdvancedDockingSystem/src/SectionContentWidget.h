#ifndef SECTION_CONTENT_WIDGET_H
#define SECTION_CONTENT_WIDGET_H
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
#include <QFrame>

#include "API.h"
#include "SectionContent.h"

namespace ads
{
class CMainContainerWidget;
class SectionWidget;
class SectionTitleWidget;


struct SectionContentWidgetPrivate;


class CSectionContentWidget : public QFrame
{
	Q_OBJECT
private:
	SectionContentWidgetPrivate* d;
	friend class SectionContentWidgetPrivate;
	friend class CMainContainerWidget;

public:
	enum Flag
	{
		None = 0,
		Closeable = 1,
		Maximizable = 2,
		AllFlags = Closeable | Maximizable
	};
	Q_DECLARE_FLAGS(Flags, Flag)

	CSectionContentWidget(SectionContent::RefPtr c = SectionContent::RefPtr(), QWidget* parent = 0);
	virtual ~CSectionContentWidget();

	/*!
	 * Creates new content, associates it to <em>container</em> and takes ownership of
	 * <em>title</em>- and <em>content</em>- widgets.
	 * \param uniqueName An unique identifier across the entire process.
	 * \param container The parent ContainerWidget in which this content will be active.
	 * \param title The widget to use as title.
	 * \param content The widget to use as content.
	 * \return May return a invalid ref-pointer in case of invalid parameters.
	 */
	static CSectionContentWidget* newSectionContent(const QString& uniqueName,
		CMainContainerWidget* container, QWidget* title, QWidget* content);

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
	SectionContent::RefPtr _content;
};

} // namespace ads
#endif
