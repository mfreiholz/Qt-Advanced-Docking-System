
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

	/**
	 * Private data constructor
	 */
	DockWidgetPrivate(CDockWidget* _public);
};
// struct DockWidgetPrivate

//============================================================================
DockWidgetPrivate::DockWidgetPrivate(CDockWidget* _public) :
	_this(_public)
{

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
}

//============================================================================
CDockWidget::~CDockWidget()
{
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
} // namespace ads

//---------------------------------------------------------------------------
// EOF DockWidget.cpp
