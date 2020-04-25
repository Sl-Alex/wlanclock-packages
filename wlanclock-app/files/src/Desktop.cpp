#include "Desktop.h"
#include "Canvas.h"
#include "Config.h"
#include <iostream>

Desktop::Desktop(AbstractDisplayInterface &displayIface)
    : mDisplayInterface(displayIface)
    , mMenuForecast(nullptr)
    , mMenuLocalStatus(nullptr)
{
    SysTimer::getInstance().subscribe(*this, Config::Display::UPDATE_INTERVAL, true);
    mBgCanvas   = new Canvas<CANVAS_COLOR_4BIT>(Config::Display::WIDTH, Config::Display::HEIGHT);
    mFgCanvas   = new Canvas<CANVAS_COLOR_4BIT>(Config::Display::WIDTH, Config::Display::HEIGHT);
    UbusServer::getInstance().subscribeGesture(*this);
}

Desktop::~Desktop()
{
    UbusServer::getInstance().unsubscribeGesture();
    delete mBgCanvas;
    delete mFgCanvas;
}

void Desktop::drawDesktop()
{
    char buff[100];
    snprintf(buff, sizeof(buff), "%02d:%02d", mTm.tm_hour, mTm.tm_min);
    mFgCanvas->clear();
    mFgCanvas->drawText(Config::Fonts::PARAMS[Config::Fonts::FONT_HOURS_MINS], std::string(buff), mColor);
    snprintf(buff, sizeof(buff), "%02d", mTm.tm_sec);
    mFgCanvas->drawText(Config::Fonts::PARAMS[Config::Fonts::FONT_SECONDS], std::string(buff), mColor);
}

void Desktop::onTimer()
{
    std::lock_guard<std::mutex> lock(mGestureMutex);
    static uint8_t r = 255;
    static uint8_t g = 0;
    static uint8_t b = 0;
    static uint8_t step = 0;
    time_t t = time(NULL);
    tm newTm = *localtime(&t);
    if (!mAnimator.isRunning() && memcmp(&newTm, &mTm, sizeof(mTm)))
    {
        memcpy(&mTm, &newTm, sizeof(mTm));
        mColor = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8);
        drawDesktop();
        mAnimator.setFgBgDest(*mFgCanvas, *mBgCanvas, *mDisplayInterface.getCanvas());
        mAnimator.setDir(Animator::ANIM_DIR_DOWN);
        mAnimator.setSpeed(10);
        mAnimator.setType(Animator::ANIM_TYPE_OPACITY);
        mAnimator.start();
    }
    if (mAnimator.isRunning())
    {
        mAnimator.tick();
        mDisplayInterface.update();
    }
    switch(step)
    {
    case 0:
        if (g!=255) g++;
        if (g==255) step = 1;
        break;
    case 1:
        if (r!=0) r--;
        if (r==0) step = 2;
        break;
    case 2:
        if (b!=255) b++;
        if (b==255) step = 3;
        break;
    case 3:
        if (g!=0) g--;
        if (g==0) step = 4;
        break;
    case 4:
        if (r!=255) r++;
        if (r==255) step = 5;
        break;
    case 5:
        if (b!=0) b--;
        if (b==0) step = 0;
        break;
    }
}

void Desktop::onGesture(Gesture gesture)
{
    std::lock_guard<std::mutex> lock(mGestureMutex);
    mLastGesture = gesture;
    switch(gesture)
    {
        case GESTURE_UP:
            mAnimator.finish();
            SysTimer::getInstance().unsubscribe(*this);
            UbusServer::getInstance().unsubscribeGesture();
            mMenuForecast = new MenuForecast(mDisplayInterface, this);
            break;
        case GESTURE_DOWN:
            mAnimator.finish();
            SysTimer::getInstance().unsubscribe(*this);
            UbusServer::getInstance().unsubscribeGesture();
            mMenuLocalStatus = new MenuLocalStatus(mDisplayInterface, this);
            break;
        default:
            break;
    }
}

void Desktop::onMenuAction(IMenuInteraction::MenuAction action)
{
    if (action == MENUACTION_EXIT)
    {
        /* Re-subscribe to what we need */
        SysTimer::getInstance().subscribe(*this, Config::Display::UPDATE_INTERVAL, true);
        UbusServer::getInstance().subscribeGesture(*this);
        /* Get current time */
        time_t t = time(NULL);
        mTm = *localtime(&t);
        /* Draw it */
        drawDesktop();
        /* Set common animation parameters */
        mAnimator.setFgBgDest(*mFgCanvas, *mBgCanvas, *mDisplayInterface.getCanvas());
        mAnimator.setType(Animator::ANIM_TYPE_SLIDE_BG);
        mAnimator.setSpeed(2);
        switch (mLastGesture)
        {
            case GESTURE_UP:
                delete mMenuForecast;
                mAnimator.setDir(Animator::ANIM_DIR_DOWN);
                break;
            case GESTURE_DOWN:
                delete mMenuLocalStatus;
                mAnimator.setDir(Animator::ANIM_DIR_UP);
                break;
            case GESTURE_LEFT:
                mAnimator.setDir(Animator::ANIM_DIR_RIGHT);
                break;
            case GESTURE_RIGHT:
                mAnimator.setDir(Animator::ANIM_DIR_LEFT);
                break;
            default:
                break;
        }
        mAnimator.start();
        mAnimator.tick();
        mDisplayInterface.update();
    }
}
