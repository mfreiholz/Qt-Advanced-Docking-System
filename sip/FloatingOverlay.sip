%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CFloatingOverlay : QWidget, ads::IFloatingWidget
{

    %TypeHeaderCode
    #include <FloatingOverlay.h>
    %End


public:
    CFloatingOverlay(ads::CDockWidget* Content /TransferThis/ );
    CFloatingOverlay(ads::CDockAreaWidget* Content /TransferThis/ );

  	virtual ~CFloatingOverlay();

    virtual void startFloating(const QPoint& DragStartMousePos, const QSize& Size,
        ads::eDragState DragState, QWidget* MouseEventHandler);

    virtual void moveFloating();
};

};

%End