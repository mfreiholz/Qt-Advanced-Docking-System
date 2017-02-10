#include <ads/MainContainerWidget.h>
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QContextMenuEvent>
#include <QMenu>
#include <QSplitter>
#include <QDataStream>
#include <QtGlobal>
#include <QGridLayout>
#include <QPoint>
#include <QApplication>

#include "ads/Internal.h"
#include "ads/SectionWidget.h"
#include "ads/SectionTitleWidget.h"
#include "ads/SectionContentWidget.h"
#include "ads/DropOverlay.h"
#include "ads/Serialization.h"

#include <iostream>

ADS_NAMESPACE_BEGIN


QSplitter* MainContainerWidget::newSplitter(Qt::Orientation orientation, QWidget* parent)
{
	QSplitter* s = new QSplitter(orientation, parent);
	s->setProperty("ads-splitter", QVariant(true));
	s->setChildrenCollapsible(false);
	s->setOpaqueResize(false);
	return s;
}



MainContainerWidget::MainContainerWidget(QWidget *parent) :
	CContainerWidget(this, parent)
{
	m_SectionDropOverlay = new DropOverlay(this, DropOverlay::ModeSectionOverlay);

	m_ContainerDropOverlay = new DropOverlay(this, DropOverlay::ModeContainerOverlay);
	m_ContainerDropOverlay->setAttribute(Qt::WA_TransparentForMouseEvents);
	m_ContainerDropOverlay->setWindowFlags(m_ContainerDropOverlay->windowFlags() | Qt::WindowTransparentForInput);
	m_Containers.append(this);
}

MainContainerWidget::~MainContainerWidget()
{
	// Note: It's required to delete in 2 steps
	// Remove from list, and then delete.
	// Because the destrcutor of objects wants to modify the current
	// iterating list as well... :-/
	while (!m_Sections.isEmpty())
	{
		SectionWidget* sw = m_Sections.takeLast();
		delete sw;
	}
	while (!m_Floatings.isEmpty())
	{
		FloatingWidget* fw = m_Floatings.takeLast();
		delete fw;
	}
	m_SectionContentIdMap.clear();
	m_SectionContentNameMap.clear();
	m_SectionWidgetIdMap.clear();
}


SectionWidget* MainContainerWidget::addSectionContent(const SectionContent::RefPtr& sc, SectionWidget* sw, DropArea area)
{
	ADS_Expects(!sc.isNull());

	// Drop it based on "area"
	InternalContentData data;
	data.content = sc;
	data.titleWidget = new SectionTitleWidget(sc, NULL);
	data.contentWidget = new SectionContentWidget(sc, NULL);

	connect(data.titleWidget, SIGNAL(activeTabChanged()), this, SLOT(onActiveTabChanged()));
	return dropContent(data, sw, area, false);
}

bool MainContainerWidget::removeSectionContent(const SectionContent::RefPtr& sc)
{
	ADS_Expects(!sc.isNull());

	// Hide the content.
	// The hideSectionContent() automatically deletes no longer required SectionWidget objects.
	if (!hideSectionContent(sc))
		return false;

	// Begin of ugly work arround.
	// TODO The hideSectionContent() method should take care of deleting FloatingWidgets and SectionWidgets,
	// but only cares about SectionWidgets right now. So we need to check whether it was a FloatingWidget
	// and delete it.
	bool found = false;
	for (int i = 0; i < m_Floatings.count(); ++i)
	{
		FloatingWidget* fw = m_Floatings.at(i);
		InternalContentData data;
		if (!(found = fw->takeContent(data)))
			continue;
		m_Floatings.removeAll(fw);
		delete fw;
		delete data.titleWidget;
		delete data.contentWidget;
		break;
	} // End of ugly work arround.

	// Get from hidden contents and delete associated internal stuff.
	if (!m_HiddenSectionContents.contains(sc->uid()))
	{
		qFatal("Something went wrong... The content should have been there :-/");
		return false;
	}

	// Delete internal objects.
	HiddenSectionItem hsi = m_HiddenSectionContents.take(sc->uid());
	delete hsi.data.titleWidget;
	delete hsi.data.contentWidget;

	// Hide the custom widgets of SectionContent.
	// ... should we? ...

	return true;
}

