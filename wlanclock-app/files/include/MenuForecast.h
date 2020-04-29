#ifndef MENUFORECAST_H
#define MENUFORECAST_H

#include "AbstractCanvas.h"
#include "AbstractDisplayInterface.h"
#include "SysTimer.h"
#include "UbusServer.h"
#include "Animator.h"
#include "Weather.h"
#include "Menu.h"

class MenuForecast: public ISysTimer, public IGestureReceiver, public IMenuInteraction
{
    public:
        MenuForecast(AbstractDisplayInterface &displayIface, IMenuInteraction *menuInterface = nullptr);
        virtual ~MenuForecast(){};

    private:
        virtual void onTimer();
        void drawRefreshItem();
        void drawForecastItem();
        virtual void onGesture(Gesture gesture);
        virtual void onMenuAction(IMenuInteraction::MenuAction action, IGestureReceiver::Gesture gesture);

        IMenuInteraction *mMenuInterface;

        AbstractCanvas *mBgCanvas;
        AbstractCanvas *mFgCanvas;
        AbstractDisplayInterface & mDisplayInterface;
        Animator mAnimator;
        std::vector<Weather::WeatherInfo> mWeatherForecast;
        int mForecastItemIndex;
};

#endif /* MENUFORECAST_H */
