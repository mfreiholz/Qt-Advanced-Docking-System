#ifndef FloatingDockContainerH
#define FloatingDockContainerH
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
/// \file   FloatingDockContainer.h
/// \author Uwe Kindler
/// \date   01.03.2017
/// \brief  Declaration of CFloatingDockContainer class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QWidget>

namespace ads
{
struct FloatingDockContainerPrivate;
class CDockAreaWidget;
class CDockContainerWidget;
class CDockWidget;
class CDockManager;

/**
 * This implements a floating widget that is a dock container that accepts
 * docking of dock widgets like the main window and that can be docked into
 * another dock container
 */
class CFloatingDockContainer : public QWidget
{
	Q_OBJECT
private:
	FloatingDockContainerPrivate* d; ///< private data (pimpl)
	friend class FloatingDockContainerPrivate;

private slots:
	void onDockAreasAddedOrRemoved();
	void onDockAreaCurrentChanged(int Index);


protected: // reimplements QWidget
	virtual void changeEvent(QEvent *event) override;
	virtual void moveEvent(QMoveEvent *event) override;
	virtual bool event(QEvent *e) override;
	virtual void closeEvent(QCloseEvent *event) override;
	virtual void hideEvent(QHideEvent *event) override;
	virtual void showEvent(QShowEvent *event) override;
	virtual bool eventFilter(QObject *watched, QEvent *event) override;

public:
	/**
	 * Create empty flatingb widget - required for restore state
	 */
	CFloatingDockContainer(CDockManager* DockManager);

	/**
	 * Create floating widget with the given dock area
	 */
	CFloatingDockContainer(CDockAreaWidget* DockArea);

	/**
	 * Create floating widget with the given dock widget
	 */
	CFloatingDockContainer(CDockWidget* DockWidget);

	/**
	 * Virtual Destructor
	 */
	virtual ~CFloatingDockContainer();

	/**
	 * Access function for the internal dock container
	 */
	CDockContainerWidget* dockContainer() const;

	/**
	 * Starts floating at the given global position.
	 * Use moveToGlobalPos() to move the widget to a new position
	 * depending on the start position given in Pos parameter
	 */
	void startFloating(const QPoint& Pos, const QSize& Size = QSize());

	/**
	 * Moves the widget to a new position relative to the position given when
	 * startFloating() was called
	 */
	void moveFloating();
}; // class FloatingDockContainer
}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // FloatingDockContainerH