bool MainContainerWidget::showSectionContent(const SectionContent::RefPtr& sc)
{
	ADS_Expects(!sc.isNull());

	// Search SC in floatings
	for (int i = 0; i < m_Floatings.count(); ++i)
	{
		FloatingWidget* fw = m_Floatings.at(i);
		const bool found = fw->content()->uid() == sc->uid();
		if (!found)
			continue;
		fw->setVisible(true);
		fw->_titleWidget->setVisible(true);
		fw->_contentWidget->setVisible(true);
		emit sectionContentVisibilityChanged(sc, true);
		return true;
	}

	// Search SC in hidden sections
	// Try to show them in the last position, otherwise simply append
	// it to the first section (or create a new section?)
	if (m_HiddenSectionContents.contains(sc->uid()))
	{
		const HiddenSectionItem hsi = m_HiddenSectionContents.take(sc->uid());
		hsi.data.titleWidget->setVisible(true);
		hsi.data.contentWidget->setVisible(true);
		SectionWidget* sw = nullptr;
		if (hsi.preferredSectionId > 0 && (sw = m_SectionWidgetIdMap.value(hsi.preferredSectionId)) != nullptr)
		{
			sw->addContent(hsi.data, true);
			emit sectionContentVisibilityChanged(sc, true);
			return true;
		}
		else if (m_Sections.size() > 0 && (sw = m_Sections.first()) != NULL)
		{
			sw->addContent(hsi.data, true);
			emit sectionContentVisibilityChanged(sc, true);
			return true;
		}
		else
		{
			sw = newSectionWidget();
			addSectionWidget(sw);
			sw->addContent(hsi.data, true);
			emit sectionContentVisibilityChanged(sc, true);
			return true;
		}
	}

	// Already visible?
	// TODO
	qWarning("Unable to show SectionContent, don't know where 8-/ (already visible?)");
	return false;
}

bool MainContainerWidget::hideSectionContent(const SectionContent::RefPtr& sc)
{
	ADS_Expects(!sc.isNull());

	// Search SC in floatings
	// We can simply hide floatings, nothing else required.
	for (int i = 0; i < m_Floatings.count(); ++i)
	{
		const bool found = m_Floatings.at(i)->content()->uid() == sc->uid();
		if (!found)
			continue;
		m_Floatings.at(i)->setVisible(false);
		emit sectionContentVisibilityChanged(sc, false);
		return true;
	}

	// Search SC in sections
	// It's required to remove the SC from SW completely and hold it in a
	// separate list as long as a "showSectionContent" gets called for the SC again.
	// In case that the SW does not have any other SCs, we need to delete it.
	for (int i = 0; i < m_Sections.count(); ++i)
	{
		SectionWidget* sw = m_Sections.at(i);
		const bool found = sw->indexOfContent(sc) >= 0;
		if (!found)
			continue;

		HiddenSectionItem hsi;
		hsi.preferredSectionId = sw->uid();
		hsi.preferredSectionIndex = sw->indexOfContent(sc);
		if (!sw->takeContent(sc->uid(), hsi.data))
			return false;

		hsi.data.titleWidget->setVisible(false);
		hsi.data.contentWidget->setVisible(false);
		m_HiddenSectionContents.insert(sc->uid(), hsi);

		if (sw->contents().isEmpty())
		{
			delete sw;
			sw = NULL;
			deleteEmptySplitter(this);
		}
		emit sectionContentVisibilityChanged(sc, false);
		return true;
	}

	// Search SC in hidden elements
	// The content may already be hidden
	if (m_HiddenSectionContents.contains(sc->uid()))
		return true;

	qFatal("Unable to hide SectionContent, don't know this one 8-/");
	return false;
}

bool MainContainerWidget::raiseSectionContent(const SectionContent::RefPtr& sc)
{
	ADS_Expects(!sc.isNull());

	// Search SC in sections
	for (int i = 0; i < m_Sections.count(); ++i)
	{
		SectionWidget* sw = m_Sections.at(i);
		int index = sw->indexOfContent(sc);
		if (index < 0)
			continue;
		sw->setCurrentIndex(index);
		return true;
	}

	// Search SC in floatings
	for (int i = 0; i < m_Floatings.size(); ++i)
	{
		FloatingWidget* fw = m_Floatings.at(i);
		if (fw->content()->uid() != sc->uid())
			continue;
		fw->setVisible(true);
		fw->raise();
		return true;
	}

	// Search SC in hidden
	if (m_HiddenSectionContents.contains(sc->uid()))
		return showSectionContent(sc);

	qFatal("Unable to hide SectionContent, don't know this one 8-/");
	return false;
}

