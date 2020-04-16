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
    SpiDisplayInterface displayInterface("/dev/spidev0.1", 1);
    BrightnessReceiver brigthness_receiver;
    Fonts::getInstance().init();

    displayInterface.start();
    displayInterface.setBrightness(0);

    Desktop desktop(displayInterface);
    desktop.init();

    std::cout << "displayInterface started" << std::endl;

    UbusServer::getInstance().registerReceiver(brigthness_receiver);

    if (0 == UbusServer::getInstance().start())
    {
        std::cout << "UbusServer started" << std::endl;
    }
    else
    {
        exit(1);
    }

    SysTimer::getInstance().subscribe(desktop, 10, true);
    while(UbusServer::getInstance().isRunning())
    {
        SysTimer::getInstance().waitAndProcess();

        if (brigthness_receiver.getNewBrightness())
        {
            displayInterface.setBrightness((int)brigthness_receiver.getBrightness());
        }
    }

    UbusServer::getInstance().stop();
    std::cout << "UbusServer stopped" << std::endl;

    displayInterface.stop();
    std::cout << "displayInterface stopped" << std::endl;

    std::cout << "Done" << std::endl;
}
