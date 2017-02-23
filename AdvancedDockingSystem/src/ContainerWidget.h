#ifndef ContainerWidgetH
#define ContainerWidgetH
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

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>

#include "ads/API.h"
#include "ads/Internal.h"
#include "ads/SectionContent.h"
#include "ads/FloatingWidget.h"
#include "ads/Serialization.h"
#include "ads/DropOverlay.h"

namespace ads
{
class SectionWidget;
class DropOverlay;
class InternalContentData;
class CMainContainerWidget;

/**
 * @brief
 */
class CContainerWidget  : public QFrame
{
	Q_OBJECT

	friend class SectionContent;
	friend class SectionWidget;
	friend class FloatingWidget;
	friend class SectionTitleWidget;
    friend class ContainerWidgetPrivate;

public:
	explicit CContainerWidget(CMainContainerWidget* MainContainerWidget, QWidget *parent = nullptr);
	virtual ~CContainerWidget();

	/**
	 * Returns the current zOrderIndex
	 */
	virtual unsigned int zOrderIndex() const;

	void dropFloatingWidget(FloatingWidget* FloatingWidget,
		const QPoint& TargetPos);

	SectionWidget* sectionWidgetAt(const QPoint& GlobalPos) const;

	/**
	 * This function returns true if this container widgets z order index is
	 * higher than the index of the container widget given in Other parameter
	 */
	bool isInFrontOf(CContainerWidget* Other) const;

	SectionWidget* dropContent(const InternalContentData& data, SectionWidget* targetSection, DropArea area, bool autoActive = true);

	/*!
	 * Adds the section-content <em>sc</em> to this container-widget into the section-widget <em>sw</em>.
	 * If <em>sw</em> is not NULL, the <em>area</em> is used to indicate how the content should be arranged.
	 * Returns a pointer to the SectionWidget of the added SectionContent. Do not use it for anything else than adding more
	 * SectionContent elements with this method.
	 */
	SectionWidget* addSectionContent(const SectionContent::RefPtr& sc, SectionWidget* sw = NULL, DropArea area = CenterDropArea);

	void dumpLayout();

	CMainContainerWidget* mainContainerWidget() const {return m_MainContainerWidget;}

	void addSectionWidget(SectionWidget* section);

	void takeSection(SectionWidget* Widget);

signals:
	/*!
	 * Emits whenever the "isActiveTab" state of a SectionContent changes.
	 * Whenever the users sets another tab as active, this signal gets invoked
	 * for the old tab and the new active tab (the order is unspecified).
	 */
	void activeTabChanged(const SectionContent::RefPtr& sc, bool active);

protected:
	void dropIntoContainer(FloatingWidget* FloatingWidget, DropArea area);
	void dropIntoSection(FloatingWidget* FloatingWidget, SectionWidget* targetSection, DropArea area);
	virtual bool event(QEvent *e) override;
	SectionWidget* newSectionWidget();
	SectionWidget* dropContentOuterHelper(QLayout* l, const InternalContentData& data, Qt::Orientation orientation, bool append);
	SectionWidget* insertNewSectionWidget(const InternalContentData& data,
		SectionWidget* targetSection, SectionWidget* ret, Qt::Orientation Orientation, int InsertIndexOffset);

	QList<SectionWidget*> m_Sections;
	// Layout stuff
	QGridLayout* m_MainLayout = nullptr;
	Qt::Orientation m_Orientation = Qt::Horizontal;
	QPointer<QSplitter> m_Splitter; // $mfreiholz: I'd like to remove this variable entirely,
								   // because it changes during user interaction anyway.

	CMainContainerWidget* m_MainContainerWidget = 0;
	unsigned int m_zOrderIndex = 0;
	static unsigned int zOrderCounter;

private:
	void dropChildSections(QWidget* Parent);

private slots:
	void onActiveTabChanged();
};

} // namespace ads

//---------------------------------------------------------------------------
#endif // ContainerWidgetH