bool MainContainerWidget::isSectionContentVisible(const SectionContent::RefPtr& sc)
{
	ADS_Expects(!sc.isNull());

	// Search SC in floatings
	for (int i = 0; i < m_Floatings.count(); ++i)
	{
		const bool found = m_Floatings.at(i)->content()->uid() == sc->uid();
		if (!found)
			continue;
		return m_Floatings.at(i)->isVisible();
	}

	// Search SC in sections
	for (int i = 0; i < m_Sections.count(); ++i)
	{
		SectionWidget* sw = m_Sections.at(i);
		const int index = sw->indexOfContent(sc);
		if (index < 0)
			continue;
		return true;
	}

	// Search SC in hidden
	if (m_HiddenSectionContents.contains(sc->uid()))
		return false;

	qWarning() << "SectionContent is not a part of this ContainerWidget:" << sc->uniqueName();
	return false;
}

QMenu* MainContainerWidget::createContextMenu() const
{
	// Fill map with actions (sorted by key!)
	QMap<QString, QAction*> actions;

	// Visible contents of sections
	for (int i = 0; i < m_Sections.size(); ++i)
	{
		const SectionWidget* sw = m_Sections.at(i);
		const QList<SectionContent::RefPtr>& contents = sw->contents();
		foreach (const SectionContent::RefPtr& sc, contents)
		{
			QAction* a = new QAction(QIcon(), sc->visibleTitle(), NULL);
			a->setObjectName(QString("ads-action-sc-%1").arg(QString::number(sc->uid())));
			a->setProperty("uid", sc->uid());
			a->setProperty("type", "section");
			a->setCheckable(true);
			a->setChecked(true);
			a->setEnabled(sc->flags().testFlag(SectionContent::Closeable));
			connect(a, SIGNAL(toggled(bool)), this, SLOT(onActionToggleSectionContentVisibility(bool)));
			actions.insert(a->text(), a);
		}
	}

	// Hidden contents of sections
	QHashIterator<int, HiddenSectionItem> hiddenIter(m_HiddenSectionContents);
	while (hiddenIter.hasNext())
	{
		hiddenIter.next();
		const SectionContent::RefPtr sc = hiddenIter.value().data.content;

		QAction* a = new QAction(QIcon(), sc->visibleTitle(), NULL);
		a->setObjectName(QString("ads-action-sc-%1").arg(QString::number(sc->uid())));
		a->setProperty("uid", sc->uid());
		a->setProperty("type", "section");
		a->setCheckable(true);
		a->setChecked(false);
		connect(a, SIGNAL(toggled(bool)), this, SLOT(onActionToggleSectionContentVisibility(bool)));
		actions.insert(a->text(), a);
	}

	// Floating contents
	for (int i = 0; i < m_Floatings.size(); ++i)
	{
		const FloatingWidget* fw = m_Floatings.at(i);
		const SectionContent::RefPtr sc = fw->content();

		QAction* a = new QAction(QIcon(), sc->visibleTitle(), NULL);
		a->setObjectName(QString("ads-action-sc-%1").arg(QString::number(sc->uid())));
		a->setProperty("uid", sc->uid());
		a->setProperty("type", "floating");
		a->setCheckable(true);
		a->setChecked(fw->isVisible());
		connect(a, SIGNAL(toggled(bool)), this, SLOT(onActionToggleSectionContentVisibility(bool)));
		actions.insert(a->text(), a);
	}

	// Create menu from "actions"
	QMenu* m = new QMenu(NULL);
	m->addActions(actions.values());
	return m;
}

QByteArray MainContainerWidget::saveState() const
{
	ADS_NS_SER::InMemoryWriter writer;

	// Hierarchy data.
	const QByteArray hierarchyData = saveHierarchy();
	if (!hierarchyData.isEmpty())
	{
		writer.write(ADS_NS_SER::ET_Hierarchy, hierarchyData);
	}

	// SectionIndex data.
	ADS_NS_SER::SectionIndexData sid;
	if (saveSectionIndex(sid))
	{
		writer.write(sid);
	}

	if (writer.offsetsCount() == 0)
		return QByteArray();
	return writer.toByteArray();
}

