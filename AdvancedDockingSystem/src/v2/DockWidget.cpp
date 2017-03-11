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
	QString CapturedState;

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
	QString CapturedState;
	QTextStream stream(&CapturedState);
	stream << (_this->isFloating() ? "F " : "D ");
	if (_this->isFloating())
	{
		CFloatingDockContainer* FloatingWidget = internal::findParent<CFloatingDockContainer*>(_this);
		QRect Rect = FloatingWidget->geometry();
		stream << Rect.left() << " " << Rect.top() << " " << Rect.width()
			<< " " << Rect.height();
	}
	else
	{
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
	}
	this->CapturedState = CapturedState;
	std::cout << "SerializedPosition: " << CapturedState.toStdString() << std::endl;
}


//============================================================================
void DockWidgetPrivate::showDockWidget()
{
	QTextStream stream(&CapturedState);
	QString DockedState;
	stream >> DockedState;
	if (DockedState == "F")
	{
		std::cout << "Restoring Floating Dock Widget" << std::endl;
		QVector<int> v;
		while (!stream.atEnd())
		{
			int Value;
			stream >> Value;
			v.append(Value);
		}

		if (v.count() == 4)
		{
			std::cout << "Rectangle Loaded" << std::endl;
			QRect Rect(v[0], v[1], v[2], v[3]);
			auto FloatingWidget = new CFloatingDockContainer(_this);
			FloatingWidget->setGeometry(Rect);
			FloatingWidget->show();
		}
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
	if ((d->DockArea != nullptr) == Open)
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
	d->capturedState();
	if (d->DockArea && RemoveFromDockArea)
	{
		d->DockArea->removeDockWidget(this);
	}
	this->setParent(d->DockManager);
	this->setDockArea(nullptr);
	// Remove title from dock area widget to prevent its deletion if dock
	// area is deleted
	d->TitleWidget->setParent(this);
}

} // namespace ads

//---------------------------------------------------------------------------
// EOF DockWidget.cpp
