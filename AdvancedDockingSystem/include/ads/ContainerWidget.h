#ifndef ContainerWidgetH
#define ContainerWidgetH
//============================================================================
/// \file   ContainerWidget.h
/// \author Uwe Kindler
/// \date   03.02.2017
/// \brief  Declaration of CContainerWidget
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>

#include "ads/API.h"
#include "ads/Internal.h"
#include "ads/SectionContent.h"
#include "ads/FloatingWidget.h"
#include "ads/Serialization.h"
#include "ads/DropOverlay.h"

namespace ads
{
class SectionWidget;
class DropOverlay;
class InternalContentData;

/**
 * @brief
 */
class CContainerWidget  : public QFrame
{
	Q_OBJECT

	friend class SectionContent;
	friend class SectionWidget;
	friend class FloatingWidget;
	friend class SectionTitleWidget;
    friend class ContainerWidgetPrivate;

public:
	explicit CContainerWidget(QWidget *parent = nullptr);
	virtual ~CContainerWidget();

protected:
	// Layout stuff
	QGridLayout* m_MainLayout = nullptr;
	Qt::Orientation m_Orientation = Qt::Horizontal;
	QPointer<QSplitter> m_Splitter; // $mfreiholz: I'd like to remove this variable entirely,
								   // because it changes during user interaction anyway.

	// Drop overlay stuff.
	QPointer<DropOverlay> m_SectionDropOverlay;
	QPointer<DropOverlay> m_ContainerDropOverlay;
};

} // namespace ads

//---------------------------------------------------------------------------
#endif // ContainerWidgetH
