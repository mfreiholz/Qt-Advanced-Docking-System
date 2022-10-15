#ifndef AD4D959A_4A16_4D35_97BB_499A32994FC4
#define AD4D959A_4A16_4D35_97BB_499A32994FC4
#ifndef SECTION_TITLE_WIDGET_H
#  define SECTION_TITLE_WIDGET_H

#  include <QFrame>
#  include <QPoint>
#  include <QPointer>

#  include "AdvancedDockingSystem/API.h"
#  include "AdvancedDockingSystem/SectionContent.h"

ADS_NAMESPACE_BEGIN
class ContainerWidget;
class SectionWidget;
class FloatingWidget;

class SectionTitleWidget : public QFrame
{
  Q_OBJECT
  Q_PROPERTY(bool activeTab READ isActiveTab WRITE setActiveTab NOTIFY
                 activeTabChanged)

  friend class ContainerWidget;
  friend class SectionWidget;

  SectionContent::RefPtr _content;

  // Drag & Drop (Floating)
  QPointer<FloatingWidget> _fw;
  QPoint _dragStartPos;

  // Drag & Drop (Title/Tabs)
  bool _tabMoving;

  // Property values
  bool _activeTab;

public:
  SectionTitleWidget(SectionContent::RefPtr content, QWidget* parent);
  virtual ~SectionTitleWidget();

  bool isActiveTab() const;
  void setActiveTab(bool active);

protected:
  virtual void mousePressEvent(QMouseEvent* ev);
  virtual void mouseReleaseEvent(QMouseEvent* ev);
  virtual void mouseMoveEvent(QMouseEvent* ev);

signals:
  void activeTabChanged();
  void clicked();
};

ADS_NAMESPACE_END
#endif

#endif /* AD4D959A_4A16_4D35_97BB_499A32994FC4 */
