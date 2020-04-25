#ifndef BRIGHTNESSCONTROLLER_H
#define BRIGHTNESSCONTROLLER_H

#include "UbusServer.h"
#include "AbstractDisplayInterface.h"

#include <time.h>
#include <cstdint>
#include <vector>

class BrightnessController: public IBrightnessReceiver
{
    public:
        static BrightnessController& getInstance()
        {
            // Guaranteed to be destroyed.
            // Instantiated on first use.
            static BrightnessController instance;

            return instance;
        }

        virtual ~BrightnessController();

        void onBrightness(uint32_t brightness);
        void setDisplayInterface(AbstractDisplayInterface *iface);

        BrightnessController(BrightnessController const&)  = delete;
        void operator=(BrightnessController const&) = delete;
    private:
        BrightnessController();

        AbstractDisplayInterface *mIface;
        uint32_t mBrightness;
};

#endif /* BRIGHTNESSCONTROLLER_H */
