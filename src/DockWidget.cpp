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
	/*if (!CapturedState.DockContainer)
	{
		auto FloatingWidget = new CFloatingDockContainer(_this);
		FloatingWidget->setGeometry(CapturedState.GlobalGeometry);
		FloatingWidget->show();
		return;
	}

	CDockContainerWidget* DockContainer = CapturedState.DockContainer.data();
	QStringList DockTree = this->CapturedState.DockTreePosition.split(' ');
	QSplitter* splitter = DockContainer->findChild<QSplitter*>(QString(), Qt::FindDirectChildrenOnly);

	while (splitter)
	{

	}

	for (const auto& TreeItem : DockTree)
	{

	}*/

	std::cout << "DockWidgetPrivate::showDockWidget()" << std::endl;
	_this->show();
	DockArea->show();

	QSplitter* Splitter = internal::findParent<QSplitter*>(_this);
	if (Splitter)
	{
		Splitter->show();
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
QAction* CDockWidget::toggleViewAction() const
{
	return d->ToggleViewAction;
}


//============================================================================
void CDockWidget::toggleView(bool Open)
{
	/*if ((d->DockArea != nullptr) == Open)
	{
		return;
	}

	if (!Open && d->DockArea)
	{
		hideDockWidget(true);
	}
	else if (Open && !d->DockArea)
	{
		d->showDockWidget();
	}*/

	if (Open)
	{
		d->showDockWidget();
	}
	else
	{
		hideDockWidget(true);
	}
}


//============================================================================
void CDockWidget::setDockArea(CDockAreaWidget* DockArea)
{
	d->DockArea = DockArea;
	d->ToggleViewAction->setChecked(DockArea != nullptr);
}



//============================================================================
void CDockWidget::hideDockWidget(bool RemoveFromDockArea)
{
	/*d->capturedState();
	if (d->DockArea && RemoveFromDockArea)
	{
		d->DockArea->removeDockWidget(this);
	}
	this->setParent(d->DockManager);
	this->setDockArea(nullptr);
	// Remove title from dock area widget to prevent its deletion if dock
	// area is deleted
	d->TitleWidget->setParent(this);*/

	std::cout << "CDockWidget::hideDockWidget" << std::endl;
	this->hide();
	d->ToggleViewAction->setChecked(false);
	d->TitleWidget->hide();
	CDockAreaWidget* DockArea = d->DockArea;
	for (int i = 0; i < DockArea->count(); ++i)
	{
		if (DockArea->dockWidget(i)->isVisible())
		{
			return;
		}
	}

	if (DockArea->count() > 1)
	{
		if (DockArea->currentIndex() == (DockArea->count() - 1))
		{
			DockArea->setCurrentIndex(DockArea->currentIndex() - 1);
		}
		else
		{
			DockArea->setCurrentIndex(DockArea->currentIndex() + 1);
		}
	}
	QSplitter* Splitter = internal::findParent<QSplitter*>(this);
	if (!Splitter)
	{
		return;
	}

	std::cout << "DockWidgets " << Splitter->count() << std::endl;
	for (int i = 0; i < Splitter->count(); ++i)
	{
		if (Splitter->widget(i)->isVisible())
		{
			return;
		}
	}

	Splitter->hide();
}

} // namespace ads

//---------------------------------------------------------------------------
// EOF DockWidget.cpp
