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


/*!
 * ContainerWidget is the main container to provide the docking
 * functionality. It manages mulitple Sections and all possible areas.
 */
class ContainerWidget : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)

	friend class SectionWidget;
	friend class FloatingWidget;
	friend class SectionTitleWidget;
	friend class SectionContentWidget;

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
	 * The caller is responsible to delete the menu.
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
	// Advanced Public API
	// You usually should not need access to this methods
	//

	// Outer DropAreas
	QRect outerTopDropRect() const;
	QRect outerRightDropRect() const;
	QRect outerBottomDropRect() const;
	QRect outerLeftDropRect() const;

private:
	//
	// Internal Stuff Begins Here
	//

	SectionWidget* dropContent(const InternalContentData& data, SectionWidget* targetSection, DropArea area, bool autoActive = true);
	void addSection(SectionWidget* section);
	SectionWidget* sectionAt(const QPoint& pos) const;
	SectionWidget* dropContentOuterHelper(QLayout* l, const InternalContentData& data, Qt::Orientation orientation, bool append);
	void saveGeometryWalk(QDataStream& out, QWidget* widget) const;
	bool restoreGeometryWalk(QDataStream& in, QSplitter* currentSplitter = NULL);
	bool takeContent(const SectionContent::RefPtr& sc, InternalContentData& data);

private slots:
	void onActionToggleSectionContentVisibility(bool visible);

signals:
	void orientationChanged();

private:
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
