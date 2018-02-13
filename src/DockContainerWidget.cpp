/*******************************************************************************
** Qt Advanced Docking System
** Copyright (C) 2017 Uwe Kindler
** 
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public
** License along with this library; If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


//============================================================================
/// \file   DockContainerWidget.cpp
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Implementation of CDockContainerWidget class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockContainerWidget.h"

#include <iostream>

#include <QEvent>
#include <QList>
#include <QGridLayout>
#include <QPointer>
#include <QVariant>
#include <QDebug>
#include <QXmlStreamWriter>

#include "DockManager.h"
#include "DockAreaWidget.h"
#include "DockWidget.h"
#include "FloatingDockContainer.h"
#include "DockOverlay.h"
#include "DockStateSerialization.h"
#include "ads_globals.h"
#include "DockSplitter.h"


namespace ads
{
static unsigned int zOrderCounter = 0;

/**
 * Helper function to ease insertion of dock area into splitter
 */
static void insertWidgetIntoSplitter(QSplitter* Splitter, QWidget* widget, bool Append)
{
	if (Append)
	{
		Splitter->addWidget(widget);
	}
	else
	{
		Splitter->insertWidget(0, widget);
	}
}

/**
 * Private data class of CDockContainerWidget class (pimpl)
 */
struct DockContainerWidgetPrivate
{
	CDockContainerWidget* _this;
	QPointer<CDockManager> DockManager;
	unsigned int zOrderIndex = 0;
	QList<CDockAreaWidget*> DockAreas;
	QGridLayout* Layout = nullptr;
	QSplitter* RootSplitter;
	bool isFloating = false;

	/**
	 * Private data constructor
	 */
	DockContainerWidgetPrivate(CDockContainerWidget* _public);

	/**
	 * Adds dock widget to container and returns the dock area that contains
	 * the inserted dock widget
	 */
	CDockAreaWidget* dockWidgetIntoContainer(DockWidgetArea area, CDockWidget* Dockwidget);

	/**
	 * Adds dock widget to a existing DockWidgetArea
	 */
	CDockAreaWidget* dockWidgetIntoDockArea(DockWidgetArea area, CDockWidget* Dockwidget,
		CDockAreaWidget* TargetDockArea);

	/**
	 * Add dock area to this container
	 */
	void addDockArea(CDockAreaWidget* NewDockWidget, DockWidgetArea area = CenterDockWidgetArea);

	/**
	 * Drop floating widget into container
	 */
	void dropIntoContainer(CFloatingDockContainer* FloatingWidget, DockWidgetArea area);

	/**
	 * Drop floating widget into dock area
	 */
	void dropIntoSection(CFloatingDockContainer* FloatingWidget,
		CDockAreaWidget* TargetArea, DockWidgetArea area);

	/**
	 * Adds new dock areas to the internal dock area list
	 */
	void addDockAreasToList(const QList<CDockAreaWidget*> NewDockAreas);

	/**
	 * Save state of child nodes
	 */
	void saveChildNodesState(QXmlStreamWriter& Stream, QWidget* Widget);

	/**
	 * Restore state of child nodes.
	 * \param[in] Stream The data stream that contains the serialized state
	 * \param[out] CreatedWidget The widget created from parsed data or 0 if
	 * the parsed widget was an empty splitter
	 * \param[in] Testing If Testing is true, only the stream data is
	 * parsed without modifiying anything.
	 */
	bool restoreChildNodes(QXmlStreamReader& Stream, QWidget*& CreatedWidget,
		bool Testing);

	/**
	 * Restores a splitter.
	 * \see restoreChildNodes() for details
	 */
	bool restoreSplitter(QXmlStreamReader& Stream, QWidget*& CreatedWidget,
		bool Testing);

	/**
	 * Restores a dock area.
	 * \see restoreChildNodes() for details
	 */
	bool restoreDockArea(QXmlStreamReader& Stream, QWidget*& CreatedWidget,
		bool Testing);