bool MainContainerWidget::restoreState(const QByteArray& data)
{
	if (data.isEmpty())
		return false;

	ADS_NS_SER::InMemoryReader reader(data);
	if (!reader.initReadHeader())
		return false;

	// Basic hierarchy data.
	QByteArray hierarchyData;
	if (reader.read(ADS_NS_SER::ET_Hierarchy, hierarchyData))
	{
		restoreHierarchy(hierarchyData);
	}
	return true;
}


QList<SectionContent::RefPtr> MainContainerWidget::contents() const
{
	QList<SectionContent::WeakPtr> wl = m_SectionContentIdMap.values();
	QList<SectionContent::RefPtr> sl;
	for (int i = 0; i < wl.count(); ++i)
	{
		const SectionContent::RefPtr sc = wl.at(i).toStrongRef();
		if (sc)
			sl.append(sc);
	}
	return sl;
}

QPointer<DropOverlay> MainContainerWidget::sectionDropOverlay() const
{
	return m_SectionDropOverlay;
}


QPointer<DropOverlay> MainContainerWidget::dropOverlay() const
{
	return m_ContainerDropOverlay;
}

///////////////////////////////////////////////////////////////////////
// PRIVATE API BEGINS HERE
///////////////////////////////////////////////////////////////////////

SectionWidget* MainContainerWidget::dropContent(const InternalContentData& data, SectionWidget* targetSectionWidget, DropArea area, bool autoActive)
{
	ADS_Expects(targetSection != NULL);

	SectionWidget* section_widget = nullptr;

	// If no sections exists yet, create a default one and always drop into it.
	if (m_Sections.isEmpty())
	{
		targetSectionWidget = newSectionWidget();
		addSectionWidget(targetSectionWidget);
		area = CenterDropArea;
	}

	// Drop on outer area
	if (!targetSectionWidget)
	{
		switch (area)
		{
		case TopDropArea:return dropContentOuterHelper(m_MainLayout, data, Qt::Vertical, false);
		case RightDropArea: return dropContentOuterHelper(m_MainLayout, data, Qt::Horizontal, true);
		case CenterDropArea:
		case BottomDropArea:return dropContentOuterHelper(m_MainLayout, data, Qt::Vertical, true);
		case LeftDropArea: return dropContentOuterHelper(m_MainLayout, data, Qt::Horizontal, false);
		default:
			return nullptr;
		}
		return section_widget;
	}

	// Drop logic based on area.
	switch (area)
	{
	case TopDropArea:return insertNewSectionWidget(data, targetSectionWidget, section_widget, Qt::Vertical, 0);
	case RightDropArea: return insertNewSectionWidget(data, targetSectionWidget, section_widget, Qt::Horizontal, 1);
	case BottomDropArea: return insertNewSectionWidget(data, targetSectionWidget, section_widget, Qt::Vertical, 1);
	case LeftDropArea: return insertNewSectionWidget(data, targetSectionWidget, section_widget, Qt::Horizontal, 0);
	case CenterDropArea:
		 targetSectionWidget->addContent(data, autoActive);
		 return targetSectionWidget;

	default:
		break;
	}
	return section_widget;
}


SectionWidget* MainContainerWidget::sectionWidgetAt(const QPoint& pos) const
{
	const QPoint gpos = mapToGlobal(pos);
	for (int i = 0; i < m_Sections.size(); ++i)
	{
		SectionWidget* sw = m_Sections[i];
		if (sw->rect().contains(sw->mapFromGlobal(gpos)))
		{
			return sw;
		}
	}
	return 0;
}



