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

#include <iostream>

#include "FloatingDockContainer.h"
#include "DockOverlay.h"

namespace ads
{

// sentinel values used to validate state data
enum VersionMarkers
{
	VersionMarker = 0xff
};

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

	/**
	 * Private data constructor
	 */
	DockManagerPrivate(CDockManager* _public);

	/**
	 * Restores a non existing container from stream
	 */
	bool restoreContainer(QDataStream& Stream);
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
    stream << VersionMarker;
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

    if (stream.status() != QDataStream::Ok || marker != VersionMarker || v != version)
    {
        return false;
    }

    int ContainerCount;
    stream >> ContainerCount;
    std::cout << "ContainerCount " << ContainerCount << std::endl;
    for (int i = 0; i < ContainerCount; ++i)
    {
    	if (i >= d->Containers.count())
    	{
    		CFloatingDockContainer* FloatingWidget = new CFloatingDockContainer(this);
    	}

    	std::cout << "d->Containers[i]->restoreState " << i << std::endl;
    	d->Containers[i]->restoreState(stream);
    }

    return true;
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockManager.cpp