	/**
	 * Helper function for recursive dumping of layout
	 */
	void dumpRecursive(int level, QWidget* widget);
}; // struct DockContainerWidgetPrivate


//============================================================================
DockContainerWidgetPrivate::DockContainerWidgetPrivate(CDockContainerWidget* _public) :
	_this(_public)
{

}


//============================================================================
void DockContainerWidgetPrivate::dropIntoContainer(CFloatingDockContainer* FloatingWidget,
	DockWidgetArea area)
{
	auto InsertParam = internal::dockAreaInsertParameters(area);
	auto NewDockAreas = FloatingWidget->dockContainer()->findChildren<CDockAreaWidget*>(
		QString(), Qt::FindChildrenRecursively);
	CDockWidget* DockWidget = FloatingWidget->dockContainer()->findChild<CDockWidget*>();
	QSplitter* Splitter = RootSplitter;

	if (DockAreas.count() <= 1)
	{
		Splitter->setOrientation(InsertParam.orientation());
	}
	else if (Splitter->orientation() != InsertParam.orientation())
	{
		QSplitter* NewSplitter = internal::newSplitter(InsertParam.orientation());
		QLayoutItem* li = Layout->replaceWidget(Splitter, NewSplitter);
		NewSplitter->addWidget(Splitter);
		Splitter = NewSplitter;
		delete li;
	}

	// Now we can insert the floating widget content into this container
	auto FloatingSplitter = FloatingWidget->dockContainer()->rootSplitter();
	if (FloatingSplitter->count() == 1)
	{
		insertWidgetIntoSplitter(Splitter, FloatingSplitter->widget(0), InsertParam.append());
	}
	else if (FloatingSplitter->orientation() == InsertParam.orientation())
	{
		while (FloatingSplitter->count())
		{
			insertWidgetIntoSplitter(Splitter, FloatingSplitter->widget(0), InsertParam.append());
		}
	}
	else
	{
		insertWidgetIntoSplitter(Splitter, FloatingSplitter, InsertParam.append());
	}

	RootSplitter = Splitter;
	addDockAreasToList(NewDockAreas);
	FloatingWidget->deleteLater();
	if (DockWidget)
	{
		DockWidget->toggleView(true);
	}
	_this->dumpLayout();
}


//============================================================================
void DockContainerWidgetPrivate::dropIntoSection(CFloatingDockContainer* FloatingWidget,
		CDockAreaWidget* TargetArea, DockWidgetArea area)
{
	CDockContainerWidget* FloatingContainer = FloatingWidget->dockContainer();
	if (area == CenterDockWidgetArea)
	{
		auto NewDockWidgets = FloatingContainer->findChildren<CDockWidget*>(
			QString(), Qt::FindChildrenRecursively);
		for (auto DockWidget : NewDockWidgets)
		{
			TargetArea->insertDockWidget(0, DockWidget, false);
		}
		TargetArea->setCurrentIndex(0); // make the topmost widget active
		FloatingWidget->deleteLater();
		TargetArea->updateDockArea();
		return;
	}

	auto InsertParam = internal::dockAreaInsertParameters(area);
	auto NewDockAreas = FloatingWidget->dockContainer()->findChildren<CDockAreaWidget*>(
		QString(), Qt::FindChildrenRecursively);
	QSplitter* TargetAreaSplitter = internal::findParent<QSplitter*>(TargetArea);

	if (!TargetAreaSplitter)
	{
		QSplitter* Splitter = internal::newSplitter(InsertParam.orientation());
		Layout->replaceWidget(TargetArea, Splitter);
		Splitter->addWidget(TargetArea);
		TargetAreaSplitter = Splitter;
	}

	int AreaIndex = TargetAreaSplitter->indexOf(TargetArea);
	auto Widget = FloatingWidget->dockContainer()->findChild<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
	auto FloatingSplitter = dynamic_cast<QSplitter*>(Widget);

	if (TargetAreaSplitter->orientation() == InsertParam.orientation())
	{
		if ((FloatingSplitter->orientation() != InsertParam.orientation()) && FloatingSplitter->count() > 1)
		{
			TargetAreaSplitter->insertWidget(AreaIndex + InsertParam.insertOffset(), Widget);
		}
		else
		{
			int InsertIndex = AreaIndex + InsertParam.insertOffset();
			while (FloatingSplitter->count())
			{
				TargetAreaSplitter->insertWidget(InsertIndex++, FloatingSplitter->widget(0));
			}
		}
	}
	else
	{
		QSplitter* NewSplitter = internal::newSplitter(InsertParam.orientation());
		if ((FloatingSplitter->orientation() != InsertParam.orientation()) && FloatingSplitter->count() > 1)
		{
			NewSplitter->addWidget(Widget);
		}
		else
		{
			while (FloatingSplitter->count())
			{
				NewSplitter->addWidget(FloatingSplitter->widget(0));
			}
		}

		insertWidgetIntoSplitter(NewSplitter, TargetArea, !InsertParam.append());
		TargetAreaSplitter->insertWidget(AreaIndex, NewSplitter);
	}

	FloatingWidget->deleteLater();
	addDockAreasToList(NewDockAreas);
	_this->dumpLayout();
}