QByteArray MainContainerWidget::saveHierarchy() const
{
	/*
		# Data Format

		quint32                   Magic
		quint32                   Version

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
	*/
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	out << (quint32) 0x00001337; // Magic
	out << (quint32) 1; // Version

	// Save state of floating contents
	saveFloatingWidgets(out);

	// Save state of sections and contents
	if (m_MainLayout->count() <= 0 || m_Sections.isEmpty())
	{
		// Looks like the user has hidden all contents and no more sections
		// are available. We can simply write a list of all hidden contents.
		out << 0; // Mode

		out << m_HiddenSectionContents.count();
		QHashIterator<int, HiddenSectionItem> iter(m_HiddenSectionContents);
		while (iter.hasNext())
		{
			iter.next();
			out << iter.value().data.content->uniqueName();
		}
	}
	else if (m_MainLayout->count() == 1)
	{
		out << 1; // Mode

		// There should only be one!
		QLayoutItem* li = m_MainLayout->itemAt(0);
		if (!li->widget())
			qFatal("Not a widget in mainLayout, this shouldn't happen.");

		// Save sections beginning with the first QSplitter (li->widget()).
		saveSectionWidgets(out, li->widget());

		// Safe state of hidden contents, which doesn't have an section association
		// or the section association points to a no longer existing section.
		QHashIterator<int, HiddenSectionItem> iter(m_HiddenSectionContents);
		int cnt = 0;
		while (iter.hasNext())
		{
			iter.next();
			if (iter.value().preferredSectionId <= 0 || !m_SectionWidgetIdMap.contains(iter.value().preferredSectionId))
				cnt++;
		}
		out << cnt;
		iter.toFront();
		while (iter.hasNext())
		{
			iter.next();
			if (iter.value().preferredSectionId <= 0 || !m_SectionWidgetIdMap.contains(iter.value().preferredSectionId))
				out << iter.value().data.content->uniqueName();
		}
	}
	else
	{
		// More? Oh oh.. something is wrong :-/
		out << -1;
		qWarning() << "Oh noooz.. Something went wrong. There are too many items in mainLayout.";
	}
	return ba;
}

void MainContainerWidget::saveFloatingWidgets(QDataStream& out) const
{
	out << m_Floatings.count();
	for (int i = 0; i < m_Floatings.count(); ++i)
	{
		FloatingWidget* fw = m_Floatings.at(i);
		out << fw->content()->uniqueName();
		out << fw->saveGeometry();
		out << fw->isVisible();
	}
}

void MainContainerWidget::saveSectionWidgets(QDataStream& out, QWidget* widget) const
{
	QSplitter* sp = NULL;
	SectionWidget* sw = NULL;

	if (!widget)
	{
		out << 0;
	}
	else if ((sp = dynamic_cast<QSplitter*>(widget)) != NULL)
	{
		out << 1; // Type = QSplitter
		out << ((sp->orientation() == Qt::Horizontal) ? (int) 1 : (int) 2);
		out << sp->count();
		out << sp->sizes();
		for (int i = 0; i < sp->count(); ++i)
		{
			saveSectionWidgets(out, sp->widget(i));
		}
	}
	else if ((sw = dynamic_cast<SectionWidget*>(widget)) != NULL)
	{
		// Format (version 1)
		//	int  Object type (SectionWidget=2)
		//	int  Current active index
		//	int  Number of contents (visible + hidden)
		//	LOOP Contents of section (last int)
		//		QString  Unique name of SectionContent
		//		bool     Visibility
		//		int      Preferred index

		const QList<SectionContent::RefPtr>& contents = sw->contents();
		QList<HiddenSectionItem> hiddenContents;

		QHashIterator<int, HiddenSectionItem> iter(m_HiddenSectionContents);
		while (iter.hasNext())
		{
			iter.next();
			const HiddenSectionItem& hsi = iter.value();
			if (hsi.preferredSectionId != sw->uid())
				continue;
			hiddenContents.append(hsi);
		}

		out << 2; // Type = SectionWidget
		out << sw->currentIndex();
		out << contents.count() + hiddenContents.count();

		for (int i = 0; i < contents.count(); ++i)
		{
			out << contents[i]->uniqueName(); // Unique name
			out << (bool) true; // Visiblity
			out << i; // Index
		}
		for (int i = 0; i < hiddenContents.count(); ++i)
		{
			out << hiddenContents.at(i).data.content->uniqueName();
			out << (bool) false;
			out << hiddenContents.at(i).preferredSectionIndex;
		}
	}
}

bool MainContainerWidget::saveSectionIndex(ADS_NS_SER::SectionIndexData& sid) const
{
	if (m_Sections.count() <= 0)
		return false;

	sid.sectionsCount = m_Sections.count();
	for (int i = 0; i < sid.sectionsCount; ++i)
	{
		ADS_NS_SER::SectionEntity se;
		se.x = mapFromGlobal(m_Sections[i]->parentWidget()->mapToGlobal(m_Sections[i]->pos())).x();
		se.y = mapFromGlobal(m_Sections[i]->parentWidget()->mapToGlobal(m_Sections[i]->pos())).y();
		se.width = m_Sections[i]->geometry().width();
		se.height = m_Sections[i]->geometry().height();
		se.currentIndex = m_Sections[i]->currentIndex();
		se.sectionContentsCount = m_Sections[i]->contents().count();
		foreach (const SectionContent::RefPtr& sc, m_Sections[i]->contents())
		{
			ADS_NS_SER::SectionContentEntity sce;
			sce.uniqueName = sc->uniqueName();
			sce.visible = true;
			sce.preferredIndex = m_Sections[i]->indexOfContent(sc);
			se.sectionContents.append(sce); // std::move()?
		}
		sid.sections.append(se); // std::move()?
	}
	return true;
}

