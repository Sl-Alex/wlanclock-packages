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
    Fonts::getInstance().init();

    /* Initialize weather service and schedule the very first weather update */
    Weather::getInstance();
    /* Initialize pressure/humidity/temperature controller */
    PresHumTempController::getInstance();

    SpiDisplayInterface::getInstance().start();
    SpiDisplayInterface::getInstance().setBrightness(0);
    BrightnessController::getInstance().setDisplayInterface(&SpiDisplayInterface::getInstance());
    Desktop desktop(SpiDisplayInterface::getInstance());

    PngStorage::getInstance().init();

    if (0 != UbusServer::getInstance().start())
    {
        exit(1);
    }

    while(UbusServer::getInstance().isRunning())
    {
        SysTimer::getInstance().waitAndProcess();
        UbusServer::getInstance().process();
    }

    UbusServer::getInstance().stop();
    SpiDisplayInterface::getInstance().stop();

    std::cout << "Done" << std::endl;
}
