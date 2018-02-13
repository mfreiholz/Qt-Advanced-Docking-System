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
/// \file   DockManager.cpp
/// \author Uwe Kindler
/// \date   26.02.2017
/// \brief  Implementation of CDockManager class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockManager.h"

#include <iostream>

#include <QMainWindow>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QDebug>
#include <QFile>
#include <QApplication>
#include <QAction>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QSettings>

#include "FloatingDockContainer.h"
#include "DockOverlay.h"
#include "DockWidget.h"
#include "ads_globals.h"
#include "DockStateSerialization.h"
#include "DockWidgetTitleBar.h"
#include "DockAreaWidget.h"

namespace ads
{
/**
 * Private data class of CDockManager class (pimpl)
 */
struct DockManagerPrivate
{
	CDockManager* _this;
	QList<CFloatingDockContainer*> FloatingWidgets;
	QList<CDockContainerWidget*> Containers;
	CDockOverlay* ContainerOverlay;
	CDockOverlay* DockAreaOverlay;
	QMap<QString, CDockWidget*> DockWidgetsMap;
	QMap<QString, QByteArray> Perspectives;

	/**
	 * Private data constructor
	 */
	DockManagerPrivate(CDockManager* _public);

	/**
	 * Checks if the given data stream is a valid docking system state
	 * file.
	 */
	bool checkFormat(const QByteArray &state, int version);

	/**
	 * Restores the state
	 */
	bool restoreState(const QByteArray &state, int version, bool Testing = internal::Restore);

	/**
	 * Restores the container with the given index
	 */
	bool restoreContainer(int Index, QXmlStreamReader& stream, bool Testing);

