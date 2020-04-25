#include <MenuForecast.h>
#include "PngStorage.h"
#include "Canvas.h"
#include "Config.h"
#include <iostream>

MenuForecast::MenuForecast(AbstractDisplayInterface &displayIface, IMenuInteraction *menuInterface)
    :mMenuInterface(menuInterface)
    ,mDisplayInterface(displayIface)
    ,mForecastItemIndex(0)
{
    UbusServer::getInstance().subscribeGesture(*this);
    SysTimer::getInstance().subscribe(*this, Config::Display::UPDATE_INTERVAL, true);
    mWeatherForecast = Weather::getInstance().getWeatherForecast();
    mBgCanvas   = new Canvas<CANVAS_COLOR_4BIT>(Config::Display::WIDTH, Config::Display::HEIGHT);
    mFgCanvas   = new Canvas<CANVAS_COLOR_4BIT>(Config::Display::WIDTH, Config::Display::HEIGHT);
    if (mWeatherForecast.size())
    {
        drawForecastItem();
    }
    else
    {
        drawRefreshItem();
    }
    mDisplayInterface.update();
    mAnimator.setFgBgDest(*mFgCanvas, *mBgCanvas, *displayIface.getCanvas());
    mAnimator.setDir(Animator::ANIM_DIR_UP);
    mAnimator.setType(Animator::ANIM_TYPE_SLIDE);
    mAnimator.setSpeed(2);
    mAnimator.start();
    mAnimator.tick();
}

void MenuForecast::drawRefreshItem()
{
    std::string line1 = "not yet available";
    std::string line2 = " please try later";

    std::string tmp_str = "/usr/share/wlanclock/images/refresh.png";
    mFgCanvas->clear();
    PngStorage::getInstance().getCanvas(tmp_str.c_str())->copyTo(*mFgCanvas, 3, 0);
    Config::Fonts::Params params;
    params = Config::Fonts::PARAMS[Config::Fonts::FONT_WEATHER];
    params.base_y = 13;
    mFgCanvas->drawText(params, line1, 0xFF800000);
    params.base_y = 26;
    mFgCanvas->drawText(params, line2, 0xFF800000);
}

void MenuForecast::drawForecastItem()
{
    char tmp[20];
    time_t time = mWeatherForecast[mForecastItemIndex].timestamp;
    tm t = *localtime(&time);
    sprintf(tmp, "%2d:%02d   %dhPa", t.tm_hour, t.tm_min, (int)mWeatherForecast[mForecastItemIndex].pressure);
    std::string tmp_str = tmp;
    std::wstring line1;
    std::wstring line2;
    std::wstring line3;
    line1.append(tmp_str.begin(), tmp_str.end());
    sprintf(tmp, "%2d", (int)mWeatherForecast[mForecastItemIndex].temperature);
    tmp_str = tmp;
    line2.append(tmp_str.begin(), tmp_str.end()).append(L"℃");
    sprintf(tmp, " %d%% %dm/s", (int)mWeatherForecast[mForecastItemIndex].humidity, (int)mWeatherForecast[mForecastItemIndex].wind_speed);
    tmp_str = tmp;
    line2.append(tmp_str.begin(), tmp_str.end());
    /* TODO: Show rain/snow volume or something else */
    line3.append(L"one more string!");

    tmp_str = "/usr/share/wlanclock/images/weather/";
    tmp_str.append(mWeatherForecast[mForecastItemIndex].weatherIcon);
    tmp_str.append(".png");
    mFgCanvas->clear();
    PngStorage::getInstance().getCanvas(tmp_str.c_str())->copyTo(*mFgCanvas, 3, 0);
    Config::Fonts::Params params;
    params = Config::Fonts::PARAMS[Config::Fonts::FONT_WEATHER];
    params.base_y = 10;
    mFgCanvas->drawText(params, line1, 0xFF500000);
    params.base_y = 20;
    mFgCanvas->drawText(params, line2, 0xFF700000);
    params.base_y = 30;
    mFgCanvas->drawText(params, line3, 0xC0A00000);
}

void MenuForecast::onGesture(IGestureReceiver::Gesture gesture)
{
    std::lock_guard<std::mutex> lock(mGestureMutex);
    mAnimator.finish();
    switch(gesture)
    {
        case GESTURE_UP:
            if (!mWeatherForecast.size())
            {
                return;
            }
            mForecastItemIndex++;
            if (mForecastItemIndex >= (int)mWeatherForecast.size())
            {
                mForecastItemIndex = mWeatherForecast.size() -1;
                return;
            }
            drawForecastItem();
            mAnimator.setDir(Animator::ANIM_DIR_UP);
            mAnimator.setType(Animator::ANIM_TYPE_SHIFT);
            mAnimator.start();
            mAnimator.tick();
            return;
        case GESTURE_DOWN:
            if (mForecastItemIndex > 0)
            {
                mForecastItemIndex--;
                drawForecastItem();
                mAnimator.setDir(Animator::ANIM_DIR_DOWN);
                mAnimator.setType(Animator::ANIM_TYPE_SHIFT);
                mAnimator.start();
                mAnimator.tick();
                return;
            }
            break;
        default:
            return;
    }

    SysTimer::getInstance().unsubscribe(*this);
    UbusServer::getInstance().unsubscribeGesture();

    delete mBgCanvas;
    delete mFgCanvas;

    /* This is the very last step, we will be destroyed here */
    if (mMenuInterface)
        mMenuInterface->onMenuAction(MENUACTION_EXIT);
}

void MenuForecast::onMenuAction(IMenuInteraction::MenuAction action)
{
}

void MenuForecast::onTimer()
{
    std::lock_guard<std::mutex> lock(mGestureMutex);
    if (mAnimator.isRunning())
    {
        mAnimator.tick();
        mDisplayInterface.update();
    }
}