//============================================================================
void DockContainerWidgetPrivate::addDockAreasToList(const QList<CDockAreaWidget*> NewDockAreas)
{
	int CountBefore = DockAreas.count();
	int NewAreaCount = NewDockAreas.count();
	DockAreas.append(NewDockAreas);

	// We need to ensure, that the dock area title bar is visible. The title bar
	// is invisible, if the dock are is a single dock area in a floating widget.
	if (1 == CountBefore)
	{
		DockAreas.at(0)->updateDockArea();
	}

	if (1 == NewAreaCount)
	{
		DockAreas.last()->updateDockArea();
	}

	emit _this->dockAreasAdded();
}


//============================================================================
void DockContainerWidgetPrivate::saveChildNodesState(QXmlStreamWriter& s, QWidget* Widget)
{
	QSplitter* Splitter = dynamic_cast<QSplitter*>(Widget);
	if (Splitter)
	{
		s.writeStartElement("Splitter");
		s.writeAttribute("Orientation", QString::number(Splitter->orientation()));
		s.writeAttribute("Count", QString::number(Splitter->count()));
		qDebug() << "NodeSplitter orient: " << Splitter->orientation()
			<< " WidgetCont: " << Splitter->count();
			for (int i = 0; i < Splitter->count(); ++i)
			{
				saveChildNodesState(s, Splitter->widget(i));
			}

			s.writeStartElement("Sizes");
			for (auto Size : Splitter->sizes())
			{
				s.writeCharacters(QString::number(Size) + " ");
			}
			s.writeEndElement();
		s.writeEndElement();
	}
	else
	{
		CDockAreaWidget* DockArea = dynamic_cast<CDockAreaWidget*>(Widget);
		if (DockArea)
		{
			DockArea->saveState(s);
		}
	}
}