bool MainContainerWidget::restoreHierarchy(const QByteArray& data)
{
	QDataStream in(data);
	in.setVersion(QDataStream::Qt_4_5);

	quint32 magic = 0;
	in >> magic;
	if (magic != 0x00001337)
		return false;

	quint32 version = 0;
	in >> version;
	if (version != 1)
		return false;

	QList<FloatingWidget*> oldFloatings = m_Floatings;
	QList<SectionWidget*> oldSections = m_Sections;

	// Restore floating widgets
	QList<FloatingWidget*> floatings;
	bool success = restoreFloatingWidgets(in, version, floatings);
	if (!success)
	{
		qWarning() << "Could not restore floatings completely";
	}

	// Restore splitters, sections and contents
	QList<SectionWidget*> sections;
	QList<SectionContent::RefPtr> contentsToHide;

	int mode = 0;
	in >> mode;
	if (mode == 0)
	{
		// List of hidden contents. There are no sections at all.
		int cnt = 0;
		in >> cnt;

		if(cnt > 0)
		{
			// Create dummy section, required to call hideSectionContent() later.
			SectionWidget* sw = new SectionWidget(this, this);
			sections.append(sw);

			for (int i = 0; i < cnt; ++i)
			{
				QString uname;
				in >> uname;

				const SectionContent::RefPtr sc = m_SectionContentNameMap.value(uname);
				if (!sc)
					continue;

				InternalContentData data;
				if (!takeContent(sc, data))
					qFatal("This should never happen!!!");

				sw->addContent(data, false);
				contentsToHide.append(sc);
			}
		}
	}
	else if (mode == 1)
	{
		success = restoreSectionWidgets(in, version, NULL, sections, contentsToHide);
		if (!success)
			qWarning() << "Could not restore sections completely";

		// Restore lonely hidden contents
		int cnt = 0;
		in >> cnt;
		for (int i = 0; i < cnt; ++i)
		{
			QString uname;
			in >> uname;
			const SectionContent::RefPtr sc = m_SectionContentNameMap.value(uname);
			if (!sc)
				continue;

			InternalContentData data;
			if (!takeContent(sc, data))
			{
				qWarning("This should never happen! Looks like a bug during serialization, since the content is already being used in SWs.");
				continue;
			}

			SectionWidget* sw = NULL;
			if (sections.size() <= 0)
				qFatal("This should never happen, because above a section should have been created.");
			else
				sw = sections.first();

			sw->addContent(data, false);
			contentsToHide.append(sc);
		}
	}

	// Handle SectionContent which is not mentioned by deserialized data.
	// What shall we do with it? For now: Simply drop them into the first SectionWidget.
	if (true)
	{
		QList<SectionContent::RefPtr> leftContents;

		// Collect all contents which has been restored
		QList<SectionContent::RefPtr> contents;
		for (int i = 0; i < floatings.count(); ++i)
			contents.append(floatings.at(i)->content());
		for (int i = 0; i < sections.count(); ++i)
			for (int j = 0; j < sections.at(i)->contents().count(); ++j)
				contents.append(sections.at(i)->contents().at(j));
		for (int i = 0; i < contentsToHide.count(); ++i)
			contents.append(contentsToHide.at(i));

		// Compare restored contents with available contents
		const QList<SectionContent::WeakPtr> allContents = m_SectionContentIdMap.values();
		for (int i = 0; i < allContents.count(); ++i)
		{
			const SectionContent::RefPtr sc = allContents.at(i).toStrongRef();
			if (sc.isNull() || sc->containerWidget() != this)
				continue;
			if (contents.contains(sc))
				continue;
			leftContents.append(sc);
		}

		// What should we do with a drunken sailor.. what should.. erm..
		// What should we do with the left-contents?
		// Lets add them to the first found SW or create one, if no SW is available.
		for (int i = 0; i < leftContents.count(); ++i)
		{
			const SectionContent::RefPtr sc = leftContents.at(i);
			SectionWidget* sw = NULL;

			if (sections.isEmpty())
			{
				sw = new SectionWidget(this, this);
				sections.append(sw);
				addSectionWidget(sw);
			}
			else
				sw = sections.first();

			InternalContentData data;
			if (!takeContent(sc, data))
				sw->addContent(sc);
			else
				sw->addContent(data, false);
		}
	}

	m_Floatings = floatings;
	m_Sections = sections;

	// Delete old objects
	QLayoutItem* old = m_MainLayout->takeAt(0);
	m_MainLayout->addWidget(m_Splitter);
	delete old;
	qDeleteAll(oldFloatings);
	qDeleteAll(oldSections);

	// Hide all as "hidden" marked contents
	for (int i = 0; i < contentsToHide.count(); ++i)
		hideSectionContent(contentsToHide.at(i));

	deleteEmptySplitter(this);
	return success;
}

