#ifndef PushButtonH
#define PushButtonH
//============================================================================
/// \file   PushButton.h
/// \author Uwe Kindler
/// \date   18.10.2022
/// \brief  Declaration of CPushButton
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QPushButton>

namespace ads
{

/**
 * ADS specific push button class
 */
class CPushButton : public QPushButton
{
    Q_OBJECT
public:
    enum Orientation {
        Horizontal,
        VerticalTopToBottom,
        VerticalBottomToTop
    };

    using QPushButton::QPushButton;

    virtual QSize sizeHint() const override;

    Orientation orientation() const;
    void setOrientation(Orientation &orientation);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    Orientation m_Orientation = Horizontal;
};

} // namespace ads

//---------------------------------------------------------------------------
#endif // PushButtonH

