#include "ads/ContainerWidget.h"
#include "ads/Internal.h"
#include "ads/SectionTitleWidget.h"
#include "ads/SectionContentWidget.h"

#include <QPaintEvent>
#include <QPainter>
#include <QContextMenuEvent>
#include <QMenu>
#include <QSplitter>
#include <QDataStream>
#include <QtGlobal>

ADS_NAMESPACE_BEGIN

// Static Helper //////////////////////////////////////////////////////

static QSplitter* newSplitter(Qt::Orientation orientation = Qt::Horizontal, QWidget* parent = 0)
{
	QSplitter* s = new QSplitter(orientation, parent);
	s->setChildrenCollapsible(false);
	s->setOpaqueResize(false);
	return s;
}

///////////////////////////////////////////////////////////////////////

ContainerWidget::ContainerWidget(QWidget *parent) :
	QFrame(parent),
	_mainLayout(NULL),
	_orientation(Qt::Horizontal),
	_splitter(NULL)
{
	_mainLayout = new QGridLayout();
	_mainLayout->setContentsMargins(9, 9, 9, 9);
	_mainLayout->setSpacing(0);
	setLayout(_mainLayout);
}

Qt::Orientation ContainerWidget::orientation() const
{
	return _orientation;
}

void ContainerWidget::setOrientation(Qt::Orientation orientation)
{
	if (_orientation != orientation)
	{
		_orientation = orientation;
		emit orientationChanged();
	}
}

SectionWidget* ContainerWidget::addSectionContent(const SectionContent::RefPtr& sc, SectionWidget* sw, DropArea area)
{
	if (!sw)
	{
		if (_sections.isEmpty())
		{	// Create default section
			sw = newSectionWidget();
			addSection(sw);
		}
		else if (area == CenterDropArea)
			// Use existing default section
			sw = _sections.first();
	}

	// Drop it based on "area"
	InternalContentData data;
	data.content = sc;
	data.titleWidget = new SectionTitleWidget(sc, NULL);
	data.contentWidget = new SectionContentWidget(sc, NULL);
	return dropContent(data, sw, area, false);
}

bool ContainerWidget::showSectionContent(const SectionContent::RefPtr& sc)
{
	// Search SC in floatings
	for (int i = 0; i < _floatings.count(); ++i)
	{
		const bool found = _floatings.at(i)->content()->uid() == sc->uid();
		if (!found)
			continue;
		_floatings.at(i)->setVisible(true);
		return true;
	}

	// Search SC in hidden sections
	// Try to show them in the last position, otherwise simply append
	// it to the first section (or create a new section?)
	if (_hiddenSectionContents.contains(sc->uid()))
	{
		const HiddenSectionItem hsi = _hiddenSectionContents.take(sc->uid());
		hsi.data.titleWidget->setVisible(true);
		hsi.data.contentWidget->setVisible(true);
		SectionWidget* sw = NULL;
		if (hsi.preferredSectionId > 0 && (sw = SectionWidget::GetLookupMap().value(hsi.preferredSectionId)) != NULL)
		{
			sw->addContent(hsi.data, true);
			return true;
		}
		else if (_sections.size() > 0 && (sw = _sections.first()) != NULL)
		{
			sw->addContent(hsi.data, true);
			return true;
		}
		else
		{
			sw = newSectionWidget();
			addSection(sw);
			sw->addContent(hsi.data, true);
			return true;
		}
	}

	// Already visible?
	// TODO
	qDebug("Unable to show SectionContent, don't know where 8-/ (already visible?)");
	return false;
}

bool ContainerWidget::hideSectionContent(const SectionContent::RefPtr& sc)
{
	// Search SC in floatings
	// We can simply hide floatings, nothing else required.
	for (int i = 0; i < _floatings.count(); ++i)
	{
		const bool found = _floatings.at(i)->content()->uid() == sc->uid();
		if (!found)
			continue;
		_floatings.at(i)->setVisible(false);
		return true;
	}

	// Search SC in sections
	// It's required to remove the SC from SW completely and hold it in a
	// separate list as long as a "showSectionContent" gets called for the SC again.
	// In case that the SW does not have any other SCs, we need to delete it.
	for (int i = 0; i < _sections.count(); ++i)
	{
		SectionWidget* sw = _sections.at(i);
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
		_hiddenSectionContents.insert(sc->uid(), hsi);

		if (sw->contents().isEmpty())
		{
			delete sw;
			sw = NULL;
			deleteEmptySplitter(this);
		}
		return true;
	}

	// Search SC in hidden elements
	// The content may already be hidden
	if (_hiddenSectionContents.contains(sc->uid()))
		return true;

	qFatal("Unable to hide SectionContent, don't know this one 8-/");
	return false;
}

