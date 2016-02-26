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
#include "ads/Internal.h"
#include "ads/SectionContent.h"
#include "ads/SectionWidget.h"
#include "ads/FloatingWidget.h"

ADS_NAMESPACE_BEGIN
class InternalContentData;


/*!
 * ContainerWidget is the main container to provide the docking
 * functionality. It manages mulitple Sections and all possible areas.
 */
class ADS_EXPORT_API ContainerWidget : public QFrame
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
	 * Shows the specific SectionContent in UI.
	 * Independed of the current state, whether it is used inside a section or is floating.
	 */
	bool showSectionContent(const SectionContent::RefPtr& sc);

	/*!
	 * Closes the specified SectionContent from UI.
	 * Independed of the current state, whether it is used inside a section or is floating.
	 */
	bool hideSectionContent(const SectionContent::RefPtr& sc);

	/*!
	 * Selects the specific SectionContent as current, if it is part of a SectionWidget.
	 * If SC is floating, it does nothing (or should we show it?)
	 */
	bool raiseSectionContent(const SectionContent::RefPtr& sc);

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

	SectionWidget* newSectionWidget();
	SectionWidget* dropContent(const InternalContentData& data, SectionWidget* targetSection, DropArea area, bool autoActive = true);
	void addSection(SectionWidget* section);
	SectionWidget* sectionAt(const QPoint& pos) const;
	SectionWidget* dropContentOuterHelper(QLayout* l, const InternalContentData& data, Qt::Orientation orientation, bool append);

	// Serialization
	void saveFloatingWidgets(QDataStream& out) const;
	void saveSectionWidgets(QDataStream& out, QWidget* widget) const;
	bool restoreFloatingWidgets(QDataStream& in, int version, QList<FloatingWidget*>& floatings);
	bool restoreSectionWidgets(QDataStream& in, int version, QSplitter* currentSplitter, QList<SectionWidget*>& sections, QList<SectionContent::RefPtr>& contentsToHide);

	bool takeContent(const SectionContent::RefPtr& sc, InternalContentData& data);

private slots:
	void onActiveTabChanged();
	void onActionToggleSectionContentVisibility(bool visible);

signals:
	void orientationChanged();

	/*!
	 * Emits whenever the "isActiveTab" state of a SectionContent changes.
	 * Whenever the users sets another tab as active, this signal gets invoked
	 * for the old now inactive tab and the new active tab (the order is unspecified).
	 */
	void activeTabChanged(const SectionContent::RefPtr& sc, bool active);

private:
	QList<SectionWidget*> _sections;
	QList<FloatingWidget*> _floatings;

	QHash<int, HiddenSectionItem> _hiddenSectionContents;


	// Layout stuff
	QGridLayout* _mainLayout;
	Qt::Orientation _orientation;
	QPointer<QSplitter> _splitter; // $mfreiholz: I'd like to remove this variable entirely,
								   // because it changes during user interaction anyway.
};

ADS_NAMESPACE_END
#endif
