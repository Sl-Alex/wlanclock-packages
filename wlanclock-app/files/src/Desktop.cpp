#include "Desktop.h"
#include "Canvas.h"

Desktop::Desktop(AbstractDisplayInterface &displayIface)
    : mDisplayInterface(displayIface)
{
    mDestCanvas = new Canvas<CANVAS_COLOR_4BIT>(DESKTOP_WIDTH, DESKTOP_HEIGHT);
    mBgCanvas   = new Canvas<CANVAS_COLOR_4BIT>(DESKTOP_WIDTH, DESKTOP_HEIGHT);
    mFgCanvas   = new Canvas<CANVAS_COLOR_4BIT>(DESKTOP_WIDTH, DESKTOP_HEIGHT);
}

Desktop::~Desktop()
{
    delete mDestCanvas;
    delete mBgCanvas;
    delete mFgCanvas;
}

int Desktop::init()
{
    int rc = 0;
    rc  = Fonts::getInstance().loadFontParams(FONT_HOURS_MINS, mHourMinParams);
    rc |= Fonts::getInstance().loadFontParams(FONT_SECONDS,    mSecParams);
    return rc;
}

void Desktop::onTimer()
{
    static uint8_t r = 255;
    static uint8_t g = 0;
    static uint8_t b = 0;
    static uint8_t step = 0;
    static struct tm old_stm;
    time_t t = time(NULL);
    struct tm stm = *localtime(&t);
    if (memcmp(&stm, &old_stm, sizeof(stm)))
    {
        memcpy(&old_stm, &stm, sizeof(stm));
        if (mAnimator.isRunning())
        {
            mAnimator.finish();
        }
        char buff[100];
        snprintf(buff, sizeof(buff), "%02d:%02d", stm.tm_hour, stm.tm_min);
        rgba32_t color = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8);
        mFgCanvas->clear();
        mFgCanvas->drawText(
                mHourMinParams.base_x,
                mHourMinParams.base_y,
                mHourMinParams.index,
                mHourMinParams.size_h,
                mHourMinParams.size_v,
                std::string(buff), color);
        snprintf(buff, sizeof(buff), "%02d", stm.tm_sec);
        mFgCanvas->drawText(
                mSecParams.base_x,
                mSecParams.base_y,
                mSecParams.index,
                mSecParams.size_h,
                mSecParams.size_v,
                std::string(buff), color);
        mAnimator.setFgBgDest(*mFgCanvas, *mBgCanvas, *mDestCanvas);
        mAnimator.setDir(Animator::ANIM_DIR_DOWN);
        mAnimator.setSpeed(10);
        mAnimator.setType(Animator::ANIM_TYPE_OPACITY);
        mAnimator.start();
    }
    if (mAnimator.isRunning())
    {
        mAnimator.tick();
        mDisplayInterface.update(*mDestCanvas);
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

void Desktop::onGesture(uint32_t gesture)
{
}