bool ContainerWidget::raiseSectionContent(const SectionContent::RefPtr& sc)
{
	// Search SC in sections
	for (int i = 0; i < _sections.count(); ++i)
	{
		SectionWidget* sw = _sections.at(i);
		int index = sw->indexOfContent(sc);
		if (index < 0)
			continue;
		sw->setCurrentIndex(index);
		return true;
	}

	// Search SC in floatings
	for (int i = 0; i < _floatings.size(); ++i)
	{
		FloatingWidget* fw = _floatings.at(i);
		if (fw->content()->uid() != sc->uid())
			continue;
		fw->setVisible(true);
		fw->raise();
		return true;
	}

	// Search SC in hidden
	// TODO

	return false;
}

QMenu* ContainerWidget::createContextMenu() const
{
	// Fill map with actions (sorted by key!)
	QMap<QString, QAction*> actions;

	// Visible contents of sections
	for (int i = 0; i < _sections.size(); ++i)
	{
		const SectionWidget* sw = _sections.at(i);
		const QList<SectionContent::RefPtr>& contents = sw->contents();
		foreach (const SectionContent::RefPtr& sc, contents)
		{
			QAction* a = new QAction(QIcon(), sc->visibleTitle(), NULL);
			a->setObjectName(QString("ads-action-sc-%1").arg(QString::number(sc->uid())));
			a->setProperty("uid", sc->uid());
			a->setProperty("type", "section");
			a->setCheckable(true);
			a->setChecked(true);
#if QT_VERSION >= 0x050000
			QObject::connect(a, &QAction::toggled, this, &ContainerWidget::onActionToggleSectionContentVisibility);
#else
			QObject::connect(a, SIGNAL(toggled(bool)), this, SLOT(onActionToggleSectionContentVisibility(bool)));
#endif
			actions.insert(a->text(), a);
		}
	}

	// Hidden contents of sections
	QHashIterator<int, HiddenSectionItem> hiddenIter(_hiddenSectionContents);
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
#if QT_VERSION >= 0x050000
			QObject::connect(a, &QAction::toggled, this, &ContainerWidget::onActionToggleSectionContentVisibility);
#else
			QObject::connect(a, SIGNAL(toggled(bool)), this, SLOT(onActionToggleSectionContentVisibility(bool)));
#endif
		actions.insert(a->text(), a);
	}

	// Floating contents
	for (int i = 0; i < _floatings.size(); ++i)
	{
		const FloatingWidget* fw = _floatings.at(i);
		const SectionContent::RefPtr sc = fw->content();

		QAction* a = new QAction(QIcon(), sc->visibleTitle(), NULL);
		a->setObjectName(QString("ads-action-sc-%1").arg(QString::number(sc->uid())));
		a->setProperty("uid", sc->uid());
		a->setProperty("type", "floating");
		a->setCheckable(true);
		a->setChecked(fw->isVisible());
#if QT_VERSION >= 0x050000
		QObject::connect(a, &QAction::toggled, fw, &FloatingWidget::setVisible);
#else
		QObject::connect(a, SIGNAL(toggled(bool)), fw, SLOT(setVisible(bool)));
#endif
		actions.insert(a->text(), a);
	}

	// Create menu from "actions"
	QMenu* m = new QMenu(NULL);
	m->addActions(actions.values());
	return m;
}

