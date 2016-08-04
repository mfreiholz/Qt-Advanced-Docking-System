#ifndef SECTIONCONTENT_H
#define SECTIONCONTENT_H

#include <QSharedPointer>
#include <QWeakPointer>
#include <QPointer>
class QWidget;

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

	enum Flag
	{
		None = 0,
		Closeable = 1,
		AllFlags = Closeable
	};
	Q_DECLARE_FLAGS(Flags, Flag)

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
	Flags flags() const;

	QString visibleTitle() const;
	QString title() const;
	void setTitle(const QString& title);
	void setFlags(const Flags f);

private:
	const int _uid;
	QString _uniqueName;

	QPointer<ContainerWidget> _containerWidget;
	QPointer<QWidget> _titleWidget;
	QPointer<QWidget> _contentWidget;

	// Optional attributes
	QString _title;
	Flags _flags;

	/* Note: This method could be a problem in static build environment
	 * since it may begin with 0 for every module which uses ADS.
	 */
	static int GetNextUid();
};

ADS_NAMESPACE_END
#endif
