#include "Desktop.h"
#include "Canvas.h"

Desktop::Desktop(AbstractDisplayInterface &displayIface)
    : mDisplayInterface(displayIface)
{
    mBgCanvas = new Canvas<CANVAS_COLOR_4BIT>(DESKTOP_WIDTH, DESKTOP_HEIGHT);
}

Desktop::~Desktop()
{
    delete mBgCanvas;
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
    static uint8_t cnt = 0;
    mBgCanvas->clear();
    mBgCanvas->drawLine(  cnt,  0,cnt, 31, 0x007000FF);
    cnt++;
    cnt = cnt % 128;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char buff[100];
    snprintf(buff, sizeof(buff), "%02d:%02d", tm.tm_hour, tm.tm_min);
    mBgCanvas->drawText(
            mHourMinParams.base_x,
            mHourMinParams.base_y,
            mHourMinParams.index,
            mHourMinParams.size_h,
            mHourMinParams.size_v,
            buff);
    snprintf(buff, sizeof(buff), "%02d", tm.tm_sec);
    mBgCanvas->drawText(
            mSecParams.base_x,
            mSecParams.base_y,
            mSecParams.index,
            mSecParams.size_h,
            mSecParams.size_v,
            buff);
    mDisplayInterface.update(*mBgCanvas);
}

void Desktop::onGesture(uint32_t gesture)
{
}