	/**
	 * Loads the stylesheet
	 */
	void loadStylesheet();
};
// struct DockManagerPrivate

//============================================================================
DockManagerPrivate::DockManagerPrivate(CDockManager* _public) :
	_this(_public)
{

}


//============================================================================
void DockManagerPrivate::loadStylesheet()
{
	QString Result;
	QFile StyleSheetFile(":ads/stylesheets/default.css");
	StyleSheetFile.open(QIODevice::ReadOnly);
	QTextStream StyleSheetStream(&StyleSheetFile);
	Result = StyleSheetStream.readAll();
	StyleSheetFile.close();
	_this->setStyleSheet(Result);
}


//============================================================================
bool DockManagerPrivate::restoreContainer(int Index, QXmlStreamReader& stream, bool Testing)
{
	if (Testing)
	{
		Index = 0;
	}

	if (Index >= Containers.count())
	{
		CFloatingDockContainer* FloatingWidget = new CFloatingDockContainer(_this);
		return FloatingWidget->restoreState(stream, Testing);
	}
	else
	{
		qDebug() << "d->Containers[i]->restoreState ";
		return Containers[Index]->restoreState(stream, Testing);
	}
}


//============================================================================
bool DockManagerPrivate::checkFormat(const QByteArray &state, int version)
{
    return restoreState(state, version, internal::RestoreTesting);
}


//============================================================================
bool DockManagerPrivate::restoreState(const QByteArray &state,  int version,
	bool Testing)
{
    if (state.isEmpty())
    {
        return false;
    }
    QXmlStreamReader s(state);
    s.readNextStartElement();
    if (s.name() != "QtAdvancedDockingSystem")
    {
    	return false;
    }
    qDebug() << s.attributes().value("Version");
    bool ok;
    int v = s.attributes().value("Version").toInt(&ok);
    if (!ok || v != version)
    {
    	return false;
    }

    bool Result = true;
    int  DockContainers = s.attributes().value("DockContainers").toInt();
    qDebug() << DockContainers;
    int DockContainerCount = 0;
    while (s.readNextStartElement())
    {
    	if (s.name() == "DockContainerWidget")
    	{
    		Result = restoreContainer(DockContainerCount, s, Testing);
			if (!Result)
			{
				break;
			}
			DockContainerCount++;
    	}
    }

    if (!Testing)
    {
		// Delete remaining empty floating widgets
		int FloatingWidgetIndex = DockContainerCount - 1;
		int DeleteCount = FloatingWidgets.count() - FloatingWidgetIndex;
		for (int i = 0; i < DeleteCount; ++i)
		{
			FloatingWidgets[FloatingWidgetIndex + i]->deleteLater();
		}
    }

    return Result;
}


//============================================================================
CDockManager::CDockManager(QWidget *parent) :
	CDockContainerWidget(this, parent),
	d(new DockManagerPrivate(this))
{
	QMainWindow* MainWindow = dynamic_cast<QMainWindow*>(parent);
	if (MainWindow)
	{
		MainWindow->setCentralWidget(this);
	}

	d->DockAreaOverlay = new CDockOverlay(this, CDockOverlay::ModeDockAreaOverlay);
	d->ContainerOverlay = new CDockOverlay(this, CDockOverlay::ModeContainerOverlay);
	d->Containers.append(this);
	d->loadStylesheet();
}

//============================================================================
CDockManager::~CDockManager()
{
	auto FloatingWidgets = d->FloatingWidgets;
	for (auto FloatingWidget : FloatingWidgets)
	{
		delete FloatingWidget;
	}
	delete d;
}


//============================================================================
void CDockManager::registerFloatingWidget(CFloatingDockContainer* FloatingWidget)
{
	d->FloatingWidgets.append(FloatingWidget);
	qDebug() << "d->FloatingWidgets.count() " << d->FloatingWidgets.count();
}


//============================================================================
void CDockManager::removeFloatingWidget(CFloatingDockContainer* FloatingWidget)
{
	d->FloatingWidgets.removeAll(FloatingWidget);
}


//============================================================================
void CDockManager::registerDockContainer(CDockContainerWidget* DockContainer)
{
	d->Containers.append(DockContainer);
}


//============================================================================
void CDockManager::removeDockContainer(CDockContainerWidget* DockContainer)
{
	if (this != DockContainer)
	{
		d->Containers.removeAll(DockContainer);
	}
}


//============================================================================
CDockOverlay* CDockManager::containerOverlay() const
{
	return d->ContainerOverlay;
}


//============================================================================
CDockOverlay* CDockManager::dockAreaOverlay() const
{
	return d->DockAreaOverlay;
}


//============================================================================
const QList<CDockContainerWidget*> CDockManager::dockContainers() const
{
	return d->Containers;
}


//============================================================================
const QList<CFloatingDockContainer*> CDockManager::floatingWidgets() const
{
	return d->FloatingWidgets;
}


//============================================================================
unsigned int CDockManager::zOrderIndex() const
{
	return 0;
}


//============================================================================
QByteArray CDockManager::saveState(int version) const
{
    QByteArray xmldata;
    QXmlStreamWriter s(&xmldata);
	s.setAutoFormatting(true);
    s.writeStartDocument();
		s.writeStartElement("QtAdvancedDockingSystem");
		s.writeAttribute("Version", QString::number(version));
		s.writeAttribute("DockContainers", QString::number(d->Containers.count()));
		for (auto Container : d->Containers)
		{
			Container->saveState(s);
		}

		s.writeEndElement();
    s.writeEndDocument();

    std::cout << xmldata.toStdString() << std::endl;
    return xmldata;
}


//============================================================================
bool CDockManager::restoreState(const QByteArray &state, int version)
{
    if (!d->checkFormat(state, version))
    {
    	qDebug() << "checkFormat: Error checking format!!!!!!!";
    	return false;
    }

    for (auto DockWidget : d->DockWidgetsMap)
    {
    	DockWidget->setProperty("dirty", true);
    }

    if (!d->restoreState(state, version))
    {
    	qDebug() << "restoreState: Error restoring state!!!!!!!";
    	return false;
    }

    // All dock widgets, that have not been processed in the restore state
    // function are invisible to the user now and have no assigned dock area
    // They do not belong to any dock container, until the user toggles the
    // toggle view action the next time
    for (auto DockWidget : d->DockWidgetsMap)
    {
    	if (DockWidget->property("dirty").toBool())
    	{
    		DockWidget->flagAsUnassigned();
    	}
    	else
    	{
    		DockWidget->toggleView(!DockWidget->property("closed").toBool());
    	}
    }

    // Now all dock areas are properly restored and we setup the index of
    // The dock areas because the previous toggleView() action has changed
    // the dock area index
    for (auto DockContainer : d->Containers)
    {
    	for (int i = 0; i < DockContainer->dockAreaCount(); ++i)
    	{
    		CDockAreaWidget* DockArea = DockContainer->dockArea(i);
    		int CurrentIndex = DockArea->property("currentIndex").toInt();
    		if (CurrentIndex < DockArea->count() && DockArea->count() > 1 && CurrentIndex > -1)
    		{
    			DockArea->setCurrentIndex(CurrentIndex);
    		}
    	}
    }

    return true;
}


//============================================================================
CDockAreaWidget* CDockManager::addDockWidget(DockWidgetArea area,
	CDockWidget* Dockwidget, CDockAreaWidget* DockAreaWidget)
{
	d->DockWidgetsMap.insert(Dockwidget->objectName(), Dockwidget);
	return CDockContainerWidget::addDockWidget(area, Dockwidget, DockAreaWidget);
}


//============================================================================
CDockWidget* CDockManager::findDockWidget(const QString& ObjectName)
{
	return d->DockWidgetsMap.value(ObjectName, nullptr);
}


//============================================================================
void CDockManager::addPerspective(const QString& UniquePrespectiveName)
{
	d->Perspectives.insert(UniquePrespectiveName, saveState());
	emit perspectiveListChanged();
}


//============================================================================
QStringList CDockManager::perspectiveNames() const
{
	return d->Perspectives.keys();
}


//============================================================================
void CDockManager::openPerspective(const QString& PerspectiveName)
{
	std::cout << "CDockManager::openPerspective " << PerspectiveName.toStdString() << std::endl;
	const auto Iterator = d->Perspectives.find(PerspectiveName);
	if (d->Perspectives.end() == Iterator)
	{
		return;
	}

	std::cout << "CDockManager::openPerspective - restoring state" << std::endl;
	restoreState(Iterator.value());
}


//============================================================================
void CDockManager::savePerspectives(QSettings& Settings) const
{
	Settings.beginWriteArray("Perspectives", d->Perspectives.size());
	int i = 0;
	for (auto it = d->Perspectives.constBegin(); it != d->Perspectives.constEnd(); ++it)
	{
		Settings.setArrayIndex(i);
		Settings.setValue("Name", it.key());
		Settings.setValue("State", it.value());
		++i;
	}
	Settings.endArray();
}


//============================================================================
void CDockManager::loadPerspectives(QSettings& Settings)
{
	d->Perspectives.clear();
	int Size = Settings.beginReadArray("Perspectives");
	if (!Size)
	{
		Settings.endArray();
		return;
	}

	for (int i = 0; i < Size; ++i)
	{
		Settings.setArrayIndex(i);
		QString Name = Settings.value("Name").toString();
		QByteArray Data = Settings.value("State").toByteArray();
		if (Name.isEmpty() || Data.isEmpty())
		{
			continue;
		}

		d->Perspectives.insert(Name, Data);
	}

	Settings.endArray();
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockManager.cpp
