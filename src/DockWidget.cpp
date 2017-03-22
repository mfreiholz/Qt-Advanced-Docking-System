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

#include <iostream>

#include "DockWidgetTitleBar.h"
#include "DockContainerWidget.h"
#include "DockAreaWidget.h"
#include "DockManager.h"
#include "FloatingDockContainer.h"

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
	struct CapturedState
	{
		QString DockTreePosition;
		QRect GlobalGeometry;
		QPointer<CDockContainerWidget> DockContainer;
	}  CapturedState;

	/**
	 * Private data constructor
	 */
	DockWidgetPrivate(CDockWidget* _public);

	/**
	 * Saves the current state into CapturedState variable
	 */
	void capturedState();

	/**
	 * Show dock widget
	 */
	void showDockWidget();

	/**
	 * Hides a parent splitter if all dock widgets in the splitter are closed
	 */
	void hideEmptyParentSplitter();

	/**
	 * Hides a dock area if all dock widgets in the area are closed
	 */
	void hideEmptyParentDockArea();
};
// struct DockWidgetPrivate

//============================================================================
DockWidgetPrivate::DockWidgetPrivate(CDockWidget* _public) :
	_this(_public)
{

}


//============================================================================
void DockWidgetPrivate::capturedState()
{
	QString DockTreePosition;
	QTextStream stream(&DockTreePosition);

	QPoint GlobalTopLeft = _this->mapToGlobal(_this->geometry().topLeft());
	QRect Rect(GlobalTopLeft, _this->geometry().size());
	CapturedState.GlobalGeometry = Rect;
	CapturedState.DockContainer = _this->dockContainer();

	QWidget* Widget = DockArea;
	QSplitter* splitter = internal::findParent<QSplitter*>(Widget);
	QStack<QString> SplitterData;
	while (splitter)
	{
		SplitterData.push(QString("%1%2")
			.arg((splitter->orientation() == Qt::Horizontal) ? "H" : "V")
			.arg(splitter->indexOf(Widget)));
		Widget = splitter;
		splitter = internal::findParent<QSplitter*>(Widget);
	}

	QString Separator;
	while (!SplitterData.isEmpty())
	{
		stream << Separator << SplitterData.pop();
		Separator = " ";
	}
	this->CapturedState.DockTreePosition = DockTreePosition;
	std::cout << "SerializedPosition: " << DockTreePosition.toStdString() << std::endl;
}


//============================================================================
void DockWidgetPrivate::showDockWidget()
{
	DockArea->show();
	DockArea->setCurrentIndex(DockArea->tabIndex(_this));
	QSplitter* Splitter = internal::findParent<QSplitter*>(_this);
	if (Splitter)
	{
		Splitter->show();
	}
}


//============================================================================
void DockWidgetPrivate::hideEmptyParentSplitter()
{
	QSplitter* Splitter = internal::findParent<QSplitter*>(_this);
	if (!Splitter)
	{
		return;
	}

	for (int i = 0; i < Splitter->count(); ++i)
	{
		if (Splitter->widget(i)->isVisible())
		{
			return;
		}
	}

	Splitter->hide();
}


//============================================================================
void DockWidgetPrivate::hideEmptyParentDockArea()
{
	auto OpenDockWidgets = DockArea->openDockWidgets();
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
	std::cout << "~CDockWidget()" << std::endl;
	delete d;
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
void CDockWidget::toggleView(bool Open)
{
	if (Open)
	{
		d->showDockWidget();
	}
	else
	{
		hideDockWidget();
	}
	d->Closed = !Open;
}


//============================================================================
void CDockWidget::setDockArea(CDockAreaWidget* DockArea)
{
	d->DockArea = DockArea;
	d->ToggleViewAction->setChecked(DockArea != nullptr);
}



//============================================================================
void CDockWidget::hideDockWidget()
{
	CDockAreaWidget* DockArea = d->DockArea;
	d->ToggleViewAction->setChecked(false);
	d->TitleWidget->hide();
	d->hideEmptyParentDockArea();
	d->hideEmptyParentSplitter();
}

} // namespace ads

//---------------------------------------------------------------------------
// EOF DockWidget.cpp
