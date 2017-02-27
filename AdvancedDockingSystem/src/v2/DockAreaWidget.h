#ifndef DockAreaWidgetH
#define DockAreaWidgetH
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
/// \file   DockAreaWidget.h
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Declaration of CDockAreaWidget class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>

namespace ads
{
struct DockAreaWidgetPrivate;
class CDockManager;
class CDockContainerWidget;
class CDockWidget;

/**
 * DockAreaWidget manages multiple instances of DckWidgets.
 * It displays a title tab, which is clickable and will switch to
 * the contents associated to the title when clicked.
 */
class CDockAreaWidget : public QFrame
{
	Q_OBJECT
private:
	DockAreaWidgetPrivate* d; ///< private data (pimpl)
	friend class DockAreaWidgetPrivate;

private slots:
	void onDockWidgetTitleClicked();

public:
	/**
	 * Default Constructor
	 */
	CDockAreaWidget(CDockManager* DockManager, CDockContainerWidget* parent);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockAreaWidget();

	/**
	 * Returns the dock container widget this dock area widget belongs to or 0
	 * if there is no
	 */
	CDockContainerWidget* dockContainerWidget() const;

	/**
	 * Add a new dock widget to dock area.
	 * All dockwidgets in the dock area tabified in a stacked layout with tabs
	 */
	void addDockWidget(CDockWidget* DockWidget);

public slots:
	/**
	 * This sets the index position of the current tab page.
	 */
	void setCurrentIndex(int index);
}; // class DockAreaWidget
}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // DockAreaWidgetH
