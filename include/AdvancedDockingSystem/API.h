#ifndef B1FD4E74_EA2A_463A_BE0F_4D1094DABD62
#define B1FD4E74_EA2A_463A_BE0F_4D1094DABD62
#ifndef ADS_API_H
#  define ADS_API_H

#  include <QFlags>

#  include "AdvancedDockingSystem/AdvancedDockingSystem_export.hpp"
class QWidget;
class QSplitter;

// DLL Export API
#  define ADS_EXPORT_API ADVANCEDDOCKINGSYSTEM_EXPORT

// Use namespace
// Disabled with Qt4, it makes problems with signals and slots.
#  ifdef ADS_NAMESPACE_ENABLED
#    define ADS_NAMESPACE_BEGIN \
      namespace ads \
      {
#    define ADS_NAMESPACE_END }
#    define ADS_NS ::ads
#  else
#    define ADS_NAMESPACE_BEGIN
#    define ADS_NAMESPACE_END
#    define ADS_NS
#  endif

// Always enable "serialization" namespace.
// It is not required for signals and slots.
#  define ADS_NAMESPACE_SER_BEGIN \
    namespace ads \
    { \
    namespace serialization \
    {
#  define ADS_NAMESPACE_SER_END \
    } \
    }
#  define ADS_NS_SER ::ads::serialization

// Width of the native window frame border (based on OS).
#  define ADS_WINDOW_FRAME_BORDER_WIDTH 7

// Beautiful C++ stuff.
#  define ADS_Expects(cond)
#  define ADS_Ensures(cond)

// Indicates whether ADS should include animations.
// #define ADS_ANIMATIONS_ENABLED 1
// #define ADS_ANIMATION_DURATION 150

ADS_NAMESPACE_BEGIN
class ContainerWidget;
class SectionWidget;

enum DropArea
{
  InvalidDropArea = 0,
  TopDropArea = 1,
  RightDropArea = 2,
  BottomDropArea = 4,
  LeftDropArea = 8,
  CenterDropArea = 16,

  AllAreas = TopDropArea | RightDropArea | BottomDropArea | LeftDropArea
      | CenterDropArea
};
Q_DECLARE_FLAGS(DropAreas, DropArea)

void deleteEmptySplitter(ContainerWidget* container);
ContainerWidget* findParentContainerWidget(QWidget* w);
SectionWidget* findParentSectionWidget(QWidget* w);
QSplitter* findParentSplitter(QWidget* w);
QSplitter* findImmediateSplitter(QWidget* w);

ADS_NAMESPACE_END
#endif

#endif /* B1FD4E74_EA2A_463A_BE0F_4D1094DABD62 */