QByteArray ContainerWidget::saveState() const
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
	qDebug() << "Begin save state";

	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_5);
	out << (quint32) 0x00001337; // Magic
	out << (quint32) 1; // Version

	// Save state of floating contents
	saveFloatingWidgets(out);

	// Save state of sections and contents
	if (_mainLayout->count() <= 0 || _sections.isEmpty())
	{
		// Looks like the user has hidden all contents and no more sections
		// are available. We can simply write a list of all hidden contents.
		out << 0;
		out << _hiddenSectionContents.count();

		QHashIterator<int, HiddenSectionItem> iter(_hiddenSectionContents);
		while (iter.hasNext())
		{
			iter.next();
			out << iter.value().data.content->uniqueName();
		}
	}
	else if (_mainLayout->count() == 1)
	{
		// There should only be one!
		out << 1;
		QLayoutItem* li = _mainLayout->itemAt(0);
		if (!li->widget())
			qWarning() << "Not a widget in _mainLayout, this shouldn't happen.";
		else
			saveSectionWidgets(out, li->widget());

		// Safe state of hidden contents, which doesn't have an section association
		// or the section association points to a no longer existing section.
		QHashIterator<int, HiddenSectionItem> iter(_hiddenSectionContents);
		int cnt = 0;
		while (iter.hasNext())
		{
			iter.next();
			if (iter.value().preferredSectionId <= 0 || !SectionWidget::GetLookupMap().contains(iter.value().preferredSectionId))
				cnt++;
		}
		out << cnt;
		iter.toFront();
		while (iter.hasNext())
		{
			iter.next();
			if (iter.value().preferredSectionId <= 0 || !SectionWidget::GetLookupMap().contains(iter.value().preferredSectionId))
				out << iter.value().data.content->uniqueName();
		}
	}
	else
	{
		// More? Oh oh.. something is wrong :-/
		out << -1;
		qWarning() << "Oh noooz.. Something went wrong. There are too many items in _mainLayout.";
	}
	qDebug() << "End save state";
	return ba;
}

