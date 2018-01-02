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
/// \file   DockWidget.cpp
/// \author Uwe Kindler
/// \date   26.02.2017
/// \brief  Implementation of CDockWidget class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockWidget.h"

#include <QBoxLayout>
#include <QAction>
#include <QSplitter>
#include <QStack>
#include <QTextStream>
#include <QPointer>
#include <QEvent>
#include <QDebug>
#include <QXmlStreamWriter>

#include "DockWidgetTitleBar.h"
#include "DockContainerWidget.h"
#include "DockAreaWidget.h"
#include "DockManager.h"
#include "FloatingDockContainer.h"
#include "DockStateSerialization.h"
#include "DockSplitter.h"
#include "ads_globals.h"

namespace ads
{
/**
 * Private data class of CDockWidget class (pimpl)
 */
struct DockWidgetPrivate
{
	CDockWidget* _this;
	QBoxLayout* Layout;
	QWidget* Widget = nullptr;
	CDockWidgetTitleBar* TitleWidget;
	CDockWidget::DockWidgetFeatures Features = CDockWidget::AllDockWidgetFeatures;
	CDockManager* DockManager = nullptr;
	CDockAreaWidget* DockArea = nullptr;
	QAction* ToggleViewAction;
	bool Closed = false;

	/**
	 * Private data constructor
	 */
	DockWidgetPrivate(CDockWidget* _public);

	/**
	 * Show dock widget
	 */
	void showDockWidget();

	/**
	 * Hide dock widget.
	 */
	void hideDockWidget();

	/**
	 * Hides a parent splitter if all dock widgets in the splitter are closed
	 */
	void hideEmptyParentSplitters();

	/**
	 * Hides a dock area if all dock widgets in the area are closed
	 */
	void hideEmptyParentDockArea();

