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

#include <iostream>

#include "FloatingDockContainer.h"
#include "DockOverlay.h"
#include "DockWidget.h"
#include "ads_globals.h"
#include "DockStateSerialization.h"

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
	 * Restores a non existing container from stream
	 */
	bool restoreContainer(QDataStream& Stream);

	/**
	 * Checks if the given data stream is a valid docking system state
	 * file.
	 */
	bool checkFormat(const QByteArray &state, int version);

	/**
	 * Restores the state
	 */
	bool restoreState(const QByteArray &state, int version);
};
// struct DockManagerPrivate

//============================================================================
DockManagerPrivate::DockManagerPrivate(CDockManager* _public) :
	_this(_public)
{

}


//============================================================================
bool DockManagerPrivate::restoreContainer(QDataStream& Stream)
{
	std::cout << "restoreContainer" << std::endl;
	CFloatingDockContainer* FloatingWidget = new CFloatingDockContainer(_this);
	return true;
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
    std::cout << "ContainerCount " << ContainerCount << std::endl;
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
    std::cout << "ContainerCount " << ContainerCount << std::endl;
    int i;
    for (i = 0; i < ContainerCount; ++i)
    {
    	if (i >= Containers.count())
    	{
    		CFloatingDockContainer* FloatingWidget = new CFloatingDockContainer(_this);
    	}

    	std::cout << "d->Containers[i]->restoreState " << i << std::endl;
    	if (!Containers[i]->restoreState(stream, internal::Restore))
    	{
    		Result = false;
    		break;
    	}
    }

    // Delete remaining empty floating widgets
    int FloatingWidgetIndex = i - 1;
    int DeleteCount = FloatingWidgets.count() - FloatingWidgetIndex;
    for (int i = 0; i < DeleteCount; ++i)
    {
    	delete FloatingWidgets[FloatingWidgetIndex];
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
	std::cout << "d->FloatingWidgets.count() " << d->FloatingWidgets.count()
		<< std::endl;
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
    	std::cout << "checkFormat: Error checking format!!!!!!!" << std::endl;
    	return false;
    }

    if (!d->restoreState(state, version))
    {
    	std::cout << "restoreState: Error restoring state!!!!!!!" << std::endl;
    	return false;
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
