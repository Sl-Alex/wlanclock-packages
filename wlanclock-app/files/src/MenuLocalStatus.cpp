#include "MenuLocalStatus.h"
#include "PresHumTempController.h"
#include "PngStorage.h"
#include "Canvas.h"
#include "main.h"
#include <iostream>

MenuLocalStatus::MenuLocalStatus(AbstractDisplayInterface &displayIface, IMenuInteraction *menuInterface)
    :mMenuInterface(menuInterface)
    ,mDisplayInterface(displayIface)
{
    UbusServer::getInstance().subscribeGesture(*this);
    SysTimer::getInstance().subscribe(*this, Config::Display::UPDATE_INTERVAL, true);
    mBgCanvas   = new Canvas<CANVAS_COLOR_4BIT>(Config::Display::WIDTH, Config::Display::HEIGHT);
    mFgCanvas   = new Canvas<CANVAS_COLOR_4BIT>(Config::Display::WIDTH, Config::Display::HEIGHT);
    drawCurrentStatus();
    mDisplayInterface.update();
    mAnimator.setFgBgDest(*mFgCanvas, *mBgCanvas, *displayIface.getCanvas());
    mAnimator.setDir(Animator::ANIM_DIR_DOWN);
    mAnimator.setType(Animator::ANIM_TYPE_SLIDE);
    mAnimator.setSpeed(2);
    mAnimator.start();
    mAnimator.tick();
}

void MenuLocalStatus::drawCurrentStatus()
{
    char tmp[20];
    float pres, hum, temp;
    PresHumTempController::getInstance().getPresHumTemp(pres, hum, temp);
    time_t t = time(NULL);
    tm nowTm = *localtime(&t);
    sprintf(tmp, "%2d:%02d   %dhPa", nowTm.tm_hour, nowTm.tm_min, (int)pres);
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
    /* TODO: Show rain/snow volume or something else */
    line3.append(L"987654321");

    mFgCanvas->clear();
    PngStorage::getInstance().getCanvas("/usr/share/wlanclock/images/house.png")->copyTo(*mFgCanvas, 3, 0);
    mFgCanvas->drawText(
            36, // mSecParams.base_x,
            10, // mSecParams.base_y,
            0,  // mSecParams.index,
            12, // mSecParams.size_h,
            12, //mSecParams.size_v,
            line1, 0xFF500000);
    mFgCanvas->drawText(
            36, // mSecParams.base_x,
            20, // mSecParams.base_y,
            0,  // mSecParams.index,
            12, // mSecParams.size_h,
            12, //mSecParams.size_v,
            line2, 0xFF700000);
    mFgCanvas->drawText(
            36, // mSecParams.base_x,
            30, // mSecParams.base_y,
            0,  // mSecParams.index,
            12, // mSecParams.size_h,
            12, //mSecParams.size_v,
            line3, 0xC0A00000);
}

void MenuLocalStatus::onGesture(IGestureReceiver::Gesture gesture)
{
    std::lock_guard<std::mutex> lock(mGestureMutex);
    mAnimator.finish();
    switch(gesture)
    {
        case GESTURE_UP:
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

void MenuLocalStatus::onMenuAction(IMenuInteraction::MenuAction action)
{
}

void MenuLocalStatus::onTimer()
{
    std::lock_guard<std::mutex> lock(mGestureMutex);
    if (mAnimator.isRunning())
    {
        mAnimator.tick();
        mDisplayInterface.update();
    }
}
