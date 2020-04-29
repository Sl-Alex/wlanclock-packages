#ifndef DEKSTOP_H
#define DEKSTOP_H

#include "MenuColor.h"
#include "MenuForecast.h"
#include "MenuLocalStatus.h"
#include "AbstractCanvas.h"
#include "AbstractDisplayInterface.h"
#include "SysTimer.h"
#include "UbusServer.h"
#include "Fonts.h"
#include "Animator.h"

class Desktop: public ISysTimer, public IGestureReceiver, public IMenuInteraction
{
    public:
        Desktop(AbstractDisplayInterface &displayIface);
        ~Desktop();

    private:
        void drawDesktop();
        void onTimer();
        void onGesture(Gesture gesture);
        virtual void onMenuAction(IMenuInteraction::MenuAction action, IGestureReceiver::Gesture gesture);

        AbstractCanvas *mBgCanvas;
        AbstractCanvas *mFgCanvas;
        AbstractDisplayInterface & mDisplayInterface;
        Animator mAnimator;
        tm mTm;

        MenuForecast    *mMenuForecast;
        MenuLocalStatus *mMenuLocalStatus;
        MenuColor    *mMenuSettings;
};

#endif /* DESKTOP_H */
