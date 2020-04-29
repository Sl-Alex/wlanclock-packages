#include "MenuColor.h"
#include "PngStorage.h"
#include "Canvas.h"
#include "Config.h"
#include "UciReader.h"
#include <iostream>

MenuColor::MenuColor(AbstractDisplayInterface &displayIface, IMenuInteraction *menuInterface)
    :mMenuInterface(menuInterface)
    ,mDisplayInterface(displayIface)
    ,mSelectedColor(0)
{
    mBgCanvas   = new Canvas<CANVAS_COLOR_4BIT>(Config::Display::WIDTH, Config::Display::HEIGHT);
    mFgCanvas   = new Canvas<CANVAS_COLOR_4BIT>(Config::Display::WIDTH, Config::Display::HEIGHT);
    mColorR = (Config::Display::color >> 24) & 0xFF;
    mColorG = (Config::Display::color >> 16) & 0xFF;
    mColorB = (Config::Display::color >> 8)  & 0xFF;
    mSavedR = mColorR;
    mSavedG = mColorG;
    mSavedB = mColorB;
    mPrevColor = Config::Display::color;
    drawMenu();
    mAnimator.setFgBgDest(*mFgCanvas, *mBgCanvas, *displayIface.getCanvas());
    mAnimator.setDir(Animator::ANIM_DIR_LEFT);
    mAnimator.setType(Animator::ANIM_TYPE_SLIDE);
    mAnimator.setSpeed(6);
    mAnimator.start();
    mAnimator.tick();
    mDisplayInterface.update();
    SysTimer::getInstance().subscribe(*this, Config::Display::UPDATE_INTERVAL, true);
    UbusServer::getInstance().subscribeGesture(*this);
    UbusServer::getInstance().subscribeRelPos(*this);
}

void MenuColor::drawMenu()
{
    constexpr int GRAD_HEIGHT = Config::Display::HEIGHT - 4;
    constexpr int GRAD_WIDTH  = 6;
    constexpr int HANDLE_WIDTH = GRAD_WIDTH + 2;
    constexpr int HANDLE_INTERVAL = HANDLE_WIDTH + 2;
    constexpr int OFFSET_LEFT = 1;
    mFgCanvas->clear();
    /* Draw three gradients (R,G,B) with handles */
    for (int i = 0; i < GRAD_HEIGHT; i++)
    {
        uint32_t pos   = i + (Config::Display::HEIGHT - GRAD_HEIGHT)/2;
        uint32_t alpha = 255 - i * 255 / (GRAD_HEIGHT - 1);
        rgba32_t color_r = 0xFF000000 | alpha;
        rgba32_t color_g = 0x00FF0000 | alpha;
        rgba32_t color_b = 0x0000FF00 | alpha;
        int offset_left = OFFSET_LEFT + (HANDLE_WIDTH - GRAD_WIDTH)/2;
        mFgCanvas->drawLine(offset_left,pos,offset_left + GRAD_WIDTH - 1, pos, color_r);
        offset_left += HANDLE_INTERVAL;
        mFgCanvas->drawLine(offset_left,pos,offset_left + GRAD_WIDTH - 1, pos, color_g);
        offset_left += HANDLE_INTERVAL;
        mFgCanvas->drawLine(offset_left,pos,offset_left + GRAD_WIDTH - 1, pos, color_b);
    }
    int targetR = (Config::Display::HEIGHT - GRAD_HEIGHT)/2 + (255 - mColorR) * (GRAD_HEIGHT - 1) / 255;
    int targetG = (Config::Display::HEIGHT - GRAD_HEIGHT)/2 + (255 - mColorG) * (GRAD_HEIGHT - 1) / 255;
    int targetB = (Config::Display::HEIGHT - GRAD_HEIGHT)/2 + (255 - mColorB) * (GRAD_HEIGHT - 1) / 255;
    int offset_left = OFFSET_LEFT;
    mFgCanvas->drawRect(offset_left,targetR - 1, offset_left + HANDLE_WIDTH - 1, targetR + 1, (mSelectedColor == 0) ? 0xFFFFFFFF : 0xFFFFFF80);
    offset_left += HANDLE_INTERVAL;
    mFgCanvas->drawRect(offset_left,targetG - 1, offset_left + HANDLE_WIDTH - 1, targetG + 1, (mSelectedColor == 1) ? 0xFFFFFFFF : 0xFFFFFF80);
    offset_left += HANDLE_INTERVAL;
    mFgCanvas->drawRect(offset_left,targetB - 1, offset_left + HANDLE_WIDTH - 1, targetB + 1, (mSelectedColor == 2) ? 0xFFFFFFFF : 0xFFFFFF80);

    int width = Config::Display::WIDTH - 2 - offset_left - HANDLE_INTERVAL - 2;
    int r1_l = offset_left + HANDLE_INTERVAL + 2;
    int r1_r = r1_l + width/2 - 1;
    int r2_l = r1_l + width/2 + 1;
    int r2_r = Config::Display::WIDTH - 2;
    rgba32_t color = ((uint32_t)mColorR << 24) | ((uint32_t)mColorG << 16) | ((uint32_t)mColorB << 8) | 0x000000FF;
    mFgCanvas->drawRect(r1_l, 13, r1_r, Config::Display::HEIGHT - 2, 0xFFFFFFA0);
    mFgCanvas->drawFilledRect(r1_l + 1, 14, r1_r - 1, Config::Display::HEIGHT - 3, color);
    mFgCanvas->drawRect(r2_l, 13, r2_r, Config::Display::HEIGHT - 2, 0xFFFFFFA0);
    mFgCanvas->drawFilledRect(r2_l + 1, 14, r2_r - 1, Config::Display::HEIGHT - 3, mPrevColor);

    /* Print a hint */
    std::wstring line1(L"click to apply");
    Config::Fonts::Params params;
    params = Config::Fonts::PARAMS[Config::Fonts::FONT_WEATHER];
    params.base_y = 10;
    params.base_x = r1_l + 10;
    mFgCanvas->drawText(params, line1, 0xFF700000);
}

