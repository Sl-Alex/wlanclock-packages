#ifndef DEKSTOP_H
#define DEKSTOP_H

#include "MenuForecast.h"
#include "MenuLocalStatus.h"
#include "AbstractCanvas.h"
#include "AbstractDisplayInterface.h"
#include "SysTimer.h"
#include "UbusServer.h"
#include "Fonts.h"
#include "Animator.h"
#include <mutex>

class Desktop: public ISysTimer, public IGestureReceiver, public IMenuInteraction
{
    public:
        Desktop(AbstractDisplayInterface &displayIface);
        ~Desktop();

    private:
        void drawDesktop();
        void onTimer();
        void onGesture(Gesture gesture);
        void onMenuAction(IMenuInteraction::MenuAction action);

        AbstractCanvas *mBgCanvas;
        AbstractCanvas *mFgCanvas;
        AbstractDisplayInterface & mDisplayInterface;
        Animator mAnimator;
        Gesture mLastGesture;
        rgba32_t mColor;
        tm mTm;

        MenuForecast *mMenuForecast;
        MenuLocalStatus *mMenuLocalStatus;
        std::mutex mGestureMutex;
};

#endif /* DESKTOP_H */