bool ContainerWidget::restoreState(const QByteArray& data)
{
	if (data.isEmpty())
		return false;

	qDebug() << "Begin to restore state";

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

	QList<FloatingWidget*> oldFloatings = _floatings;
	QList<SectionWidget*> oldSections = _sections;

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

		// Create dummy section, required to call hideSectionContent() later.
		SectionWidget* sw = new SectionWidget(this);
		sections.append(sw);

		for (int i = 0; i < cnt; ++i)
		{
			QString uname;
			in >> uname;

			const SectionContent::RefPtr sc = SectionContent::GetLookupMapByName().value(uname);
			if (!sc)
				continue;

			InternalContentData data;
			if (!takeContent(sc, data))
				qFatal("This should never happen!!!");

			sw->addContent(data, false);
			contentsToHide.append(sc);
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
			const SectionContent::RefPtr sc = SectionContent::GetLookupMapByName().value(uname);
			if (!sc)
				continue;

			InternalContentData data;
			if (!takeContent(sc, data))
				qFatal("This should never happen!!!");

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
		const QList<SectionContent::WeakPtr> allContents = SectionContent::GetLookupMap().values();
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
				sw = new SectionWidget(this);
				sections.append(sw);
				addSection(sw);
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

	_floatings = floatings;
	_sections = sections;

	// Delete old objects
	QLayoutItem* old = _mainLayout->takeAt(0);
	_mainLayout->addWidget(_splitter);
	delete old;
	qDeleteAll(oldFloatings);
	qDeleteAll(oldSections);

	// Hide all as "hidden" marked contents
	for (int i = 0; i < contentsToHide.count(); ++i)
		hideSectionContent(contentsToHide.at(i));

	qDebug() << "End of restore state" << success;
	return success;
}

QRect ContainerWidget::outerTopDropRect() const
{
	QRect r = rect();
	int h = r.height() / 100 * 5;
	return QRect(r.left(), r.top(), r.width(), h);
}

QRect ContainerWidget::outerRightDropRect() const
{
	QRect r = rect();
	int w = r.width() / 100 * 5;
	return QRect(r.right() - w, r.top(), w, r.height());
}

QRect ContainerWidget::outerBottomDropRect() const
{
	QRect r = rect();
	int h = r.height() / 100 * 5;
	return QRect(r.left(), r.bottom() - h, r.width(), h);
}

QRect ContainerWidget::outerLeftDropRect() const
{
	QRect r = rect();
	int w = r.width() / 100 * 5;
	return QRect(r.left(), r.top(), w, r.height());
}

///////////////////////////////////////////////////////////////////////
// PRIVATE API BEGINS HERE
///////////////////////////////////////////////////////////////////////

SectionWidget* ContainerWidget::newSectionWidget()
{
	SectionWidget* sw = new SectionWidget(this);
	_sections.append(sw);
	return sw;
}

SectionWidget* ContainerWidget::dropContent(const InternalContentData& data, SectionWidget* targetSection, DropArea area, bool autoActive)
{
	SectionWidget* ret = NULL;

	// Drop on outer area
	if (!targetSection)
	{
		switch (area)
		{
		case TopDropArea:
			ret = dropContentOuterHelper(_mainLayout, data, Qt::Vertical, false);
			break;
		case RightDropArea:
			ret = dropContentOuterHelper(_mainLayout, data, Qt::Horizontal, true);
			break;
		case BottomDropArea:
			ret = dropContentOuterHelper(_mainLayout, data, Qt::Vertical, true);
			break;
		case LeftDropArea:
			ret = dropContentOuterHelper(_mainLayout, data, Qt::Horizontal, false);
			break;
		default:
			return NULL;
		}
		return NULL;
	}

	QSplitter* targetSectionSplitter = findParentSplitter(targetSection);

	// Drop logic based on area.
	switch (area)
	{
	case TopDropArea:
	{
		SectionWidget* sw = newSectionWidget();
		sw->addContent(data, true);
		if (targetSectionSplitter->orientation() == Qt::Vertical)
		{
			const int index = targetSectionSplitter->indexOf(targetSection);
			targetSectionSplitter->insertWidget(index, sw);
		}
		else
		{
			const int index = targetSectionSplitter->indexOf(targetSection);
			QSplitter* s = newSplitter(Qt::Vertical);
			s->addWidget(sw);
			s->addWidget(targetSection);
			targetSectionSplitter->insertWidget(index, s);
		}
		ret = sw;
		break;
	}
	case RightDropArea:
	{
		SectionWidget* sw = newSectionWidget();
		sw->addContent(data, true);
		if (targetSectionSplitter->orientation() == Qt::Horizontal)
		{
			const int index = targetSectionSplitter->indexOf(targetSection);
			targetSectionSplitter->insertWidget(index + 1, sw);
		}
		else
		{
			const int index = targetSectionSplitter->indexOf(targetSection);
			QSplitter* s = newSplitter(Qt::Horizontal);
			s->addWidget(targetSection);
			s->addWidget(sw);
			targetSectionSplitter->insertWidget(index, s);
		}
		ret = sw;
		break;
	}
	case BottomDropArea:
	{
		SectionWidget* sw = newSectionWidget();
		sw->addContent(data, true);
		if (targetSectionSplitter->orientation() == Qt::Vertical)
		{
			int index = targetSectionSplitter->indexOf(targetSection);
			targetSectionSplitter->insertWidget(index + 1, sw);
		}
		else
		{
			int index = targetSectionSplitter->indexOf(targetSection);
			QSplitter* s = newSplitter(Qt::Vertical);
			s->addWidget(targetSection);
			s->addWidget(sw);
			targetSectionSplitter->insertWidget(index, s);
		}
		ret = sw;
		break;
	}
	case LeftDropArea:
	{
		SectionWidget* sw = newSectionWidget();
		sw->addContent(data, true);
		if (targetSectionSplitter->orientation() == Qt::Horizontal)
		{
			int index = targetSectionSplitter->indexOf(targetSection);
			targetSectionSplitter->insertWidget(index, sw);
		}
		else
		{
			QSplitter* s = newSplitter(Qt::Horizontal);
			s->addWidget(sw);
			int index = targetSectionSplitter->indexOf(targetSection);
			targetSectionSplitter->insertWidget(index, s);
			s->addWidget(targetSection);
		}
		ret = sw;
		break;
	}
	case CenterDropArea:
	{
		targetSection->addContent(data, autoActive);
		ret = targetSection;
		break;
	}
	default:
		break;
	}
	return ret;
}

void ContainerWidget::addSection(SectionWidget* section)
{
	// Create default splitter.
	if (!_splitter)
	{
		_splitter = newSplitter(_orientation);
		_mainLayout->addWidget(_splitter, 0, 0);
	}
	if (_splitter->indexOf(section) != -1)
	{
		qWarning() << Q_FUNC_INFO << QString("Section has already been added");
		return;
	}
	_splitter->addWidget(section);
}

SectionWidget* ContainerWidget::sectionAt(const QPoint& pos) const
{
	const QPoint gpos = mapToGlobal(pos);
	for (int i = 0; i < _sections.size(); ++i)
	{
		SectionWidget* sw = _sections[i];
		if (sw->rect().contains(sw->mapFromGlobal(gpos)))
		{
			return sw;
		}
	}
	return 0;
}

SectionWidget* ContainerWidget::dropContentOuterHelper(QLayout* l, const InternalContentData& data, Qt::Orientation orientation, bool append)
{
	SectionWidget* sw = newSectionWidget();
	sw->addContent(data, true);

	QSplitter* oldsp = findImmediateSplitter(this);
	if (!oldsp)
	{
		QSplitter* sp = newSplitter(orientation);
		if (l->count() > 0)
		{
			qWarning() << "Still items in layout. This should never happen.";
			QLayoutItem* li = l->takeAt(0);
			delete li;
		}
		l->addWidget(sp);
		sp->addWidget(sw);
	}
	else if (oldsp->orientation() == orientation
			|| oldsp->count() == 1)
	{
		oldsp->setOrientation(orientation);
		if (append)
			oldsp->addWidget(sw);
		else
			oldsp->insertWidget(0, sw);
	}
	else
	{
		QSplitter* sp = newSplitter(orientation);
		if (append)
		{
#if QT_VERSION >= 0x050000
			QLayoutItem* li = l->replaceWidget(oldsp, sp);
			sp->addWidget(oldsp);
			sp->addWidget(sw);
			delete li;
#else
			int index = l->indexOf(oldsp);
			QLayoutItem* li = l->takeAt(index);
			l->addWidget(sp);
			sp->addWidget(oldsp);
			sp->addWidget(sw);
			delete li;
#endif
		}
		else
		{
#if QT_VERSION >= 0x050000
			sp->addWidget(sw);
			QLayoutItem* li = l->replaceWidget(oldsp, sp);
			sp->addWidget(oldsp);
			delete li;
#else
			sp->addWidget(sw);
			int index = l->indexOf(oldsp);
			QLayoutItem* li = l->takeAt(index);
			l->addWidget(sp);
			sp->addWidget(oldsp);
			delete li;
#endif
		}
	}
	return sw;
}

void ContainerWidget::saveFloatingWidgets(QDataStream& out) const
{
	out << _floatings.count();
	for (int i = 0; i < _floatings.count(); ++i)
	{
		FloatingWidget* fw = _floatings.at(i);
		out << fw->content()->uniqueName();
		out << fw->saveGeometry();
		out << fw->isVisible();
	}
}

void ContainerWidget::saveSectionWidgets(QDataStream& out, QWidget* widget) const
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

		QHashIterator<int, HiddenSectionItem> iter(_hiddenSectionContents);
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

bool ContainerWidget::restoreFloatingWidgets(QDataStream& in, int version, QList<FloatingWidget*>& floatings)
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
		qDebug() << "Restore FloatingWidget" << uname << geom << visible;

		const SectionContent::RefPtr sc = SectionContent::GetLookupMapByName().value(uname).toStrongRef();
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
		floatings.append(fw);
		data.titleWidget->_fw = fw; // $mfreiholz: Don't look at it :-< It's more than ugly...
	}
	return true;
}

