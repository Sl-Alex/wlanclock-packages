#include "main.h"
#include "BrightnessController.h"
#include <iostream>

BrightnessController::BrightnessController()
    :mIface(nullptr)
    ,mBrightness(0)
{
    UbusServer::getInstance().subscribeBrightness(*this);
}

BrightnessController::~BrightnessController()
{
    UbusServer::getInstance().unsubscribeBrightness();
}

void BrightnessController::setDisplayInterface(AbstractDisplayInterface *iface)
{
    mIface = iface;
}

void BrightnessController::onBrightness(uint32_t brightness)
{
    uint32_t new_brightness = brightness / 10;
    if (new_brightness != mBrightness)
    {
        mBrightness = new_brightness;
        if (mIface)
        {
            mIface->setBrightness(mBrightness);
        }
        /* TODO: levels, limits */
    }
}
