#include "SpiDisplayInterface.h"
#include "gpio_ctrl.h"
#include "spi_ctrl.h"

#include "unistd.h"
#include <iostream>
#include <limits>

SpiDisplayInterface::SpiDisplayInterface(std::string spidev, int dat_ncfg_gpio)
    :mBrightness(0)
    ,mGpio(dat_ncfg_gpio)
    ,mThread(nullptr)
    ,mDisplayBufferIndex(0)
    ,mNewBrightness(false)
    ,mNewDisplayData(false)
{
    gpio_ctrl_init(mGpio);
    gpio_ctrl_set_dir(mGpio, GPIO_CTRL_DIR_OUT);
    mSpiFd = spi_init(spidev.c_str(), SPI_SPEED);
    mDisplayBuffer[0] = nullptr;
    mDisplayBuffer[1] = nullptr;
}

SpiDisplayInterface::~SpiDisplayInterface()
{
    delete[] mDisplayBuffer[0];
    delete[] mDisplayBuffer[1];
}

void SpiDisplayInterface::spi_loop(void)
{
    bool newBrightness = false;
    bool newData = false;
    int newDataIndex;
    char cfg_data_arr[4];
    while (!mStopThread)
    {
        {
            std::unique_lock<std::mutex> lock(mThreadMutex);
            while ((!mNewBrightness) && (!mNewDisplayData) && (!mStopThread))
            {
                mThreadCond.wait(lock);
            }
            if (mNewBrightness)
            {
                mNewBrightness = false;
                newBrightness = true;
                uint32_t cfg_data = prepareConfig();
                cfg_data_arr[0] = (cfg_data >> 24) & 0xFF;
                cfg_data_arr[1] = (cfg_data >> 16) & 0xFF;
                cfg_data_arr[2] = (cfg_data >> 8) & 0xFF;
                cfg_data_arr[3] = cfg_data & 0xFF;
            }
            if (mNewDisplayData)
            {
                mNewDisplayData = false;
                newData = true;
                newDataIndex = mDisplayBufferIndex;
                mNewDisplayData = false;
                mDisplayBufferIndex = 1 - mDisplayBufferIndex;
            }
        }
        if (newBrightness)
        {
            newBrightness = false;
            gpio_ctrl_set_value(mGpio, 1);
            usleep(1);
            gpio_ctrl_set_value(mGpio, 0);
            spi_write(mSpiFd, cfg_data_arr, sizeof(cfg_data_arr));
        }
        if (newData)
        {
            newData = false;
            gpio_ctrl_set_value(mGpio, 0);
            usleep(1);
            gpio_ctrl_set_value(mGpio, 1);
            spi_write(mSpiFd, mDisplayBuffer[newDataIndex], mDisplayBufferSize);
        }
    }
}

int SpiDisplayInterface::start(void)
{
    if (mThread != nullptr)
    {
        std::cerr << "Thread is already running" << std::endl;
        return 1;
    }
    /* Start a separate thread which sends everything over SPI */
    mStopThread = false;
    mThread = new std::thread(&SpiDisplayInterface::spi_loop, this);
    return 0;
}

int SpiDisplayInterface::stop(void)
{
    if (mThread == nullptr)
    {
        std::cerr << "Thread is not running" << std::endl;
        return 1;
    }

    {
        /* Stop SPI thread */
        std::unique_lock<std::mutex> lock(mThreadMutex);
        mStopThread = true;
        mThreadCond.notify_one();
    }
    mThread->join();
    delete mThread;
    mThread = nullptr;
    return 0;
}

uint32_t SpiDisplayInterface::prepareConfig(void)
{
    /* Here we prepare FM6126A configuration registers */

    /* These are default bits without brightness */
    constexpr uint16_t CFG1_DEFAULT = 0x7000;
    constexpr uint16_t CFG2_DEFAULT = 0x0040;

    /* Brightness has 6 bits */
    constexpr uint16_t MAX_BRIGHTNESS = (1 << 6) - 1;

    int br = mBrightness;

    /* Brightness range is 0..100 */
    if (br > 100) br = 100;
    if (br <   0) br = 0;

    /* Convert range of 0..100 to 0..MAX_BRIGHTNESS */
    br = (MAX_BRIGHTNESS * br) / 100;

    /* Brightness in FM6126A consists of two parts */
    uint16_t lower_2 = br & 0x03;
    uint16_t upper_4 = (br >> 2) & 0x0F;
    /* Lower two bits shall be inverted */
    lower_2 = (~lower_2) & 0x03;

    uint16_t cfg1 = CFG1_DEFAULT | (upper_4 << 7) | (lower_2 << 2);
    uint16_t cfg2 = CFG2_DEFAULT;
    return ((uint32_t)cfg1 << 16) | cfg2;
}

void SpiDisplayInterface::setBrightness(int brightness)
{
    std::unique_lock<std::mutex> lock(mThreadMutex);
    mBrightness = brightness;
    mNewBrightness = true;
    mThreadCond.notify_one();
}

void SpiDisplayInterface::update(AbstractCanvas &canvas)
{
    std::unique_lock<std::mutex> lock(mThreadMutex);
    if (mDisplayBuffer[mDisplayBufferIndex] == nullptr)
    {
        mDisplayBuffer[mDisplayBufferIndex] = new char[canvas.getSize()];
        mDisplayBufferSize = canvas.getSize();
    }
    if (mDisplayBufferSize != canvas.getSize())
    {
        std::cerr << "mDisplayBufferSize != canvas.getSize()" << std::endl;
        return;
    }
    memcpy(mDisplayBuffer[mDisplayBufferIndex], canvas.getData(), canvas.getSize());
    mNewDisplayData = true;
    mThreadCond.notify_one();
}

