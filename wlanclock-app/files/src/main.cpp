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
 
int main(void)
{
    AbstractCanvas *canvas = new Canvas<CANVAS_COLOR_4BIT>(128, 32);
    SpiDisplayInterface *displayInterface = new SpiDisplayInterface("/dev/spidev0.1", 1);

    displayInterface->start();
    std::cout << "displayInterface started" << std::endl;

    canvas->drawLine(  0,  0,127, 31, 0x0000FFFF);
    canvas->drawLine(127,  0,  0, 31, 0xFF0000FF);
    canvas->drawRect(  0,  0,127, 31, 0x00FF00FF);

    if (0 == UbusServer::getInstance().start())
    {
        std::cout << "UbusServer started" << std::endl;
    }
    else
    {
        exit(1);
    }

    displayInterface->setBrightness(0);
    while(UbusServer::getInstance().isRunning())
    {
        usleep(10000);
        displayInterface->update(*canvas);
    }

    UbusServer::getInstance().stop();
    std::cout << "UbusServer stopped" << std::endl;

    displayInterface->stop();
    std::cout << "displayInterface stopped" << std::endl;

    delete canvas;
    delete displayInterface;
    std::cout << "Done" << std::endl;
}
