#ifndef SPIDISPLAYINTERFACE_H
#define SPIDISPLAYINTERFACE_H

#include "AbstractDisplayInterface.h"
#include "spi_ctrl.h"
#include <mutex>
#include <thread>
#include <condition_variable>

class SpiDisplayInterface : public AbstractDisplayInterface
{
    public:
        static SpiDisplayInterface& getInstance()
        {
            // Guaranteed to be destroyed.
            // Instantiated on first use.
            static SpiDisplayInterface instance;

            return instance;
        }

        virtual ~SpiDisplayInterface();

        virtual int start(void);
        int stop(void);
        virtual AbstractCanvas *getCanvas();

        virtual void setBrightness(int brightness);
        virtual void update();

        SpiDisplayInterface(SpiDisplayInterface const&)  = delete;
        void operator=(SpiDisplayInterface const&) = delete;
    private:
        SpiDisplayInterface();

        void spi_loop(void);

        uint32_t prepareConfig(void);
        const int SPI_SPEED = 10000000;
        int mBrightness;
        int mSpiFd;
        int mGpio;

        static constexpr const char* CONFIG_FILE          = "wlanclock";
        static constexpr const char* CONFIG_SECTION       = "app";
        static constexpr const char* CONFIG_KEY_SPIDEV    = "device";
        static constexpr const char* CONFIG_KEY_GPIO      = "dat_ncfg_gpio";

        std::mutex mThreadMutex;
        std::thread *mThread;

        char *mDisplayBuffer[2];
        AbstractCanvas *mDestCanvas;
        int  mDisplayBufferSize;
        int mDisplayBufferIndex;
        bool mNewBrightness;
        bool mNewDisplayData;
        bool mStopThread;
        std::condition_variable mThreadCond;
};

#endif /* SPIDISPLAYINTERFACE_H */
