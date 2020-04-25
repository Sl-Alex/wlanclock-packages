#ifndef MENULOCALSTATUS_H
#define MENULOCALSTATUS_H

#include "AbstractCanvas.h"
#include "AbstractDisplayInterface.h"
#include "SysTimer.h"
#include "UbusServer.h"
#include "Fonts.h"
#include "Animator.h"
#include "Weather.h"
#include "Menu.h"
#include <mutex>

class MenuLocalStatus: public ISysTimer, public IGestureReceiver, public IMenuInteraction
{
    public:
        MenuLocalStatus(AbstractDisplayInterface &displayIface, IMenuInteraction *menuInterface = nullptr);
        virtual ~MenuLocalStatus(){};

    private:
        virtual void onTimer();
        virtual void onGesture(Gesture gesture);
        virtual void onMenuAction(MenuAction action);

        void drawCurrentStatus();

        IMenuInteraction *mMenuInterface;

        AbstractCanvas *mBgCanvas;
        AbstractCanvas *mFgCanvas;
        AbstractDisplayInterface & mDisplayInterface;
        Animator mAnimator;
        std::mutex mGestureMutex;
};

#endif /* MENULOCALSTATUS_H */
