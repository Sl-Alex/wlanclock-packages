#include "MenuLocalStatus.h"
#include "PresHumTempController.h"
#include "PngStorage.h"
#include "Canvas.h"
#include "Config.h"
#include <iostream>

MenuLocalStatus::MenuLocalStatus(AbstractDisplayInterface &displayIface, IMenuInteraction *menuInterface)
    :mMenuInterface(menuInterface)
    ,mDisplayInterface(displayIface)
{
    mBgCanvas   = new Canvas<CANVAS_COLOR_4BIT>(Config::Display::WIDTH, Config::Display::HEIGHT);
    mFgCanvas   = new Canvas<CANVAS_COLOR_4BIT>(Config::Display::WIDTH, Config::Display::HEIGHT);
    drawCurrentStatus();
    mAnimator.setFgBgDest(*mFgCanvas, *mBgCanvas, *displayIface.getCanvas());
    mAnimator.setDir(Animator::ANIM_DIR_DOWN);
    mAnimator.setType(Animator::ANIM_TYPE_SLIDE);
    mAnimator.setSpeed(2);
    mAnimator.start();
    mAnimator.tick();
    mDisplayInterface.update();
    SysTimer::getInstance().subscribe(*this, Config::Display::UPDATE_INTERVAL, true);
    UbusServer::getInstance().subscribeGesture(*this);
}

void MenuLocalStatus::start()
{

}

static const char *getMonth(int mon)
{
    static const char *months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    if ((mon < 0)||(mon>11))
    {
        return "";
    }
    return months[mon];
}

void MenuLocalStatus::drawCurrentStatus()
{
    char tmp[100];
    float pres, hum, temp;
    PresHumTempController::getInstance().getPresHumTemp(pres, hum, temp);
    time_t t = time(NULL);
    tm nowTm = *localtime(&t);
    sprintf(tmp, "%2d:%02d     %2d %s", nowTm.tm_hour, nowTm.tm_min, nowTm.tm_mday, getMonth(nowTm.tm_mon));
    std::string tmp_str = tmp;
    std::wstring line1;
    std::wstring line2;
    std::wstring line3;
    line1.append(tmp_str.begin(), tmp_str.end());
    sprintf(tmp, "%2d", (int)temp);
    tmp_str = tmp;
    line2.append(tmp_str.begin(), tmp_str.end()).append(L"℃");
    sprintf(tmp, "   %d%%", (int)hum);
    tmp_str = tmp;
    line2.append(tmp_str.begin(), tmp_str.end());
    sprintf(tmp, "%dhPa", (int)pres);
    tmp_str = tmp;
    line3.append(tmp_str.begin(), tmp_str.end());

    mFgCanvas->clear();
    PngStorage::getInstance().getCanvas("house.png")->copyTo(*mFgCanvas, 3, 0);
    Config::Fonts::Params params;
    params = Config::Fonts::PARAMS[Config::Fonts::FONT_WEATHER];
    params.base_y = 10;
    mFgCanvas->drawText(params, line1, 0xFF500000);
    params.base_y = 20;
    mFgCanvas->drawText(params, line2, 0xFF700000);
    params.base_y = 30;
    mFgCanvas->drawText(params, line3, 0xC0A00000);
}

void MenuLocalStatus::onGesture(IGestureReceiver::Gesture gesture)
{
    switch(gesture)
    {
        case GESTURE_DOWN:
            return;
        default:
            break;
    }

    SysTimer::getInstance().unsubscribe(*this);
    UbusServer::getInstance().unsubscribeGesture();
    UbusServer::getInstance().unsubscribeRelPos();

    mAnimator.finish();

    delete mBgCanvas;
    delete mFgCanvas;

    /* This is the very last step, we will be destroyed here */
    if (mMenuInterface)
        mMenuInterface->onMenuAction(MENUACTION_EXIT, gesture);
}

void MenuLocalStatus::onMenuAction(IMenuInteraction::MenuAction action, IGestureReceiver::Gesture gesture)
{
}

void MenuLocalStatus::onTimer()
{
    if (mAnimator.isRunning())
    {
        mAnimator.tick();
        mDisplayInterface.update();
        if (!mAnimator.isRunning())
        {
            SysTimer::getInstance().subscribe(*this, Config::Display::MENU_TIMEOUT, false);
        }
    }
    else
    {
        SysTimer::getInstance().unsubscribe(*this);
        UbusServer::getInstance().unsubscribeGesture();

        delete mBgCanvas;
        delete mFgCanvas;

        /* This is the very last step, we will be destroyed here */
        if (mMenuInterface)
            mMenuInterface->onMenuAction(MENUACTION_EXIT, GESTURE_UP);
    }
}