//============================================================================
bool DockContainerWidgetPrivate::restoreSplitter(QXmlStreamReader& s,
	QWidget*& CreatedWidget, bool Testing)
{
	bool Ok;
	int Orientation  = s.attributes().value("Orientation").toInt(&Ok);
	if (!Ok)
	{
		return false;
	}

	int WidgetCount = s.attributes().value("Count").toInt(&Ok);
	if (!Ok)
	{
		return false;
	}
	qDebug() << "Restore NodeSplitter Orientation: " <<  Orientation <<
			" WidgetCount: " << WidgetCount;
	QSplitter* Splitter = nullptr;
	if (!Testing)
	{
		Splitter = internal::newSplitter((Qt::Orientation)Orientation);
	}
	bool Visible = false;
	QList<int> Sizes;
	while (s.readNextStartElement())
	{
		QWidget* ChildNode = nullptr;
		bool Result = true;
		if (s.name() == "Splitter")
		{
			Result = restoreSplitter(s, ChildNode, Testing);
		}
		else if (s.name() == "DockAreaWidget")
		{
			Result = restoreDockArea(s, ChildNode, Testing);
		}
		else if (s.name() == "Sizes")
		{
			QString sSizes = s.readElementText().trimmed();
			qDebug() << "Sizes: " << sSizes;
			QTextStream TextStream(&sSizes);
			while (!TextStream.atEnd())
			{
				int value;
				TextStream >> value;
				Sizes.append(value);
			}
		}
		else
		{
			s.skipCurrentElement();
		}

		if (!Result)
		{
			return false;
		}

		if (Testing || !ChildNode)
		{
			continue;
		}

		qDebug() << "ChildNode isVisible " << ChildNode->isVisible()
			<< " isVisibleTo " << ChildNode->isVisibleTo(Splitter);
		Splitter->addWidget(ChildNode);
		Visible |= ChildNode->isVisibleTo(Splitter);
	}

	if (Sizes.count() != WidgetCount)
	{
		return false;
	}

	if (!Testing)
	{
		if (!Splitter->count())
		{
			delete Splitter;
			Splitter = nullptr;
		}
		else
		{
			Splitter->setSizes(Sizes);
			Splitter->setVisible(Visible);
		}
		CreatedWidget = Splitter;
	}
	else
	{
		CreatedWidget = nullptr;
	}

	return true;
}


//============================================================================
bool DockContainerWidgetPrivate::restoreDockArea(QXmlStreamReader& s,
	QWidget*& CreatedWidget, bool Testing)
{
	bool Ok;
	int Tabs = s.attributes().value("Tabs").toInt(&Ok);
	if (!Ok)
	{
		return false;
	}

	int CurrentIndex = s.attributes().value("CurrentIndex").toInt(&Ok);
	if (!Ok)
	{
		return false;
	}
	qDebug() << "Restore NodeDockArea Tabs: " << Tabs << " CurrentIndex: "
			<< CurrentIndex;

	CDockAreaWidget* DockArea = nullptr;
	if (!Testing)
	{
		DockArea = new CDockAreaWidget(DockManager, _this);
	}

	while (s.readNextStartElement())
	{
		if (s.name() != "DockWidget")
		{
			continue;
		}

		auto ObjectName = s.attributes().value("ObjectName");
		if (ObjectName.isEmpty())
		{
			return false;
		}

		bool Closed = s.attributes().value("Closed").toInt(&Ok);
		if (!Ok)
		{
			return false;
		}

		s.skipCurrentElement();
		CDockWidget* DockWidget = DockManager->findDockWidget(ObjectName.toString());
		if (!DockWidget || Testing)
		{
			continue;
		}

		qDebug() << "Dock Widget found - parent " << DockWidget->parent();
		DockArea->addDockWidget(DockWidget);

		DockArea->hide();
		DockWidget->setToggleViewActionChecked(!Closed);
		DockWidget->setProperty("closed", Closed);
		DockWidget->setProperty("dirty", false);
	}

	if (Testing)
	{
		return true;
	}

	if (!DockArea->count())
	{
		delete DockArea;
		DockArea = nullptr;
	}
	else
	{
		DockArea->setProperty("currentIndex", CurrentIndex);
		DockAreas.append(DockArea);
	}

	CreatedWidget = DockArea;
	return true;
}


