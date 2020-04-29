#include "UbusServer.h"
#include "ubus_server.h"
#include <iostream>

std::mutex UbusServer::mUbusMutex;

UbusServer::UbusServer()
    :fGestureReceiver(nullptr)
    ,fBrightnessReceiver(nullptr)
    ,fPresHumTempReceiver(nullptr)
    ,fRelPosReceiver(nullptr)
{
}

int UbusServer::start(void)
{
    ubus_server_set_cb_gesture(uBusCbGesture);
    ubus_server_set_cb_brightness(uBusCbBrightness);
    ubus_server_set_cb_pres_hum_temp(uBusCbPresHumTemp);
    ubus_server_set_cb_rel_pos(uBusCbRelPos);
    int rc = ubus_server_init();
    if (0 == rc)
    {
        std::cout << "UbusServer started at " << wlanclock_data_obj.name << std::endl;
    }
    return rc;
}

int UbusServer::stop(void)
{
    ubus_server_deinit();
    std::cout << "UbusServer stopped" << std::endl;
    return 0;
}

void UbusServer::process()
{
    std::lock_guard<std::mutex> lock(mUbusMutex);
    if (mNewGesture)
    {
        if (getInstance().fGestureReceiver)
            getInstance().fGestureReceiver->onGesture((IGestureReceiver::Gesture)mLastGesture);
    }
    if (mNewBrightness)
    {
        if (getInstance().fBrightnessReceiver)
            getInstance().fBrightnessReceiver->onBrightness(mLastBrightness);
    }
    if (mNewPresHumTemp)
    {
        if (getInstance().fPresHumTempReceiver)
            getInstance().fPresHumTempReceiver->onPresHumTemp(mLastPressure, mLastHumidity, mLastTemperature);
    }
    if (mNewRelPos)
    {
        if (getInstance().fRelPosReceiver)
            getInstance().fRelPosReceiver->onRelPos(mRelX, mRelY);
        mRelX = 0;
        mRelY = 0;
    }
    mNewGesture = false;
    mNewBrightness = false;
    mNewPresHumTemp = false;
    mNewRelPos = false;
}

bool UbusServer::isRunning(void)
{
    return ubus_server_is_running();
}

void UbusServer::uBusCbGesture(uint32_t gesture)
{
    std::lock_guard<std::mutex> lock(mUbusMutex);
    UbusServer::getInstance().mLastGesture = gesture;
    UbusServer::getInstance().mNewGesture = true;
}

void UbusServer::uBusCbBrightness(uint32_t brightness)
{
    std::lock_guard<std::mutex> lock(mUbusMutex);
    UbusServer::getInstance().mLastBrightness = brightness;
    UbusServer::getInstance().mNewBrightness = true;
}

void UbusServer::uBusCbPresHumTemp(double pressure, double humidity, double temperature)
{
    std::lock_guard<std::mutex> lock(mUbusMutex);
    UbusServer::getInstance().mLastPressure = pressure;
    UbusServer::getInstance().mLastHumidity = humidity;
    UbusServer::getInstance().mLastTemperature = temperature;
    UbusServer::getInstance().mNewPresHumTemp = true;
}

void UbusServer::uBusCbRelPos(int rel_x, int rel_y)
{
    std::lock_guard<std::mutex> lock(mUbusMutex);
    UbusServer::getInstance().mRelX += rel_x;
    UbusServer::getInstance().mRelY += rel_y;
    UbusServer::getInstance().mNewRelPos = true;
}


