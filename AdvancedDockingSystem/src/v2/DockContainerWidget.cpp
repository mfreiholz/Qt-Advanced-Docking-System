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

#include "DockManager.h"
#include "DockAreaWidget.h"

namespace ads
{
static unsigned int zOrderCounter = 0;

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
	CDockAreaWidget* newDockAreaWidget()
	{
		auto DockAreaWidget = new CDockAreaWidget(DockManager, _this);
		DockAreas.append(DockAreaWidget);
		return DockAreaWidget;
	}
}; // struct DockContainerWidgetPrivate


//============================================================================
DockContainerWidgetPrivate::DockContainerWidgetPrivate(CDockContainerWidget* _public) :
	_this(_public)
{

}

//============================================================================
CDockContainerWidget::CDockContainerWidget(CDockManager* DockManager, QWidget *parent) :
	QFrame(parent),
	d(new DockContainerWidgetPrivate(this))
{
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
	if (d->DockAreas.isEmpty())
	{

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