//============================================================================
bool DockContainerWidgetPrivate::restoreChildNodes(QXmlStreamReader& s,
	QWidget*& CreatedWidget, bool Testing)
{
	bool Result = true;
	while (s.readNextStartElement())
	{
		if (s.name() == "Splitter")
		{
			Result = restoreSplitter(s, CreatedWidget, Testing);
			qDebug() << "Splitter";
		}
		else if (s.name() == "DockAreaWidget")
		{
			Result = restoreDockArea(s, CreatedWidget, Testing);
			qDebug() << "DockAreaWidget";
		}
		else
		{
			s.skipCurrentElement();
			qDebug() << "Unknown element";
		}
	}

	return Result;
}


//============================================================================
CDockAreaWidget* DockContainerWidgetPrivate::dockWidgetIntoContainer(DockWidgetArea area,
	CDockWidget* Dockwidget)
{
	CDockAreaWidget* NewDockArea = new CDockAreaWidget(DockManager, _this);
	NewDockArea->addDockWidget(Dockwidget);
	addDockArea(NewDockArea, area);
	return NewDockArea;
}


//============================================================================
void DockContainerWidgetPrivate::addDockArea(CDockAreaWidget* NewDockArea, DockWidgetArea area)
{
	auto InsertParam = internal::dockAreaInsertParameters(area);
	// As long as we have only one dock area in the splitter we can adjust
	// its orientation
	if (DockAreas.count() <= 1)
	{
		RootSplitter->setOrientation(InsertParam.orientation());
	}

	QSplitter* Splitter = RootSplitter;
	if (Splitter->orientation() == InsertParam.orientation())
	{
		insertWidgetIntoSplitter(Splitter, NewDockArea, InsertParam.append());
	}
	else
	{
		QSplitter* NewSplitter = internal::newSplitter(InsertParam.orientation());
		if (InsertParam.append())
		{
			QLayoutItem* li = Layout->replaceWidget(Splitter, NewSplitter);
			NewSplitter->addWidget(Splitter);
			NewSplitter->addWidget(NewDockArea);
			delete li;
		}
		else
		{
			NewSplitter->addWidget(NewDockArea);
			QLayoutItem* li = Layout->replaceWidget(Splitter, NewSplitter);
			NewSplitter->addWidget(Splitter);
			delete li;
		}
		RootSplitter = NewSplitter;
	}

	DockAreas.append(NewDockArea);
	NewDockArea->updateDockArea();
	emit _this->dockAreasAdded();
}


//============================================================================
void DockContainerWidgetPrivate::dumpRecursive(int level, QWidget* widget)
{
#if defined(QT_DEBUG)
	QSplitter* Splitter = dynamic_cast<QSplitter*>(widget);
	QByteArray buf;
    buf.fill(' ', level * 4);
	if (Splitter)
	{
		qDebug("%sSplitter %s v: %s c: %s",
			(const char*)buf,
			(Splitter->orientation() == Qt::Vertical) ? "-" : "|",
			 Splitter->isVisibleTo(Splitter->parentWidget()) ? "1" : "0",
			 QString::number(Splitter->count()).toStdString().c_str());
		for (int i = 0; i < Splitter->count(); ++i)
		{
			dumpRecursive(level + 1, Splitter->widget(i));
		}
	}
	else
	{
		CDockAreaWidget* DockArea = dynamic_cast<CDockAreaWidget*>(widget);
		if (!DockArea)
		{
			return;
		}
		qDebug("%sDockArea", (const char*)buf);
	}
#else
	Q_UNUSED(level);
	Q_UNUSED(widget);
#endif
}


