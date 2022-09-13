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
/// \file   DockWidgetTab.h
/// \author Syarif Fakhri
/// \date   05.09.2022
/// \brief  Implementation of CDockWidgetSideTab class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockWidgetSideTab.h"
#include "SideTabBar.h"

#include <QBoxLayout>

#include "DockAreaWidget.h"
#include "DockManager.h"
#include "ElidingLabel.h"

#include "DockWidget.h"
#include "OverlayDockContainer.h"

namespace ads
{

using tTabLabel = CVerticalElidingLabel;

/**
 * Private data class of CDockWidgetTab class (pimpl)
 */
struct DockWidgetSideTabPrivate
{
    CDockWidgetSideTab* _this;
    CDockWidget* DockWidget;
    tTabLabel* TitleLabel;
	QBoxLayout* Layout;
	QBoxLayout* TitleLayout; // To have independent spacing from the icon
	CSideTabBar* SideTabBar;
	QSize IconSize;
	Qt::Orientation Orientation{Qt::Vertical};
	SideTabIconLabel* IconLabel = nullptr;
	QIcon Icon;

	/**
	 * Private data constructor
	 */
	DockWidgetSideTabPrivate(CDockWidgetSideTab* _public);

	/**
	 * Creates the complete layout
	 */
	void createLayout();

	/**
	 * Update the icon in case the icon size changed
	 */
	void updateIcon()
	{
		if (!IconLabel || Icon.isNull())
		{
			return;
		}

		if (IconSize.isValid())
		{
			IconLabel->setPixmap(Icon.pixmap(IconSize));
		}
		else
		{
			IconLabel->setPixmap(Icon.pixmap(_this->style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, _this)));
		}
		IconLabel->setVisible(true);
	}

	void updateContentsMargins()
	{
        QFontMetrics fm(TitleLabel->font());
        int Spacing = qRound(fm.height() / 2.0);

		if (Orientation == Qt::Vertical)
		{
            TitleLayout->setContentsMargins(Spacing, Spacing, 0, Spacing);
			if (IconLabel)
			{
                IconLabel->setContentsMargins(Spacing / 2, Spacing / 2, Spacing / 2, 0);
			}
		}
		else if (Orientation == Qt::Horizontal)
		{
            TitleLayout->setContentsMargins(Spacing,Spacing / 2,Spacing,Spacing);
			if (IconLabel)
			{
                IconLabel->setContentsMargins(Spacing / 2, Spacing / 2, Spacing / 2, 0);
			}
		}
	}
}; // struct DockWidgetTabPrivate


//============================================================================
DockWidgetSideTabPrivate::DockWidgetSideTabPrivate(CDockWidgetSideTab* _public) :
	_this(_public)
{

}

//============================================================================
void DockWidgetSideTabPrivate::createLayout()
{
	TitleLabel = new tTabLabel();
	TitleLabel->setElideMode(Qt::ElideRight);
	TitleLabel->setText(DockWidget->windowTitle());
	TitleLabel->setObjectName("dockWidgetTabLabel");
	//TitleLabel->setAlignment(Qt::AlignLeft);
	_this->connect(TitleLabel, SIGNAL(elidedChanged(bool)), SIGNAL(elidedChanged(bool)));

	QFontMetrics fm(TitleLabel->font());
	int Spacing = qRound(fm.height() / 2.0);

	// Fill the layout
    // Purely for spacing on the text without messing up spacing on the icon
	TitleLayout = new QBoxLayout(QBoxLayout::TopToBottom); 
	TitleLayout->addWidget(TitleLabel);
	TitleLayout->setSpacing(0);

	Layout = new QBoxLayout(QBoxLayout::TopToBottom);
	Layout->setContentsMargins(0,0,0,0);
	Layout->setSpacing(0);
	_this->setLayout(Layout);
	Layout->addLayout(TitleLayout, 1);

	updateContentsMargins();

	TitleLabel->setVisible(true);
}

//============================================================================
void CDockWidgetSideTab::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
        emit clicked();
	}

    QFrame::mousePressEvent(event);
}


//============================================================================
void CDockWidgetSideTab::setSideTabBar(CSideTabBar* SideTabBar)
{
	d->SideTabBar = SideTabBar;
}


//============================================================================
void CDockWidgetSideTab::removeFromSideTabBar()
{
	if (d->SideTabBar == nullptr)
	{
		return;
	}
	d->SideTabBar->removeSideTab(this);
    setSideTabBar(nullptr);
}

//============================================================================
CDockWidgetSideTab::CDockWidgetSideTab(CDockWidget* DockWidget, QWidget* parent) : 
	QFrame(parent),
	d(new DockWidgetSideTabPrivate(this))
{
	setAttribute(Qt::WA_NoMousePropagation);
	d->DockWidget = DockWidget;
	d->createLayout();
	setFocusPolicy(Qt::NoFocus);
}


