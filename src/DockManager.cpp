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
/// \file   DockManager.cpp
/// \author Uwe Kindler
/// \date   26.02.2017
/// \brief  Implementation of CDockManager class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockManager.h"

#include <QMainWindow>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QDebug>
#include <QFile>
#include <QApplication>
#include <QAction>

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
	bool restoreState(const QByteArray &state, int version);

	/**
	 * Restores the container with the given index
	 */
	bool restoreContainer(int Index, QDataStream& stream, bool Testing);

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
bool DockManagerPrivate::checkFormat(const QByteArray &state, int version)
{
    if (state.isEmpty())
    {
        return false;
    }
    QByteArray sd = state;
    QDataStream stream(&sd, QIODevice::ReadOnly);

    int marker;
	int v;
    stream >> marker;
    stream >> v;

    if (stream.status() != QDataStream::Ok || marker != internal::VersionMarker || v != version)
    {
        return false;
    }

    int Result = true;
    int ContainerCount;
    stream >> ContainerCount;
    int i;
    for (i = 0; i < ContainerCount; ++i)
    {
    	if (!Containers[0]->restoreState(stream, internal::RestoreTesting))
    	{
    		Result = false;
    		break;
    	}
    }

    return Result;
}


//============================================================================
bool DockManagerPrivate::restoreContainer(int Index, QDataStream& stream, bool Testing)
{
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
bool DockManagerPrivate::restoreState(const QByteArray &state,  int version)
{
    if (state.isEmpty())
    {
        return false;
    }
    QByteArray sd = state;
    QDataStream stream(&sd, QIODevice::ReadOnly);

    int marker;
	int v;
    stream >> marker;
    stream >> v;

    if (stream.status() != QDataStream::Ok || marker != internal::VersionMarker || v != version)
    {
        return false;
    }

    int Result = true;
    int ContainerCount;
    stream >> ContainerCount;
    qDebug() << "ContainerCount " << ContainerCount;
    int i;
    for (i = 0; i < ContainerCount; ++i)
    {
    	Result = restoreContainer(i, stream, internal::Restore);
    	if (!Result)
    	{
    		break;
    	}
    }

    // Delete remaining empty floating widgets
    int FloatingWidgetIndex = i - 1;
    int DeleteCount = FloatingWidgets.count() - FloatingWidgetIndex;
    for (int i = 0; i < DeleteCount; ++i)
    {
    	FloatingWidgets[FloatingWidgetIndex + i]->deleteLater();
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
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << internal::VersionMarker;
    stream << version;

    stream << d->Containers.count();
    for (auto Container : d->Containers)
	{
		Container->saveState(stream);
	}
    return data;
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
    // The do not belong to any dock container, until the user toggles the
    // toggle view action the next time
    for (auto DockWidget : d->DockWidgetsMap)
    {
    	if (DockWidget->property("dirty").toBool())
    	{
    		DockWidget->flagAsUnassigned();
    	}
    	else if (!DockWidget->property("closed").toBool())
    	{
    		DockWidget->toggleView(true);
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
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockManager.cpp
