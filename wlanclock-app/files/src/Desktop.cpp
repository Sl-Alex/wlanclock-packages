#include "Desktop.h"
#include "Canvas.h"
#include "Config.h"
#include "UciReader.h"
#include <iostream>

Desktop::Desktop(AbstractDisplayInterface &displayIface)
    : mDisplayInterface(displayIface)
    , mMenuForecast(nullptr)
    , mMenuLocalStatus(nullptr)
    , mMenuSettings(nullptr)
{
    rgba32_t color;
    std::string colorString = UciReader::getInstance().getKey(Config::UciPaths::COLOR);
    try {
        color = std::stoul(colorString, 0, 16);
    } catch(...) {
        std::cerr << "Can't get color" << std::endl;
        color = 0x808080FF;
    }
    Config::Display::color = color;
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
    mFgCanvas->drawText(Config::Fonts::PARAMS[Config::Fonts::FONT_HOURS_MINS], std::string(buff), Config::Display::color);
    snprintf(buff, sizeof(buff), "%02d", mTm.tm_sec);
    mFgCanvas->drawText(Config::Fonts::PARAMS[Config::Fonts::FONT_SECONDS], std::string(buff), Config::Display::color);
}

void Desktop::onTimer()
{
    time_t t = time(NULL);
    tm newTm = *localtime(&t);
    if (!mAnimator.isRunning() && memcmp(&newTm, &mTm, sizeof(mTm)))
    {
        memcpy(&mTm, &newTm, sizeof(mTm));
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
}

void Desktop::onGesture(Gesture gesture)
{
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
        case GESTURE_LEFT:
            mAnimator.finish();
            SysTimer::getInstance().unsubscribe(*this);
            UbusServer::getInstance().unsubscribeGesture();
            mMenuSettings = new MenuColor(mDisplayInterface, this);
            break;
        default:
            break;
    }
}

void Desktop::onMenuAction(IMenuInteraction::MenuAction action, IGestureReceiver::Gesture gesture)
{
    if (action == MENUACTION_EXIT)
    {
        delete mMenuForecast;
        delete mMenuLocalStatus;
        delete mMenuSettings;
        mMenuForecast    = nullptr;
        mMenuLocalStatus = nullptr;
        mMenuSettings    = nullptr;
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
        switch (gesture)
        {
            case GESTURE_UP:
                mAnimator.setSpeed(2);
                mAnimator.setDir(Animator::ANIM_DIR_UP);
                break;
            case GESTURE_DOWN:
                mAnimator.setSpeed(2);
                mAnimator.setDir(Animator::ANIM_DIR_DOWN);
                break;
            case GESTURE_LEFT:
                mAnimator.setSpeed(6);
                mAnimator.setDir(Animator::ANIM_DIR_LEFT);
                break;
            case GESTURE_RIGHT:
                mAnimator.setSpeed(6);
                mAnimator.setDir(Animator::ANIM_DIR_RIGHT);
                break;
            default:
                break;
        }
        mAnimator.start();
        mAnimator.tick();
        mDisplayInterface.update();
    }
}