//============================================================================
CDockWidgetSideTab::~CDockWidgetSideTab()
{
	delete d;
}


//============================================================================
void CDockWidgetSideTab::updateStyle()
{
	internal::repolishStyle(this, internal::RepolishDirectChildren);
}


//============================================================================
CDockWidgetSideTab::SideTabBarArea CDockWidgetSideTab::sideTabBarArea() const
{
	auto dockAreaWidget = d->DockWidget->dockAreaWidget();
	if (dockAreaWidget && dockAreaWidget->isOverlayed())
	{
		return dockAreaWidget->overlayDockContainer()->sideTabBarArea();
	}

	return Left;
}


//============================================================================
void CDockWidgetSideTab::setIcon(const QIcon& Icon)
{
    QBoxLayout* Layout = qobject_cast<QBoxLayout*>(layout());
	if (!d->IconLabel && Icon.isNull())
	{
		return;
	}

	if (!d->IconLabel)
	{
		d->IconLabel = new SideTabIconLabel();
		internal::setToolTip(d->IconLabel, d->TitleLabel->toolTip());
		Layout->insertWidget(0, d->IconLabel, Qt::AlignHCenter);
	}
	else if (Icon.isNull())
	{
		// Remove icon label
		Layout->removeWidget(d->IconLabel);
		delete d->IconLabel;
		d->IconLabel = nullptr;
	}

	d->Icon = Icon;
	d->updateIcon();
}


//============================================================================
QSize CDockWidgetSideTab::iconSize() const
{
	return d->IconSize;
}


//============================================================================
void CDockWidgetSideTab::setIconSize(const QSize& Size)
{
    d->IconSize = Size;
	d->updateIcon();
}


//============================================================================
void CDockWidgetSideTab::setOrientation(Qt::Orientation Orientation)
{
	d->Orientation = Orientation;
	d->Layout->setDirection(Orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);
	d->TitleLabel->setOrientation(Orientation);
}


//============================================================================
void CDockWidgetSideTab::updateOrientationAndSpacing(SideTabBarArea area)
{
	setOrientation(area == Bottom ? Qt::Horizontal : Qt::Vertical);

	d->updateContentsMargins();

	// Handle Icon changes
    if (d->Icon.isNull())
    {
		return;
    }

    QFontMetrics fm(d->TitleLabel->font());
    int Spacing = qRound(fm.height() / 2.0);

	if (CDockManager::testConfigFlag(CDockManager::LeftSideBarPrioritizeIconOnly) && area == Left)
	{
		d->TitleLabel->hide();
		d->TitleLayout->setContentsMargins(0, 0, 0, 0);
        d->IconLabel->setContentsMargins(Spacing / 2, Spacing / 2, Spacing / 2, Spacing / 2);
		return;
	}
	if (CDockManager::testConfigFlag(CDockManager::RightSideBarPrioritizeIconOnly) && area == Right)
	{
		d->TitleLabel->hide();
		d->TitleLayout->setContentsMargins(0, 0, 0, 0);
        d->IconLabel->setContentsMargins(Spacing / 2, Spacing / 2, Spacing, Spacing / 2);
		return;
	}

	d->TitleLabel->show();
}

/**
 * Private data class of SideTabIcon class (pimpl)
 */
struct SideTabIconLabelPrivate
{
	SideTabIconLabel* _this;
	QLabel* IconLabel;
	QBoxLayout* Layout;

	SideTabIconLabelPrivate(SideTabIconLabel* _public);
}; // struct SideTabIconLabelPrivate


//============================================================================
SideTabIconLabelPrivate::SideTabIconLabelPrivate(SideTabIconLabel* _public) :
    _this(_public)
{
}


//============================================================================
SideTabIconLabel::SideTabIconLabel(QWidget* parent) : QWidget(parent),
	d(new SideTabIconLabelPrivate(this))
{
	d->Layout = new QBoxLayout(QBoxLayout::TopToBottom);
	d->Layout->addWidget(d->IconLabel = new QLabel());
	d->IconLabel->setAlignment(Qt::AlignHCenter);
    d->IconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	setLayout(d->Layout);
}


//============================================================================
SideTabIconLabel::~SideTabIconLabel()
{
	delete d;
}


//============================================================================
void SideTabIconLabel::setPixmap(const QPixmap& pixmap)
{
	d->IconLabel->setPixmap(pixmap);
}


//============================================================================
void SideTabIconLabel::setContentsMargins(int left, int top, int right, int bottom)
{
	d->Layout->setContentsMargins(left, top, right, bottom);
}
}
