//============================================================================
/// \file   DockContainerWidget.cpp
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Implementation of CDockContainerWidget class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockContainerWidget.h"

#include <QEvent>
#include <QList>
#include <QGridLayout>
#include <QSplitter>

#include "DockManager.h"
#include "DockAreaWidget.h"
#include "ads_globals.h"

namespace ads
{
static unsigned int zOrderCounter = 0;

/**
 * Helper function to ease insertion of dock area into splitter
 */
static void inserDockAreaIntoSplitter(QSplitter* Splitter, QWidget* widget, bool Append)
{
	if (Append)
	{
		Splitter->addWidget(widget);
	}
	else
	{
		Splitter->insertWidget(0, widget);
	}
}

/**
 * Private data class of CDockContainerWidget class (pimpl)
 */
struct DockContainerWidgetPrivate
{
	CDockContainerWidget* _this;
	CDockManager* DockManager = nullptr;
	unsigned int zOrderIndex = 0;
	QList<CDockAreaWidget*> DockAreas;
	QGridLayout* Layout = nullptr;

	/**
	 * Private data constructor
	 */
	DockContainerWidgetPrivate(CDockContainerWidget* _public);

	/**
	 * Create a new dock area widget and adds it to the list of doc areas
	 */
	CDockAreaWidget* newDockArea()
	{
		auto DockAreaWidget = new CDockAreaWidget(DockManager, _this);
		DockAreas.append(DockAreaWidget);
		return DockAreaWidget;
	}

	/**
	 * Adds dock widget to container
	 */
	void dockWidgetIntoContainer(DockWidgetArea area, CDockWidget* Dockwidget);

	/**
	 * Adds dock widget to a existing DockWidgetArea
	 */
	void dockWidgetIntoDockArea(DockWidgetArea area, CDockWidget* Dockwidget,
		CDockAreaWidget* DockAreaWidget);
}; // struct DockContainerWidgetPrivate


//============================================================================
DockContainerWidgetPrivate::DockContainerWidgetPrivate(CDockContainerWidget* _public) :
	_this(_public)
{

}


//============================================================================
void DockContainerWidgetPrivate::dockWidgetIntoContainer(DockWidgetArea area,
	CDockWidget* Dockwidget)
{
	CDockAreaWidget* NewDockArea = new CDockAreaWidget(DockManager, _this);
	NewDockArea->addDockWidget(Dockwidget);
	auto InsertParam = internal::dockAreaInsertParameters(area);

	if (DockAreas.isEmpty())
	{
		Layout->addWidget(NewDockArea, 0, 0);
	}
	else if (DockAreas.count() == 1)
	{
		QSplitter* Splitter = internal::newSplitter(InsertParam.first);
		auto DockArea = dynamic_cast<CDockAreaWidget*>(Layout->itemAtPosition(0, 0)->widget());
		Layout->replaceWidget(DockArea, Splitter);
		Splitter->addWidget(DockArea);
		inserDockAreaIntoSplitter(Splitter, NewDockArea, InsertParam.second);
	}
	else
	{
		QSplitter* Splitter = _this->findChild<QSplitter*>(QString(), Qt::FindDirectChildrenOnly);
		if (Splitter->orientation() == InsertParam.first)
		{
			inserDockAreaIntoSplitter(Splitter, NewDockArea, InsertParam.second);
		}
		else
		{
			QSplitter* NewSplitter = internal::newSplitter(InsertParam.first);
			if (InsertParam.second)
			{
				QLayoutItem* li = Layout->replaceWidget(Splitter, NewSplitter);
				NewSplitter->addWidget(Splitter);
				NewSplitter->addWidget(NewDockArea);
				delete li;
			}
			else
			{
				NewSplitter->addWidget(NewDockArea);
				QLayoutItem* li = Layout->replaceWidget(Splitter, NewSplitter);
				NewSplitter->addWidget(Splitter);
				delete li;
			}
		}
	}

	DockAreas.append(NewDockArea);
}


//============================================================================
void DockContainerWidgetPrivate::dockWidgetIntoDockArea(DockWidgetArea area,
	CDockWidget* Dockwidget, CDockAreaWidget* DockAreaWidget)
{

}


//============================================================================
CDockContainerWidget::CDockContainerWidget(CDockManager* DockManager, QWidget *parent) :
	QFrame(parent),
	d(new DockContainerWidgetPrivate(this))
{
	setStyleSheet("background: green;");
	d->DockManager = DockManager;

	d->Layout = new QGridLayout();
	d->Layout->setContentsMargins(0, 1, 0, 0);
	d->Layout->setSpacing(0);
	setLayout(d->Layout);
}

//============================================================================
CDockContainerWidget::~CDockContainerWidget()
{
	delete d;
}


//============================================================================
void CDockContainerWidget::addDockWidget(DockWidgetArea area, CDockWidget* Dockwidget,
	CDockAreaWidget* DockAreaWidget)
{
	if (DockAreaWidget)
	{
		d->dockWidgetIntoDockArea(area, Dockwidget, DockAreaWidget);
	}
	else
	{
		d->dockWidgetIntoContainer(area, Dockwidget);
	}
}


//============================================================================
unsigned int CDockContainerWidget::zOrderIndex() const
{
	return d->zOrderIndex;
}


//============================================================================
bool CDockContainerWidget::isInFrontOf(CDockContainerWidget* Other) const
{
	return this->zOrderIndex() > Other->zOrderIndex();
}


//============================================================================
bool CDockContainerWidget::event(QEvent *e)
{
	bool Result = QWidget::event(e);
	if (e->type() == QEvent::WindowActivate)
    {
        d->zOrderIndex = ++zOrderCounter;
    }
	else if (e->type() == QEvent::Show && !d->zOrderIndex)
	{
		d->zOrderIndex = ++zOrderCounter;
	}

	return Result;
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockContainerWidget.cpp
