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

        int init(void);

    private:
        void drawDesktop();
        void onTimer();
        void onGesture(Gesture gesture);
        void onMenuAction(IMenuInteraction::MenuAction action);

        const std::string FONT_HOURS_MINS = "HOURS_MINS";
        const std::string FONT_SECONDS    = "SECONDS";

        AbstractCanvas *mBgCanvas;
        AbstractCanvas *mFgCanvas;
        AbstractDisplayInterface & mDisplayInterface;
        Fonts::FontParams mHourMinParams;
        Fonts::FontParams mSecParams;
        Animator mAnimator;
        Gesture mLastGesture;
        rgba32_t mColor;
        tm mTm;

        MenuForecast *mMenuForecast;
        MenuLocalStatus *mMenuLocalStatus;
        std::mutex mGestureMutex;
};

#endif /* DESKTOP_H */
