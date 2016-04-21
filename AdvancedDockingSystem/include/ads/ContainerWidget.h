#ifndef ADS_CONTAINERWIDGET_H
#define ADS_CONTAINERWIDGET_H

#include <QList>
#include <QHash>
#include <QPointer>
#include <QFrame>
class QPoint;
class QSplitter;
class QMenu;
class QGridLayout;

#include "ads/API.h"
#include "ads/Internal.h"
#include "ads/SectionContent.h"
#include "ads/FloatingWidget.h"
#include "ads/Serialization.h"

ADS_NAMESPACE_BEGIN
class SectionWidget;
class DropOverlay;
class InternalContentData;


/*!
 * ContainerWidget is the main container to provide the docking
 * functionality. It manages multiple sections with all possible areas.
 */
class ADS_EXPORT_API ContainerWidget : public QFrame
{
	Q_OBJECT

	friend class SectionContent;
	friend class SectionWidget;
	friend class FloatingWidget;
	friend class SectionTitleWidget;
	friend class SectionContentWidget;

public:
	explicit ContainerWidget(QWidget *parent = NULL);
	virtual ~ContainerWidget();

	//
	// Public API
	//

	/*!
	 * Adds the section-content <em>sc</em> to this container-widget into the section-widget <em>sw</em>.
	 * If <em>sw</em> is not NULL, the <em>area</em> is used to indicate how the content should be arranged.
	 * Returns a pointer to the SectionWidget of the added SectionContent. Do not use it for anything else than adding more
	 * SectionContent elements with this method.
	 */
	SectionWidget* addSectionContent(const SectionContent::RefPtr& sc, SectionWidget* sw = NULL, DropArea area = CenterDropArea);

	/*!
	 * Completely removes the <em>sc</em> from this ContainerWidget.
	 * This container will no longer hold a reference to the content.
	 * The content can be safely deleted.
	 */
	bool removeSectionContent(const SectionContent::RefPtr& sc);

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
	 * Indicates whether the SectionContent <em>sc</em> is visible.
	 */
	bool isSectionContentVisible(const SectionContent::RefPtr& sc);

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

	/*!
	 * \brief contents
	 * \return List of known SectionContent for this ContainerWidget.
	 */
	QList<SectionContent::RefPtr> contents() const;

	QPointer<DropOverlay> dropOverlay() const;

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
	QByteArray saveHierarchy() const;
	void saveFloatingWidgets(QDataStream& out) const;
	void saveSectionWidgets(QDataStream& out, QWidget* widget) const;

	bool saveSectionIndex(ADS_NS_SER::SectionIndexData &sid) const;

	bool restoreHierarchy(const QByteArray& data);
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
	 * for the old tab and the new active tab (the order is unspecified).
	 */
	void activeTabChanged(const SectionContent::RefPtr& sc, bool active);

	/*!
	 * Emits whenever the visibility of a SectionContent changes.
	 * \see showSectionContent(), hideSectionContent()
	 * \since 0.2
	 */
	void sectionContentVisibilityChanged(const SectionContent::RefPtr& sc, bool visible);

private:
	// Elements inside container.
	QList<SectionWidget*> _sections;
	QList<FloatingWidget*> _floatings;
	QHash<int, HiddenSectionItem> _hiddenSectionContents;


	// Helper lookup maps, restricted to this container.
	QHash<int, SectionContent::WeakPtr> _scLookupMapById;
	QHash<QString, SectionContent::WeakPtr> _scLookupMapByName;
	QHash<int, SectionWidget*> _swLookupMapById;


	// Layout stuff
	QGridLayout* _mainLayout;
	Qt::Orientation _orientation;
	QPointer<QSplitter> _splitter; // $mfreiholz: I'd like to remove this variable entirely,
								   // because it changes during user interaction anyway.

	// Drop overlay stuff.
	QPointer<DropOverlay> _dropOverlay;
};

ADS_NAMESPACE_END
#endif