void MenuColor::onGesture(IGestureReceiver::Gesture gesture)
{
    if (mAnimator.isRunning())
    {
        mAnimator.finish();
    }
    switch(gesture)
    {
        /* Intentional fall-through */
        case GESTURE_LEFT:
        case GESTURE_UP:
        case GESTURE_DOWN:
            return;
        case GESTURE_CLICK:
            switch(mSelectedColor)
            {
                case 0:
                    mSavedR = mColorR;
                    break;
                case 1:
                    mSavedG = mColorG;
                    break;
                case 2:
                    mSavedB = mColorB;
                    break;
            }
            mSelectedColor++;
            mSelectedColor %= 3;
            drawMenu();
            mFgCanvas->copyTo(*mDisplayInterface.getCanvas(),0,0);
            mDisplayInterface.update();
            SysTimer::getInstance().subscribe(*this, Config::Display::MENU_TIMEOUT, false);
            return;
        default:
            break;
    }

    Config::Display::color = ((uint32_t)mSavedR << 24) | ((uint32_t)mSavedG << 16) | ((uint32_t)mSavedB << 8) | 0x000000FF;

    constexpr const char hex_digits[] = "0123456789ABCDEF";
    std::string colorHex;
    colorHex.reserve(8);
    uint32_t value = Config::Display::color;
    for (int i = 0; i < 4; i++)
    {
        uint8_t byte = (value & 0xFF000000) >> 24;
        value <<= 8;
        colorHex.push_back(hex_digits[byte >> 4]);
        colorHex.push_back(hex_digits[byte & 0x0F]);
    }
    UciReader::getInstance().setKey(Config::UciPaths::COLOR, colorHex.c_str());

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

void MenuColor::onRelPos(int rel_x, int rel_y)
{
    if (mAnimator.isRunning())
    {
        mAnimator.finish();
    }
    switch(mSelectedColor)
    {
        case 0:
            mColorR -= rel_y;
            break;
        case 1:
            mColorG -= rel_y;
            break;
        case 2:
            mColorB -= rel_y;
            break;
    }
    if (mColorR > 255) mColorR = 255;
    if (mColorG > 255) mColorG = 255;
    if (mColorB > 255) mColorB = 255;
    if (mColorR < 0) mColorR = 0;
    if (mColorG < 0) mColorG = 0;
    if (mColorB < 0) mColorB = 0;
    drawMenu();
    mFgCanvas->copyTo(*mDisplayInterface.getCanvas(),0,0);
    mDisplayInterface.update();
    SysTimer::getInstance().subscribe(*this, Config::Display::MENU_TIMEOUT, false);
}

void MenuColor::onMenuAction(IMenuInteraction::MenuAction action, IGestureReceiver::Gesture gesture)
{
}

void MenuColor::onTimer()
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
        UbusServer::getInstance().unsubscribeRelPos();

        delete mBgCanvas;
        delete mFgCanvas;

        /* This is the very last step, we will be destroyed here */
        if (mMenuInterface)
            mMenuInterface->onMenuAction(MENUACTION_EXIT, GESTURE_RIGHT);
    }
}
