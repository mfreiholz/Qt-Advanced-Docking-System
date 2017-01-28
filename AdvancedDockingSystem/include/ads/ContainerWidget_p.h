#ifndef ContainerWidget_pH
#define ContainerWidget_pH
//============================================================================
/// \file   ContainerWidget_p.h
/// \author Uwe Kindler
/// \date   21.01.2017
/// \brief  Declaration of 
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QList>
#include <QHash>
#include <QPointer>
#include <QGridLayout>
#include <QSplitter>

#include "ads/DropOverlay.h"

namespace ads
{

class SectionWidget;
class FloatingWidget;
class HiddenSectionItem;


/**
 * Container widget private data
 */
struct ContainerWidgetPrivate
{
	// Elements inside container.
	QList<SectionWidget*> sections;
	QList<FloatingWidget*> floatings;
	QHash<int, HiddenSectionItem> hiddenSectionContents;


	// Helper lookup maps, restricted to this container.
	QHash<int, SectionContent::WeakPtr> SectionContentIdMap;
	QHash<QString, SectionContent::WeakPtr> SectionContentNameMap;
	QHash<int, SectionWidget*> SectionWidgetIdMap;


	// Layout stuff
	QGridLayout* MainLayout = nullptr;
	Qt::Orientation orientation = Qt::Horizontal;
	QPointer<QSplitter> splitter; // $mfreiholz: I'd like to remove this variable entirely,
								   // because it changes during user interaction anyway.

	// Drop overlay stuff.
	QPointer<DropOverlay> SectionDropOverlay;
	QPointer<DropOverlay> ContainerDropOverlay;
	//QWidget* LeftBorderDropArea;
}; // struct ContainerWidgetPrivate
} // namespace ads

//---------------------------------------------------------------------------
#endif // ContainerWidget_pH