//============================================================================
CDockAreaWidget* DockContainerWidgetPrivate::dockWidgetIntoDockArea(DockWidgetArea area,
	CDockWidget* Dockwidget, CDockAreaWidget* TargetDockArea)
{
	if (CenterDockWidgetArea == area)
	{
		TargetDockArea->addDockWidget(Dockwidget);
		return TargetDockArea;
	}

	CDockAreaWidget* NewDockArea = new CDockAreaWidget(DockManager, _this);
	NewDockArea->addDockWidget(Dockwidget);
	auto InsertParam = internal::dockAreaInsertParameters(area);

	QSplitter* TargetAreaSplitter = internal::findParent<QSplitter*>(TargetDockArea);
	int index = TargetAreaSplitter ->indexOf(TargetDockArea);
	if (TargetAreaSplitter->orientation() == InsertParam.orientation())
	{
		qDebug() << "TargetAreaSplitter->orientation() == InsertParam.orientation()";
		TargetAreaSplitter->insertWidget(index + InsertParam.insertOffset(), NewDockArea);
	}
	else
	{
		qDebug() << "TargetAreaSplitter->orientation() != InsertParam.orientation()";
		QSplitter* NewSplitter = internal::newSplitter(InsertParam.orientation());
		NewSplitter->addWidget(TargetDockArea);
		insertWidgetIntoSplitter(NewSplitter, NewDockArea, InsertParam.append());
		TargetAreaSplitter->insertWidget(index, NewSplitter);
	}

	DockAreas.append(NewDockArea);
	emit _this->dockAreasAdded();
	return NewDockArea;
}


//============================================================================
CDockContainerWidget::CDockContainerWidget(CDockManager* DockManager, QWidget *parent) :
	QFrame(parent),
	d(new DockContainerWidgetPrivate(this))
{
	d->isFloating = dynamic_cast<CFloatingDockContainer*>(parent) != 0;

	//setStyleSheet("background: green;");
	d->DockManager = DockManager;
	if (DockManager != this)
	{
		d->DockManager->registerDockContainer(this);
	}

	d->Layout = new QGridLayout();
	d->Layout->setContentsMargins(0, 1, 0, 1);
	d->Layout->setSpacing(0);
	setLayout(d->Layout);

	d->RootSplitter = internal::newSplitter(Qt::Horizontal);
	d->Layout->addWidget(d->RootSplitter);
}

//============================================================================
CDockContainerWidget::~CDockContainerWidget()
{
	if (d->DockManager)
	{
		d->DockManager->removeDockContainer(this);
	}
	delete d;
}


//============================================================================
CDockAreaWidget* CDockContainerWidget::addDockWidget(DockWidgetArea area, CDockWidget* Dockwidget,
	CDockAreaWidget* DockAreaWidget)
{
	CDockAreaWidget* OldDockArea = Dockwidget->dockAreaWidget();
	if (OldDockArea)
	{
		OldDockArea->removeDockWidget(Dockwidget);
	}

	Dockwidget->setDockManager(d->DockManager);
	if (DockAreaWidget)
	{
		return d->dockWidgetIntoDockArea(area, Dockwidget, DockAreaWidget);
	}
	else
	{
		return d->dockWidgetIntoContainer(area, Dockwidget);
	}
}


//============================================================================
unsigned int CDockContainerWidget::zOrderIndex() const
{
	return d->zOrderIndex;
}


//============================================================================
bool CDockContainerWidget::isInFrontOf(CDockContainerWidget* Other) const
{
	return this->zOrderIndex() > Other->zOrderIndex();
}


//============================================================================
bool CDockContainerWidget::event(QEvent *e)
{
	bool Result = QWidget::event(e);
	if (e->type() == QEvent::WindowActivate)
    {
        d->zOrderIndex = ++zOrderCounter;
    }
	else if (e->type() == QEvent::Show && !d->zOrderIndex)
	{
		d->zOrderIndex = ++zOrderCounter;
	}

	return Result;
}


//============================================================================
void CDockContainerWidget::addDockArea(CDockAreaWidget* DockAreaWidget,
	DockWidgetArea area)
{
	CDockContainerWidget* Container = DockAreaWidget->dockContainer();
	if (Container && Container != this)
	{
		Container->removeDockArea(DockAreaWidget);
	}

	d->addDockArea(DockAreaWidget, area);
}