bool ContainerWidget::restoreSectionWidgets(QDataStream& in, int version, QSplitter* currentSplitter, QList<SectionWidget*>& sections, QList<SectionContent::RefPtr>& contentsToHide)
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
				_splitter = sp;
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

		SectionWidget* sw = new SectionWidget(this);
		for (int i = 0; i < count; ++i)
		{
			QString uname;
			in >> uname;
			bool visible = false;
			in >> visible;
			int preferredIndex = -1;
			in >> preferredIndex;

			const SectionContent::RefPtr sc = SectionContent::GetLookupMapByName().value(uname).toStrongRef();
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
		qDebug() << QString();
	}

	return true;
}

bool ContainerWidget::takeContent(const SectionContent::RefPtr& sc, InternalContentData& data)
{
	// Search in sections
	bool found = false;
	for (int i = 0; i < _sections.count() && !found; ++i)
	{
		found = _sections.at(i)->takeContent(sc->uid(), data);
	}

	// Search in floating widgets
	for (int i = 0; i < _floatings.count() && !found; ++i)
	{
		found = _floatings.at(i)->content()->uid() == sc->uid();
		if (found)
			_floatings.at(i)->takeContent(data);
	}

	return found;
}

void ContainerWidget::onActionToggleSectionContentVisibility(bool visible)
{
	QAction* a = qobject_cast<QAction*>(sender());
	if (!a)
		return;
	const int uid = a->property("uid").toInt();
	const SectionContent::RefPtr sc = SectionContent::GetLookupMap().value(uid).toStrongRef();
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

ADS_NAMESPACE_END
