#ifndef UBUSSERVER_H
#define UBUSSERVER_H

#include <cstdint>
#include <mutex>

class IGestureReceiver
{
  public:
    typedef enum
    {
        GESTURE_LEFT  = 0,
        GESTURE_RIGHT = 1,
        GESTURE_UP    = 2,
        GESTURE_DOWN  = 3,
        GESTURE_CLICK = 4
    } Gesture;
    virtual ~IGestureReceiver() {};
    virtual void onGesture(Gesture gesture) = 0;
};

class IBrightnessReceiver
{
  public:
    virtual ~IBrightnessReceiver() {};
    virtual void onBrightness(uint32_t brightness) = 0;
};

class IPresHumTempReceiver
{
  public:
    virtual ~IPresHumTempReceiver() {};
    virtual void onPresHumTemp(double pres, double hum, double temp) = 0;
};

class IRelPosReceiver
{
  public:
    virtual ~IRelPosReceiver() {};
    virtual void onRelPos(int rel_x, int rel_y) = 0;
};

class UbusServer
{
    public:
        static UbusServer& getInstance()
        {
            // Guaranteed to be destroyed.
            // Instantiated on first use.
            static UbusServer instance;

            return instance;
        }

        virtual ~UbusServer() {};

        int start(void);
        int stop (void);
        void process (void);
        bool isRunning(void);

        UbusServer(UbusServer const&)  = delete;
        void operator=(UbusServer const&) = delete;

        void subscribeGesture(IGestureReceiver     &receiver)
        {
            fGestureReceiver = &receiver;
        }
        void unsubscribeGesture()
        {
            fGestureReceiver = nullptr;
        }
        void subscribeBrightness(IBrightnessReceiver  &receiver)
        {
            fBrightnessReceiver = &receiver;
        }
        void unsubscribeBrightness()
        {
            fBrightnessReceiver = nullptr;
        }
        void subscribePresHumTemp(IPresHumTempReceiver &receiver)
        {
            fPresHumTempReceiver = &receiver;
        }
        void unsubscribePresHumTemp()
        {
            fPresHumTempReceiver = nullptr;
        }
        void subscribeRelPos(IRelPosReceiver &receiver)
        {
            fRelPosReceiver = &receiver;
        }
        void unsubscribeRelPos()
        {
            fRelPosReceiver = nullptr;
        }
    private:
        UbusServer();

        static void uBusCbGesture(uint32_t gesture);
        static void uBusCbBrightness(uint32_t brightness);
        static void uBusCbPresHumTemp(double pressure, double humidity, double temperature);
        static void uBusCbRelPos(int rel_x, int rel_y);

        IGestureReceiver     *fGestureReceiver;
        IBrightnessReceiver  *fBrightnessReceiver;
        IPresHumTempReceiver *fPresHumTempReceiver;
        IRelPosReceiver      *fRelPosReceiver;

        uint32_t            mLastBrightness;
        uint32_t            mLastGesture;
        double              mLastPressure;
        double              mLastHumidity;
        double              mLastTemperature;
        int                 mRelX;
        int                 mRelY;

        bool                mNewGesture;
        bool                mNewBrightness;
        bool                mNewPresHumTemp;
        bool                mNewRelPos;

        static std::mutex   mUbusMutex;
};

#endif /* UBUS_SERVER_H */
