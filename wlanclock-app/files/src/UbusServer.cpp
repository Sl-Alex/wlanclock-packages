#include "UbusServer.h"
#include "ubus_server.h"
#include <iostream>

UbusServer::UbusServer()
    :fGestureReceiver(nullptr)
    ,fBrightnessReceiver(nullptr)
    ,fPresHumTempReceiver(nullptr)
{
}

int UbusServer::start(void)
{
    ubus_server_set_cb_gesture(uBusCbGesture);
    ubus_server_set_cb_brightness(uBusCbBrightness);
    ubus_server_set_cb_pres_hum_temp(uBusCbPresHumTemp);
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

bool UbusServer::isRunning(void)
{
    return ubus_server_is_running();
}

void UbusServer::uBusCbGesture(uint32_t gesture)
{
    if (getInstance().fGestureReceiver)
        getInstance().fGestureReceiver->onGesture((IGestureReceiver::Gesture)gesture);
}

void UbusServer::uBusCbBrightness(uint32_t brightness)
{
    if (getInstance().fBrightnessReceiver)
        getInstance().fBrightnessReceiver->onBrightness(brightness);
}

void UbusServer::uBusCbPresHumTemp(double pressure, double humidity, double temperature)
{
    if (getInstance().fPresHumTempReceiver)
        getInstance().fPresHumTempReceiver->onPresHumTemp(pressure, humidity, temperature);
}


