//============================================================================
/// \file   ContainerWidget.cpp
/// \author Uwe Kindler
/// \date   03.02.2017
/// \brief  Implementation of CContainerWidget
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "ads/ContainerWidget.h"

#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QContextMenuEvent>
#include <QMenu>
#include <QSplitter>
#include <QDataStream>
#include <QtGlobal>
#include <QGridLayout>
#include <QPoint>
#include <QApplication>

#include <iostream>

#include "ads/Internal.h"
#include "ads/SectionWidget.h"
#include "ads/SectionTitleWidget.h"
#include "ads/SectionContentWidget.h"
#include "ads/DropOverlay.h"
#include "ads/Serialization.h"
#include "ads/MainContainerWidget.h"

namespace ads
{
unsigned int CContainerWidget::zOrderCounter = 0;

//============================================================================
CContainerWidget::CContainerWidget(MainContainerWidget* MainContainer, QWidget *parent)
	: QFrame(parent),
	  m_MainContainerWidget(MainContainer)
{
	m_MainLayout = new QGridLayout();
	m_MainLayout->setContentsMargins(0, 1, 0, 0);
	m_MainLayout->setSpacing(0);
	setLayout(m_MainLayout);
}


//============================================================================
CContainerWidget::~CContainerWidget()
{
	std::cout << "CContainerWidget::~CContainerWidget()" << std::endl;
}


bool CContainerWidget::event(QEvent *e)
{
	bool Result = QWidget::event(e);
	if (e->type() == QEvent::WindowActivate)
    {
        m_zOrderIndex = ++zOrderCounter;
    }
	else if (e->type() == QEvent::Show && !m_zOrderIndex)
	{
		m_zOrderIndex = ++zOrderCounter;
	}

	return Result;
}


unsigned int CContainerWidget::zOrderIndex() const
{
	return m_zOrderIndex;
}

void CContainerWidget::dropFloatingWidget(FloatingWidget* FloatingWidget,
	const QPoint& TargetPos)
{
	if (!FloatingWidget->isDraggingActive())
	{
		return;
	}

	QPoint MousePos = mapFromGlobal(TargetPos);
	SectionWidget* sectionWidget = sectionWidgetAt(MousePos);
	DropArea dropArea = InvalidDropArea;
	if (sectionWidget)
	{
		//m_SectionDropOverlay->setAllowedAreas(ADS_NS::AllAreas);
		//dropArea = m_SectionDropOverlay->showDropOverlay(sectionWidget);
		if (dropArea != InvalidDropArea)
		{
			InternalContentData data;
			FloatingWidget->takeContent(data);
			FloatingWidget->deleteLater();
			//dropContent(data, sectionWidget, dropArea, true);
		}
	}

	// mouse is over container
	if (InvalidDropArea == dropArea)
	{
		//dropArea = m_ContainerDropOverlay->dropAreaUnderCursor();
		std::cout << "Cursor location: " << dropArea << std::endl;
		if (dropArea != InvalidDropArea)
		{
			InternalContentData data;
			FloatingWidget->takeContent(data);
			FloatingWidget->deleteLater();
			//dropContent(data, nullptr, dropArea, true);
		}
	}
}


SectionWidget* CContainerWidget::sectionWidgetAt(const QPoint& pos) const
{
	const QPoint gpos = mapToGlobal(pos);
	for (int i = 0; i < m_Sections.size(); ++i)
	{
		SectionWidget* sw = m_Sections[i];
		if (sw->rect().contains(sw->mapFromGlobal(gpos)))
		{
			return sw;
		}
	}
	return 0;
}


bool CContainerWidget::isInFrontOf(CContainerWidget* Other) const
{
	return this->m_zOrderIndex > Other->m_zOrderIndex;
}

SectionWidget*  CContainerWidget::dropContent(const InternalContentData& data, SectionWidget* targetSectionWidget, DropArea area, bool autoActive)
{
	ADS_Expects(targetSection != NULL);

	SectionWidget* section_widget = nullptr;

	// If no sections exists yet, create a default one and always drop into it.
	if (m_Sections.isEmpty())
	{
		targetSectionWidget = newSectionWidget();
		addSectionWidget(targetSectionWidget);
		area = CenterDropArea;
	}

	// Drop on outer area
	if (!targetSectionWidget)
	{
		switch (area)
		{
		case TopDropArea:return dropContentOuterHelper(m_MainLayout, data, Qt::Vertical, false);
		case RightDropArea: return dropContentOuterHelper(m_MainLayout, data, Qt::Horizontal, true);
		case CenterDropArea:
		case BottomDropArea:return dropContentOuterHelper(m_MainLayout, data, Qt::Vertical, true);
		case LeftDropArea: return dropContentOuterHelper(m_MainLayout, data, Qt::Horizontal, false);
		default:
			return nullptr;
		}
		return section_widget;
	}

	// Drop logic based on area.
	switch (area)
	{
	case TopDropArea:return insertNewSectionWidget(data, targetSectionWidget, section_widget, Qt::Vertical, 0);
	case RightDropArea: return insertNewSectionWidget(data, targetSectionWidget, section_widget, Qt::Horizontal, 1);
	case BottomDropArea: return insertNewSectionWidget(data, targetSectionWidget, section_widget, Qt::Vertical, 1);
	case LeftDropArea: return insertNewSectionWidget(data, targetSectionWidget, section_widget, Qt::Horizontal, 0);
	case CenterDropArea:
		 targetSectionWidget->addContent(data, autoActive);
		 return targetSectionWidget;

	default:
		break;
	}
	return section_widget;
}


SectionWidget* CContainerWidget::newSectionWidget()
{
	SectionWidget* sw = new SectionWidget(m_MainContainerWidget, this);
	m_Sections.append(sw);
	return sw;
}

void CContainerWidget::addSectionWidget(SectionWidget* section)
{
	ADS_Expects(section != NULL);

	// Create default splitter.
	if (!m_Splitter)
	{
		m_Splitter = MainContainerWidget::newSplitter(m_Orientation);
		m_MainLayout->addWidget(m_Splitter, 0, 0);
	}
	if (m_Splitter->indexOf(section) != -1)
	{
		qWarning() << Q_FUNC_INFO << QString("Section has already been added");
		return;
	}
	m_Splitter->addWidget(section);
}

SectionWidget* CContainerWidget::dropContentOuterHelper(QLayout* l, const InternalContentData& data, Qt::Orientation orientation, bool append)
{
	ADS_Expects(l != NULL);

	SectionWidget* sw = newSectionWidget();
	sw->addContent(data, true);

	QSplitter* oldsp = findImmediateSplitter(this);
	if (!oldsp)
	{
		QSplitter* sp = MainContainerWidget::newSplitter(orientation);
		if (l->count() > 0)
		{
			qWarning() << "Still items in layout. This should never happen.";
			QLayoutItem* li = l->takeAt(0);
			delete li;
		}
		l->addWidget(sp);
		sp->addWidget(sw);
	}
	else if (oldsp->orientation() == orientation
			|| oldsp->count() == 1)
	{
		oldsp->setOrientation(orientation);
		if (append)
			oldsp->addWidget(sw);
		else
			oldsp->insertWidget(0, sw);
	}
	else
	{
		QSplitter* sp = MainContainerWidget::newSplitter(orientation);
		if (append)
		{
			QLayoutItem* li = l->replaceWidget(oldsp, sp);
			sp->addWidget(oldsp);
			sp->addWidget(sw);
			delete li;
		}
		else
		{
			sp->addWidget(sw);
			QLayoutItem* li = l->replaceWidget(oldsp, sp);
			sp->addWidget(oldsp);
			delete li;
		}
	}
	return sw;
}

SectionWidget* CContainerWidget::insertNewSectionWidget(
    const InternalContentData& data, SectionWidget* targetSection, SectionWidget* ret,
    Qt::Orientation Orientation, int InsertIndexOffset)
{
	QSplitter* targetSectionSplitter = findParentSplitter(targetSection);
	SectionWidget* sw = newSectionWidget();
	sw->addContent(data, true);
	if (targetSectionSplitter->orientation() == Orientation)
	{
		const int index = targetSectionSplitter->indexOf(targetSection);
		targetSectionSplitter->insertWidget(index + InsertIndexOffset, sw);
	}
	else
	{
		const int index = targetSectionSplitter->indexOf(targetSection);
		QSplitter* s = MainContainerWidget::newSplitter(Orientation);
		s->addWidget(sw);
		s->addWidget(targetSection);
		targetSectionSplitter->insertWidget(index, s);
	}
	ret = sw;
	return ret;
}

SectionWidget* CContainerWidget::addSectionContent(const SectionContent::RefPtr& sc, SectionWidget* sw, DropArea area)
{
	ADS_Expects(!sc.isNull());

	// Drop it based on "area"
	InternalContentData data;
	data.content = sc;
	data.titleWidget = new SectionTitleWidget(sc, NULL);
	data.contentWidget = new SectionContentWidget(sc, NULL);

	//connect(data.titleWidget, SIGNAL(activeTabChanged()), this, SLOT(onActiveTabChanged()));
	return dropContent(data, sw, area, false);
}
} // namespace ads

//---------------------------------------------------------------------------
// EOF ContainerWidget.cpp
