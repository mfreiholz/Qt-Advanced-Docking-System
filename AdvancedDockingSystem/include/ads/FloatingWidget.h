#ifndef FLOATINGWIDGET_H
#define FLOATINGWIDGET_H

#include <QWidget>
#include <QFrame>
class QBoxLayout;

#include "ads/API.h"
#include "ads/SectionContent.h"

ADS_NAMESPACE_BEGIN
class MainContainerWidget;
class SectionTitleWidget;
class SectionContentWidget;
class InternalContentData;
class SectionWidget;
class CContainerWidget;
class FloatingWidget;

class CFloatingTitleWidget : public QFrame
{
	Q_OBJECT
private:
	QPoint m_DragStartPosition;
	QPoint m_DragStartMousePosition;
	FloatingWidget* floatingWidget() const;
	MainContainerWidget* mainContainerWidget() const;
	void moveFloatingWidget(QMouseEvent* ev);

private slots:
	void onMaximizeButtonClicked();

protected:
	virtual void mousePressEvent(QMouseEvent* ev);
	virtual void mouseReleaseEvent(QMouseEvent* ev);
	virtual void mouseMoveEvent(QMouseEvent* ev);

public:
	CFloatingTitleWidget(SectionContent::Flags Flags, FloatingWidget* Parent);

signals:
	void closeButtonClicked();
};


// FloatingWidget holds and displays SectionContent as a floating window.
// It can be resized, moved and dropped back into a SectionWidget.
class FloatingWidget : public QWidget
{
	Q_OBJECT

	friend class MainContainerWidget;
	friend class CFloatingTitleWidget;

public:
	FloatingWidget(MainContainerWidget* container, SectionContent::RefPtr sc, SectionTitleWidget* titleWidget, SectionContentWidget* contentWidget, QWidget* parent = NULL);
    FloatingWidget(SectionWidget* sectionWidget);
    virtual ~FloatingWidget();

	SectionContent::RefPtr content() const { return _content; }

	/**
	 * Returns the current zOrderIndex
	 */
	unsigned int zOrderIndex() const;

	CContainerWidget* containerWidget() const  {return m_ContainerWidget;}
	MainContainerWidget* mainContainerWidget() const {return m_MainContainerWidget;}

public://private:
	bool takeContent(InternalContentData& data);

	void startFloating(const QPoint& Pos);

protected:
	virtual void changeEvent(QEvent *event) override;
	virtual void moveEvent(QMoveEvent *event) override;
	virtual bool event(QEvent *e);
	void titleMouseReleaseEvent();
	virtual bool eventFilter(QObject *watched, QEvent *event) override;
	void updateDropOverlays(const QPoint& GlobalPos);

private slots:
	void onCloseButtonClicked();

private:
	void setDraggingActive(bool Active);

	MainContainerWidget* m_MainContainerWidget;
	SectionContent::RefPtr _content;
	SectionTitleWidget* _titleWidget;
	SectionContentWidget* _contentWidget;
	CContainerWidget* m_ContainerWidget;
	CContainerWidget* m_DropContainer;
	bool m_DraggingActive = false;
	unsigned int m_zOrderIndex = 0;
	QPoint m_DragStartPosition;
	QPoint m_DragStartMousePosition;
	static unsigned int zOrderCounter;
};

ADS_NAMESPACE_END
#endif
