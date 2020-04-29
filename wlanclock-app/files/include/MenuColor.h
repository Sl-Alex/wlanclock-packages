#ifndef MENUSETTINGS_H
#define MENUSETTINGS_H

#include "AbstractCanvas.h"
#include "AbstractDisplayInterface.h"
#include "SysTimer.h"
#include "UbusServer.h"
#include "Animator.h"
#include "Menu.h"

class MenuColor: public ISysTimer, public IGestureReceiver, public IRelPosReceiver, public IMenuInteraction
{
    public:
        MenuColor(AbstractDisplayInterface &displayIface, IMenuInteraction *menuInterface = nullptr);
        virtual ~MenuColor(){};

        void drawMenu(void);

    private:
        virtual void onTimer();
        virtual void onGesture(Gesture gesture);
        virtual void onRelPos(int rel_x, int rel_y);
        virtual void onMenuAction(IMenuInteraction::MenuAction action, IGestureReceiver::Gesture gesture);

        IMenuInteraction *mMenuInterface;

        AbstractCanvas *mBgCanvas;
        AbstractCanvas *mFgCanvas;
        AbstractDisplayInterface & mDisplayInterface;
        Animator mAnimator;

        int32_t mColorR;
        int32_t mColorG;
        int32_t mColorB;
        int32_t mSavedR;
        int32_t mSavedG;
        int32_t mSavedB;

        int mSelectedColor = 0;
        rgba32_t mPrevColor;
};

#endif /* MENUSETTINGS_H */