bool MainContainerWidget::restoreFloatingWidgets(QDataStream& in, int version, QList<FloatingWidget*>& floatings)
{
	Q_UNUSED(version)

	int fwCount = 0;
	in >> fwCount;
	if (fwCount <= 0)
		return true;

	for (int i = 0; i < fwCount; ++i)
	{
		QString uname;
		in >> uname;
		QByteArray geom;
		in >> geom;
		bool visible = false;
		in >> visible;

		const SectionContent::RefPtr sc = m_SectionContentNameMap.value(uname).toStrongRef();
		if (!sc)
		{
			qWarning() << "Can not find SectionContent:" << uname;
			continue;
		}

		InternalContentData data;
		if (!this->takeContent(sc, data))
			continue;

		FloatingWidget* fw = new FloatingWidget(this, sc, data.titleWidget, data.contentWidget, this);
		fw->restoreGeometry(geom);
		fw->setVisible(visible);
		if (visible)
		{
			fw->_titleWidget->setVisible(visible);
			fw->_contentWidget->setVisible(visible);
		}
		floatings.append(fw);
		data.titleWidget->m_FloatingWidget = fw; // $mfreiholz: Don't look at it :-< It's more than ugly...
	}
	return true;
}

bool MainContainerWidget::restoreSectionWidgets(QDataStream& in, int version, QSplitter* currentSplitter, QList<SectionWidget*>& sections, QList<SectionContent::RefPtr>& contentsToHide)
{
	if (in.atEnd())
		return true;

	int type;
	in >> type;

	// Splitter
	if (type == 1)
	{
		int orientation, count;
		QList<int> sizes;
		in >> orientation >> count >> sizes;

		QSplitter* sp = newSplitter((Qt::Orientation) orientation);
		for (int i = 0; i < count; ++i)
		{
			if (!restoreSectionWidgets(in, version, sp, sections, contentsToHide))
				return false;
		}
		if (sp->count() <= 0)
		{
			delete sp;
			sp = NULL;
		}
		else if (sp)
		{
			sp->setSizes(sizes);

			if (!currentSplitter)
				m_Splitter = sp;
			else
				currentSplitter->addWidget(sp);
		}
	}
	// Section
	else if (type == 2)
	{
		if (!currentSplitter)
		{
			qWarning() << "Missing splitter object for section";
			return false;
		}

		int currentIndex, count;
		in >> currentIndex >> count;

		SectionWidget* sw = new SectionWidget(this, this);
		for (int i = 0; i < count; ++i)
		{
			QString uname;
			in >> uname;
			bool visible = false;
			in >> visible;
			int preferredIndex = -1;
			in >> preferredIndex;

			const SectionContent::RefPtr sc = m_SectionContentNameMap.value(uname).toStrongRef();
			if (!sc)
			{
				qWarning() << "Can not find SectionContent:" << uname;
				continue;
			}

			InternalContentData data;
			if (!takeContent(sc, data))
			{
				qCritical() << "Can not find InternalContentData of SC, this should never happen!" << sc->uid() << sc->uniqueName();
				sw->addContent(sc);
			}
			else
				sw->addContent(data, false);

			if (!visible)
				contentsToHide.append(sc);
		}
		if (sw->contents().isEmpty())
		{
			delete sw;
			sw = NULL;
		}
		else if (sw)
		{
			sw->setCurrentIndex(currentIndex);
			currentSplitter->addWidget(sw);
			sections.append(sw);
		}
	}
	// Unknown
	else
	{
		qWarning() << "Unknown object type during restore";
	}

	return true;
}

