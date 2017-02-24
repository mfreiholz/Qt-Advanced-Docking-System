//============================================================================
/// \file   DockAreaWidget.cpp
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Implementation of CDockAreaWidget class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockAreaWidget.h"

#include "DockContainerWidget.h"

namespace ads
{
/**
 * Private data class of CDockAreaWidget class (pimpl)
 */
struct DockAreaWidgetPrivate
{
	CDockAreaWidget* _this;

	/**
	 * Private data constructor
	 */
	DockAreaWidgetPrivate(CDockAreaWidget* _public);
};
// struct DockAreaWidgetPrivate

//============================================================================
DockAreaWidgetPrivate::DockAreaWidgetPrivate(CDockAreaWidget* _public) :
	_this(_public)
{

}

//============================================================================
CDockAreaWidget::CDockAreaWidget(CDockManager* DockManager, CDockContainerWidget* parent) :
	QFrame(parent),
	d(new DockAreaWidgetPrivate(this))
{

}

//============================================================================
CDockAreaWidget::~CDockAreaWidget()
{
	delete d;
}


//============================================================================
CDockContainerWidget* CDockAreaWidget::dockContainerWidget() const
{
	QWidget* Parent = parentWidget();
	while (Parent)
	{
		CDockContainerWidget* Container = dynamic_cast<CDockContainerWidget*>(Parent);
		if (Container)
		{
			return Container;
		}
		Parent = Parent->parentWidget();
	}

	return 0;
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockAreaWidget.cpp