//============================================================================
void CDockContainerWidget::removeDockArea(CDockAreaWidget* area)
{
	qDebug() << "CDockContainerWidget::removeDockArea";
	d->DockAreas.removeAll(area);
	CDockSplitter* Splitter = internal::findParent<CDockSplitter*>(area);

	// Remove are from parent splitter and hide splitter if it has no visible
	// content
	area->setParent(0);
	Splitter->setVisible(Splitter->hasVisibleContent());

	// If splitter has more than 1 widgets, we are finished and can leave
	if (Splitter->count() >  1)
	{
		goto emitAndExit;
	}

	// If this is the RootSplitter we need to remove empty splitters to
	// avoid too many empty splitters
	if (Splitter == d->RootSplitter)
	{
		qDebug() << "Removed from RootSplitter";
		// If splitter is empty, we are finished
		if (!Splitter->count())
		{
			Splitter->hide();
			goto emitAndExit;
		}

		QWidget* widget = Splitter->widget(0);
		QSplitter* ChildSplitter = dynamic_cast<QSplitter*>(widget);
		// If the one and only content widget of the splitter is not a splitter
		// then we are finished
		if (!ChildSplitter)
		{
			goto emitAndExit;
		}

		// We replace the superfluous RootSplitter with the ChildSplitter
		ChildSplitter->setParent(0);
		QLayoutItem* li = d->Layout->replaceWidget(Splitter, ChildSplitter);
		d->RootSplitter = ChildSplitter;
		delete li;
		qDebug() << "RootSplitter replaced by child splitter";
	}
	else if (Splitter->count() == 1)
	{
		qDebug() << "Replacing splitter with content";
		QWidget* widget = Splitter->widget(0);
		widget->setParent(this);
		QSplitter* ParentSplitter = internal::findParent<QSplitter*>(Splitter);
		internal::replaceSplitterWidget(ParentSplitter, Splitter, widget);
	}

	delete Splitter;

emitAndExit:
	dumpLayout();
	emit dockAreasRemoved();
}


//============================================================================
CDockAreaWidget* CDockContainerWidget::dockAreaAt(const QPoint& GlobalPos) const
{
	for (const auto& DockArea : d->DockAreas)
	{
		if (DockArea->isVisible() && DockArea->rect().contains(DockArea->mapFromGlobal(GlobalPos)))
		{
			return DockArea;
		}
	}

	return 0;
}


//============================================================================
CDockAreaWidget* CDockContainerWidget::dockArea(int Index) const
{
	return (Index < dockAreaCount()) ? d->DockAreas[Index] : 0;
}


//============================================================================
bool CDockContainerWidget::isFloating() const
{
	return d->isFloating;
}


//============================================================================
int CDockContainerWidget::dockAreaCount() const
{
	return d->DockAreas.count();
}


//============================================================================
int CDockContainerWidget::visibleDockAreaCount() const
{
	// TODO Cache or precalculate this to speed it up because it is used during
	// movement of floating widget
	int Result = 0;
	for (auto DockArea : d->DockAreas)
	{
		Result += DockArea->isVisible() ? 1 : 0;
	}

	return Result;
}


//============================================================================
void CDockContainerWidget::dropFloatingWidget(CFloatingDockContainer* FloatingWidget,
	const QPoint& TargetPos)
{
	qDebug() << "CDockContainerWidget::dropFloatingWidget";
	CDockAreaWidget* DockArea = dockAreaAt(TargetPos);
	auto dropArea = InvalidDockWidgetArea;
	auto ContainerDropArea = d->DockManager->containerOverlay()->dropAreaUnderCursor();
	if (DockArea)
	{
		auto dropOverlay = d->DockManager->dockAreaOverlay();
		dropOverlay->setAllowedAreas(AllDockAreas);
		dropArea = dropOverlay->showOverlay(DockArea);
		if (ContainerDropArea != InvalidDockWidgetArea &&
			ContainerDropArea != dropArea)
		{
			dropArea = InvalidDockWidgetArea;
		}

		if (dropArea != InvalidDockWidgetArea)
		{
			qDebug() << "Dock Area Drop Content: " << dropArea;
			d->dropIntoSection(FloatingWidget, DockArea, dropArea);
		}
	}

	// mouse is over container
	if (InvalidDockWidgetArea == dropArea)
	{
		dropArea = ContainerDropArea;
		qDebug()  << "Container Drop Content: " << dropArea;
		if (dropArea != InvalidDockWidgetArea)
		{
			d->dropIntoContainer(FloatingWidget, dropArea);
		}
	}
}


