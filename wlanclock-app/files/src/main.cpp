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

/* TODO: delete this debug stuff */
class BrightnessReceiver: public IBrightnessReceiver
{
    public:
        void onBrightness(uint32_t brightness){mBrightness = brightness; newBrightness = true;}
        uint32_t getBrightness(void) {newBrightness = false; return mBrightness; }
        bool getNewBrightness(void){return newBrightness;}
    private:
        uint32_t mBrightness;
        bool newBrightness = false;

};
 
int main(void)
{
    AbstractCanvas *canvas = new Canvas<CANVAS_COLOR_4BIT>(128, 32);
    SpiDisplayInterface *displayInterface = new SpiDisplayInterface("/dev/spidev0.1", 1);
    BrightnessReceiver brigthness_receiver;

    displayInterface->start();
    std::cout << "displayInterface started" << std::endl;

    /* TODO: remove */
    canvas->drawLine(  0,  0,127, 31, 0x0000FFFF);
    canvas->drawLine(127,  0,  0, 31, 0xFF0000FF);
    canvas->drawRect(  0,  0,127, 31, 0x00FF00FF);

    UbusServer::getInstance().registerReceiver(brigthness_receiver);

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
        if (brigthness_receiver.getNewBrightness())
        {
            displayInterface->setBrightness((int)brigthness_receiver.getBrightness());
        }
    }

    UbusServer::getInstance().stop();
    std::cout << "UbusServer stopped" << std::endl;

    displayInterface->stop();
    std::cout << "displayInterface stopped" << std::endl;

    delete canvas;
    delete displayInterface;
    std::cout << "Done" << std::endl;
}
