#ifndef SPIDISPLAYINTERFACE_H
#define SPIDISPLAYINTERFACE_H

#include <string>

#include "AbstractDisplayInterface.h"
#include "spi_ctrl.h"
#include <mutex>
#include <thread>
#include <condition_variable>

class SpiDisplayInterface : public AbstractDisplayInterface
{
    public:
        SpiDisplayInterface(std::string spidev, int dat_ncfg_gpio);
        virtual ~SpiDisplayInterface();

        virtual int start(void);
        int stop(void);

        virtual void setBrightness(int brightness);
        virtual void update(AbstractCanvas &cavas);

    private:

        void spi_loop(void);

        uint32_t prepareConfig(void);
        const int SPI_SPEED = 10000000;
        int mBrightness;
        int mSpiFd;
        int mGpio;

        std::mutex mThreadMutex;
        std::thread *mThread;

        char *mDisplayBuffer[2];
        int  mDisplayBufferSize;
        int mDisplayBufferIndex;
        bool mNewBrightness;
        bool mNewDisplayData;
        bool mStopThread;
        std::condition_variable mThreadCond;
};

#endif /* SPIDISPLAYINTERFACE_H */