//============================================================================
QList<CDockAreaWidget*> CDockContainerWidget::openedDockAreas() const
{
	QList<CDockAreaWidget*> Result;
	for (auto DockArea : d->DockAreas)
	{
		if (DockArea->isVisible())
		{
			Result.append(DockArea);
		}
	}

	return Result;
}


//============================================================================
void CDockContainerWidget::saveState(QXmlStreamWriter& s) const
{
	qDebug() << "CDockContainerWidget::saveState isFloating "
		<< isFloating();

	s.writeStartElement("DockContainerWidget");
	s.writeAttribute("Floating", QString::number(isFloating() ? 1 : 0));
	if (isFloating())
	{
		CFloatingDockContainer* FloatingWidget = internal::findParent<CFloatingDockContainer*>(this);
		QByteArray Geometry = FloatingWidget->saveGeometry();
		s.writeTextElement("Geometry", Geometry.toHex(' '));
	}
	d->saveChildNodesState(s, d->RootSplitter);
	s.writeEndElement();
}


//============================================================================
bool CDockContainerWidget::restoreState(QXmlStreamReader& s, bool Testing)
{
	bool IsFloating = s.attributes().value("Floating").toInt();
	qDebug() << "Restore CDockContainerWidget Floating" << IsFloating;

	QWidget*NewRootSplitter {};
	if (!Testing)
	{
		d->DockAreas.clear();
	}

	if (IsFloating)
	{
		qDebug() << "Restore floating widget";
		if (!s.readNextStartElement() || s.name() != "Geometry")
		{
			return false;
		}

		QByteArray GeometryString = s.readElementText(QXmlStreamReader::ErrorOnUnexpectedElement).toLocal8Bit();
		QByteArray Geometry = QByteArray::fromHex(GeometryString);
		std::cout << "Geometry: " << Geometry.toHex(' ').toStdString() << std::endl;
		if (Geometry.isEmpty())
		{
			return false;
		}

		if (!Testing)
		{
			CFloatingDockContainer* FloatingWidget = internal::findParent<CFloatingDockContainer*>(this);
			FloatingWidget->restoreGeometry(Geometry);
		}
	}

	if (!d->restoreChildNodes(s, NewRootSplitter, Testing))
	{
		return false;
	}

	if (Testing)
	{
		return true;
	}

	// If the root splitter is empty, rostoreChildNodes returns a 0 pointer
	// and we need to create a new empty root splitter
	if (!NewRootSplitter)
	{
		NewRootSplitter = internal::newSplitter(Qt::Horizontal);
	}

	d->Layout->replaceWidget(d->RootSplitter, NewRootSplitter);
	QSplitter* OldRoot = d->RootSplitter;
	d->RootSplitter = dynamic_cast<QSplitter*>(NewRootSplitter);
	OldRoot->deleteLater();

	return true;
}


//============================================================================
QSplitter* CDockContainerWidget::rootSplitter() const
{
	return d->RootSplitter;
}


//============================================================================
void CDockContainerWidget::dumpLayout()
{
	qDebug("\n\nDumping layout --------------------------");
	d->dumpRecursive(0, d->RootSplitter);
	qDebug("--------------------------\n\n");
}


} // namespace ads

//---------------------------------------------------------------------------
// EOF DockContainerWidget.cpp
