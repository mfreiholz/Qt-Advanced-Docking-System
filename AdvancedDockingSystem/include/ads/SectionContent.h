#ifndef SECTIONCONTENT_H
#define SECTIONCONTENT_H

#include <QPointer>
#include <QWidget>
#include <QHash>
#include <QSharedPointer>
#include <QWeakPointer>

#include "ads/API.h"

ADS_NAMESPACE_BEGIN
class ContainerWidget;

class ADS_EXPORT_API SectionContent
{
	friend class ContainerWidget;

private:
	SectionContent();
	SectionContent(const SectionContent&);
	SectionContent& operator=(const SectionContent&);

public:
	typedef QSharedPointer<SectionContent> RefPtr;
	typedef QWeakPointer<SectionContent> WeakPtr;

	/*!
	 * Creates new content, associates it to <em>container</em> and takes ownership of
	 * <em>title</em>- and <em>content</em>- widgets.
	 * \param uniqueName An unique identifier across the entire process.
	 * \param container The parent ContainerWidget in which this content will be active.
	 * \param title The widget to use as title.
	 * \param content The widget to use as content.
	 * \return May return a invalid ref-pointer in case of invalid parameters.
	 */
	static RefPtr newSectionContent(const QString& uniqueName, ContainerWidget* container, QWidget* title, QWidget* content);

	virtual ~SectionContent();
	int uid() const;
	QString uniqueName() const;
	ContainerWidget* containerWidget() const;
	QWidget* titleWidget() const;
	QWidget* contentWidget() const;

	QString visibleTitle() const;
	QString title() const;
	void setTitle(const QString& title);

private:
	const int _uid;
	QString _uniqueName;

	ContainerWidget* _containerWidget;
	QPointer<QWidget> _titleWidget;
	QPointer<QWidget> _contentWidget;

	// Optional attributes
	QString _title;

	static int GetNextUid();
	static QHash<int, SectionContent::WeakPtr>& GetLookupMap();
	static QHash<QString, SectionContent::WeakPtr>& GetLookupMapByName();
};

ADS_NAMESPACE_END
#endif