bool MainContainerWidget::takeContent(const SectionContent::RefPtr& sc, InternalContentData& data)
{
	ADS_Expects(!sc.isNull());

	// Search in sections
	bool found = false;
	for (int i = 0; i < m_Sections.count() && !found; ++i)
	{
		found = m_Sections.at(i)->takeContent(sc->uid(), data);
	}

	// Search in floating widgets
	for (int i = 0; i < m_Floatings.count() && !found; ++i)
	{
		found = m_Floatings.at(i)->content()->uid() == sc->uid();
		if (found)
			m_Floatings.at(i)->takeContent(data);
	}

	// Search in hidden items
	if (!found && m_HiddenSectionContents.contains(sc->uid()))
	{
		const HiddenSectionItem hsi = m_HiddenSectionContents.take(sc->uid());
		data = hsi.data;
		found = true;
	}

	return found;
}

void MainContainerWidget::onActiveTabChanged()
{
	SectionTitleWidget* stw = qobject_cast<SectionTitleWidget*>(sender());
	if (stw)
	{
		emit activeTabChanged(stw->m_Content, stw->isActiveTab());
	}
}

void MainContainerWidget::onActionToggleSectionContentVisibility(bool visible)
{
	QAction* a = qobject_cast<QAction*>(sender());
	if (!a)
		return;
	const int uid = a->property("uid").toInt();
	const SectionContent::RefPtr sc = m_SectionContentIdMap.value(uid).toStrongRef();
	if (sc.isNull())
	{
		qCritical() << "Can not find content by ID" << uid;
		return;
	}
	if (visible)
		showSectionContent(sc);
	else
		hideSectionContent(sc);
}


void MainContainerWidget::hideContainerOverlay()
{
	m_ContainerDropOverlay->hideDropOverlay();
}


FloatingWidget* MainContainerWidget::startFloating(SectionWidget* sectionwidget, int ContentUid, const QPoint& TargetPos)
{
    // Create floating widget.
    InternalContentData data;
    if (!sectionwidget->takeContent(ContentUid, data))
    {
        qWarning() << "THIS SHOULD NOT HAPPEN!!" << ContentUid;
        return 0;
    }

    FloatingWidget* fw = new FloatingWidget(this, data.content, data.titleWidget, data.contentWidget, this);
    fw->resize(sectionwidget->size());
    fw->move(TargetPos);
    fw->show();
    fw->setObjectName("FloatingWidget");

    // Delete old section, if it is empty now.
    if (sectionwidget->contents().isEmpty())
    {
        delete sectionwidget;
        sectionwidget = NULL;
    }
    deleteEmptySplitter(this);

	m_ContainerDropOverlay->setAllowedAreas(OuterAreas);
	m_ContainerDropOverlay->showDropOverlay(this);
	m_ContainerDropOverlay->raise();
    return fw;
}


bool MainContainerWidget::event(QEvent *e)
{
	//std::cout << "ContainerWidget::event " << e->type() << std::endl;
	return QFrame::event(e);
}

void MainContainerWidget::moveFloatingWidget(const QPoint& TargetPos)
{
    QPoint MousePos = mapFromGlobal(QCursor::pos());

	// Mouse is over the container widget
    if (rect().contains(MousePos))
    {
    	//std::cout << "over Container" << std::endl;
    	m_ContainerDropOverlay->showDropOverlay(this);
    	m_ContainerDropOverlay->raise();
    }
    else
    {
    	std::cout << "-----------------" << std::endl;
    	m_ContainerDropOverlay->hideDropOverlay();
    }

    // Mouse is over a SectionWidget
    SectionWidget* sectionwidget = sectionWidgetAt(MousePos);
    if (sectionwidget)
    {
        //qInfo() << "over sectionWidget";
        m_SectionDropOverlay->setAllowedAreas(ADS_NS::AllAreas);
        m_SectionDropOverlay->showDropOverlay(sectionwidget);
    }
    else
    {
        m_SectionDropOverlay->hideDropOverlay();
    }
}


ADS_NAMESPACE_END
