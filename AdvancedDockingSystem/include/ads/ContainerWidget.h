#ifndef ADS_CONTAINERWIDGET_H
#define ADS_CONTAINERWIDGET_H

#include <QPointer>
#include <QFrame>
#include <QGridLayout>
#include <QPoint>
#include <QList>
class QSplitter;
class QMenu;

#include "ads/API.h"
#include "ads/SectionContent.h"
#include "ads/SectionWidget.h"
#include "ads/FloatingWidget.h"

ADS_NAMESPACE_BEGIN

class InternalContentData;

// ContainerWidget is the main container to provide the docking
// functionality. It manages mulitple Sections and all possible areas.
class ContainerWidget : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)

	friend class SectionWidget;
	friend class FloatingWidget;

public:
	explicit ContainerWidget(QWidget *parent = NULL);

	//
	// Public API
	//

	Qt::Orientation orientation() const;
	void setOrientation(Qt::Orientation orientation);

	/*!
	 * Adds the section-content <em>sc</em> to this container-widget into the section-widget <em>sw</em>.
	 * If <em>sw</em> is not NULL, the <em>area</em> is used to indicate how the content should be arranged.
	 * Returns a pointer to the SectionWidget of the added SectionContent. Do not use it for anything else than adding more
	 * SectionContent elements with this method.
	 */
	SectionWidget* addSectionContent(const SectionContent::RefPtr& sc, SectionWidget* sw = NULL, DropArea area = CenterDropArea);

	/*!
	 * Creates a QMenu based on available SectionContents.
	 * The ownership is needs to be handled by the caller.
	 */
	QMenu* createContextMenu() const;

	/*!
	 * Serializes the current state of contents and returns it as a plain byte array.
	 * \see restoreState(const QByteArray&)
	 */
	QByteArray saveState() const;

	/*!
	 * Deserilizes the state of contents from <em>data</em>, which was written with <em>saveState()</em>.
	 * \see saveState()
	 */
	bool restoreState(const QByteArray& data);

	//
	// Internal Stuff Begins Here
	//

	// splitSections splits "section1" and "section2" with given "orientation".
	// The "section2" element is moved to the "section1" element.
	void splitSections(SectionWidget* section1, SectionWidget* section2, Qt::Orientation orientation = Qt::Horizontal);

	SectionWidget* dropContent(const InternalContentData& data, SectionWidget* targetSection, DropArea area, bool autoActive = true);
	void addSection(SectionWidget* section);
	SectionWidget* sectionAt(const QPoint& pos) const;

	// Drop areas for the ContainerWidget
	QRect outerTopDropRect() const;
	QRect outerRightDropRect() const;
	QRect outerBottomDropRect() const;
	QRect outerLeftDropRect() const;

private:
	SectionWidget* dropContentOuterHelper(QLayout* l, const InternalContentData& data, Qt::Orientation orientation, bool append);
	void saveGeometryWalk(QDataStream& out, QWidget* widget) const;
	bool restoreGeometryWalk(QDataStream& in, QSplitter* currentSplitter = NULL);

	// takeContent searches all section-widgets and floating-widgets for "sc" and takes
	// the ownership of it and passes it to "data" object.
	bool takeContent(const SectionContent::RefPtr& sc, InternalContentData& data);

private slots:
	void onActionToggleSectionContentVisibility(bool visible);

signals:
	void orientationChanged();

public:
	// Existing sections.
	// SectionWidgets are always visible.
	QList<SectionWidget*> _sections;

	// All currently active Floatingwidgets.
	QList<FloatingWidget*> _floatingWidgets;

	// Layout stuff
	QGridLayout* _mainLayout;
	Qt::Orientation _orientation;
	QPointer<QSplitter> _splitter;
};

ADS_NAMESPACE_END
#endif
