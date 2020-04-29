#ifndef MENULOCALSTATUS_H
#define MENULOCALSTATUS_H

#include "AbstractCanvas.h"
#include "AbstractDisplayInterface.h"
#include "SysTimer.h"
#include "UbusServer.h"
#include "Animator.h"
#include "Menu.h"

class MenuLocalStatus: public ISysTimer, public IGestureReceiver, public IMenuInteraction
{
    public:
        MenuLocalStatus(AbstractDisplayInterface &displayIface, IMenuInteraction *menuInterface = nullptr);
        virtual ~MenuLocalStatus(){};

        void start();

    private:
        virtual void onTimer();
        virtual void onGesture(Gesture gesture);
        virtual void onMenuAction(IMenuInteraction::MenuAction action, IGestureReceiver::Gesture gesture);

        void drawCurrentStatus();

        IMenuInteraction *mMenuInterface;

        AbstractCanvas *mBgCanvas;
        AbstractCanvas *mFgCanvas;
        AbstractDisplayInterface & mDisplayInterface;
        Animator mAnimator;
};

#endif /* MENULOCALSTATUS_H */
