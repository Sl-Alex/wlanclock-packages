/*
    spidevlib.c - A user-space program to comunicate using spidev.
                Gustavo Zamboni
*/
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <linux/types.h>

#include <iostream>

#include "UbusServer.h"
#include "ubus_server.h"
#include "Canvas.h"
#include "SpiDisplayInterface.h"
#include "SysTimer.h"
#include "Desktop.h"
#include "Fonts.h"
#include "PngStorage.h"
#include "Weather.h"
#include "BrightnessController.h"
#include "PresHumTempController.h"

int main(void)
{
    SpiDisplayInterface displayInterface;
    Fonts::getInstance().init();

    /* Initialize weather service and schedule the very first weather update */
    Weather::getInstance();
    /* Initialize pressure/humidity/temperature controller */
    PresHumTempController::getInstance();

    displayInterface.start();
    displayInterface.setBrightness(0);
    BrightnessController::getInstance().setDisplayInterface(&displayInterface);

    Desktop desktop(displayInterface);

    PngStorage::getInstance().init();
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/refresh.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/house.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/01d.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/01n.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/02d.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/02n.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/03d.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/03n.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/04d.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/04n.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/09d.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/09n.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/10d.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/10n.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/11d.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/11n.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/13d.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/13n.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/50d.png");
    PngStorage::getInstance().loadPng("/usr/share/wlanclock/images/weather/50n.png");

    if (0 != UbusServer::getInstance().start())
    {
        exit(1);
    }

    while(UbusServer::getInstance().isRunning())
    {
        SysTimer::getInstance().waitAndProcess();
    }

    UbusServer::getInstance().stop();
    displayInterface.stop();

    std::cout << "Done" << std::endl;
}
