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

#include "ads/Internal.h"
#include "ads/SectionWidget.h"
#include "ads/SectionTitleWidget.h"
#include "ads/SectionContentWidget.h"
#include "ads/DropOverlay.h"
#include "ads/Serialization.h"

namespace ads
{
//============================================================================
CContainerWidget::CContainerWidget(QWidget *parent)
	: QFrame(parent)
{
	m_SectionDropOverlay = new DropOverlay(this, DropOverlay::ModeSectionOverlay);
	m_ContainerDropOverlay = new DropOverlay(this, DropOverlay::ModeContainerOverlay);
	m_ContainerDropOverlay->setAttribute(Qt::WA_TransparentForMouseEvents);
	m_ContainerDropOverlay->setWindowFlags(m_ContainerDropOverlay->windowFlags() | Qt::WindowTransparentForInput);

	m_MainLayout = new QGridLayout();
	m_MainLayout->setContentsMargins(0, 1, 0, 0);
	m_MainLayout->setSpacing(0);
	setLayout(m_MainLayout);
}


//============================================================================
CContainerWidget::~CContainerWidget()
{

}
} // namespace ads

//---------------------------------------------------------------------------
// EOF ContainerWidget.cpp