	/**
	 * Hides a floating widget if all dock areas are empty - that means,
	 * if all dock widgets in all dock areas are closed
	 */
	void hideEmptyFloatingWidget();
};
// struct DockWidgetPrivate

//============================================================================
DockWidgetPrivate::DockWidgetPrivate(CDockWidget* _public) :
	_this(_public)
{

}


//============================================================================
void DockWidgetPrivate::showDockWidget()
{
	if (!DockArea)
	{
		CFloatingDockContainer* FloatingWidget = new CFloatingDockContainer(_this);
		FloatingWidget->resize(_this->size());
		FloatingWidget->show();
	}
	else
	{
		DockArea->show();
		DockArea->setCurrentIndex(DockArea->tabIndex(_this));
		QSplitter* Splitter = internal::findParent<QSplitter*>(_this);
		while (Splitter && !Splitter->isVisible())
		{
			Splitter->show();
			Splitter = internal::findParent<QSplitter*>(Splitter);
		}

		CDockContainerWidget* Container = DockArea->dockContainer();
		if (Container->isFloating())
		{
			CFloatingDockContainer* FloatingWidget = internal::findParent<
					CFloatingDockContainer*>(Container);
			FloatingWidget->show();
		}
	}
}


//============================================================================
void DockWidgetPrivate::hideDockWidget()
{
	TitleWidget->hide();
	hideEmptyParentDockArea();
	hideEmptyParentSplitters();
	hideEmptyFloatingWidget();
}


//============================================================================
void DockWidgetPrivate::hideEmptyParentSplitters()
{
	auto Splitter = internal::findParent<CDockSplitter*>(_this);
	while (Splitter && Splitter->isVisible())
	{
		if (!Splitter->hasVisibleContent())
		{
			Splitter->hide();
		}
		Splitter = internal::findParent<CDockSplitter*>(Splitter);
	}
}


//============================================================================
void DockWidgetPrivate::hideEmptyParentDockArea()
{
	auto OpenDockWidgets = DockArea->openedDockWidgets();
	if (OpenDockWidgets.count() > 1)
	{
		CDockWidget* NextDockWidget;
		if (OpenDockWidgets.last() == _this)
		{
			NextDockWidget = OpenDockWidgets[OpenDockWidgets.count() - 2];
		}
		else
		{
			int NextIndex = OpenDockWidgets.indexOf(_this) + 1;
			NextDockWidget = OpenDockWidgets[NextIndex];
		}

		DockArea->setCurrentDockWidget(NextDockWidget);
	}
	else
	{
		DockArea->hide();
	}
}


//============================================================================
void DockWidgetPrivate::hideEmptyFloatingWidget()
{
	CDockContainerWidget* Container = _this->dockContainer();
	if (Container->isFloating() && Container->openedDockAreas().isEmpty())
	{
		CFloatingDockContainer* FloatingWidget = internal::findParent<CFloatingDockContainer*>(Container);
		FloatingWidget->hide();
	}
}


//============================================================================
CDockWidget::CDockWidget(const QString &title, QWidget *parent) :
	QFrame(parent),
	d(new DockWidgetPrivate(this))
{
	d->Layout = new QBoxLayout(QBoxLayout::TopToBottom);
	d->Layout->setContentsMargins(0, 0, 0, 0);
	d->Layout->setSpacing(0);
	setLayout(d->Layout);
	setWindowTitle(title);

	d->TitleWidget = new CDockWidgetTitleBar(this);
	d->ToggleViewAction = new QAction(title);
	d->ToggleViewAction->setCheckable(true);
	connect(d->ToggleViewAction, SIGNAL(triggered(bool)), this,
		SLOT(toggleView(bool)));
}

//============================================================================
CDockWidget::~CDockWidget()
{
	qDebug() << "~CDockWidget()";
	delete d;
}


//============================================================================
void CDockWidget::setToggleViewActionChecked(bool Checked)
{
	QAction* Action = d->ToggleViewAction;
	Action->blockSignals(true);
	Action->setChecked(Checked);
	Action->blockSignals(false);
}


//============================================================================
void CDockWidget::setWidget(QWidget* widget)
{
	if (d->Widget)
	{
		d->Layout->replaceWidget(d->Widget, widget);
	}
	else
	{
		d->Layout->addWidget(widget);
	}

	d->Widget = widget;
}


//============================================================================
QWidget* CDockWidget::widget() const
{
	return d->Widget;
}


//============================================================================
CDockWidgetTitleBar* CDockWidget::titleBar() const
{
	return d->TitleWidget;
}


//============================================================================
void CDockWidget::setFeatures(DockWidgetFeatures features)
{
	d->Features = features;
}


//============================================================================
CDockWidget::DockWidgetFeatures CDockWidget::features() const
{
	return d->Features;
}


//============================================================================
CDockManager* CDockWidget::dockManager() const
{
	return d->DockManager;
}


//============================================================================
void CDockWidget::setDockManager(CDockManager* DockManager)
{
	d->DockManager = DockManager;
}


//============================================================================
CDockContainerWidget* CDockWidget::dockContainer() const
{
	return internal::findParent<CDockContainerWidget*>(this);
}


//============================================================================
CDockAreaWidget* CDockWidget::dockAreaWidget() const
{
	return internal::findParent<CDockAreaWidget*>(this);
}


//============================================================================
bool CDockWidget::isFloating() const
{
	return dockContainer() ? dockContainer()->isFloating() : false;
}


//============================================================================
bool CDockWidget::isClosed() const
{
	return d->Closed;
}


//============================================================================
QAction* CDockWidget::toggleViewAction() const
{
	return d->ToggleViewAction;
}


//============================================================================
void CDockWidget::setToggleViewActionMode(eToggleViewActionMode Mode)
{
	if (ActionModeToggle == Mode)
	{
		d->ToggleViewAction->setCheckable(true);
		d->ToggleViewAction->setIcon(QIcon());
	}
	else
	{
		d->ToggleViewAction->setCheckable(false);
		d->ToggleViewAction->setIcon(d->TitleWidget->icon());
	}
}


//============================================================================
void CDockWidget::toggleView(bool Open)
{
	QAction* Sender = qobject_cast<QAction*>(sender());
	if (Sender == d->ToggleViewAction && !d->ToggleViewAction->isCheckable())
	{
		Open = true;
	}

	if (Open)
	{
		d->showDockWidget();
	}
	else
	{
		d->hideDockWidget();
	}
	d->Closed = !Open;
	d->ToggleViewAction->blockSignals(true);
	d->ToggleViewAction->setChecked(Open);
	d->ToggleViewAction->blockSignals(false);
	if (!Open)
	{
		emit closed();
	}
	emit viewToggled(Open);
}


//============================================================================
void CDockWidget::setDockArea(CDockAreaWidget* DockArea)
{
	d->DockArea = DockArea;
	d->ToggleViewAction->setChecked(DockArea != nullptr);
}


//============================================================================
void CDockWidget::saveState(QXmlStreamWriter& s) const
{
	s.writeStartElement("DockWidget");
	s.writeAttribute("ObjectName", objectName());
	s.writeAttribute("Closed", QString::number(d->Closed ? 1 : 0));
	s.writeEndElement();
}


//============================================================================
void CDockWidget::flagAsUnassigned()
{
	setParent(d->DockManager);
	setDockArea(nullptr);
	titleBar()->setParent(this);
}


//============================================================================
bool CDockWidget::event(QEvent *e)
{
	if (e->type() == QEvent::WindowTitleChange)
	{
		emit titleChanged(windowTitle());
	}
	return QFrame::event(e);
}


//============================================================================
void CDockWidget::setIcon(const QIcon& Icon)
{
	d->TitleWidget->setIcon(Icon);
	if (!d->ToggleViewAction->isCheckable())
	{
		d->ToggleViewAction->setIcon(Icon);
	}
}



} // namespace ads

//---------------------------------------------------------------------------
// EOF DockWidget.cpp
