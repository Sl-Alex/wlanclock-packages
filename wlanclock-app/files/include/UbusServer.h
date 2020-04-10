#ifndef UBUSSERVER_H
#define UBUSSERVER_H

#include <cstdint>

class IGestureReceiver
{
  public:
    virtual ~IGestureReceiver() {};
    virtual void onGesture(uint32_t gesture) = 0;
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
        bool isRunning(void);

        UbusServer(UbusServer const&)  = delete;
        void operator=(UbusServer const&) = delete;

        void registerReceiver(IGestureReceiver     &receiver)
        {
            fGestureReceiver = &receiver;
        }
        void registerReceiver(IBrightnessReceiver  &receiver)
        {
            fBrightnessReceiver = &receiver;
        }
        void registerReceiver(IPresHumTempReceiver &receiver)
        {
            fPresHumTempReceiver = &receiver;
        }
    private:
        UbusServer();

        static void uBusCbGesture(uint32_t gesture);
        static void uBusCbBrightness(uint32_t brightness);
        static void uBusCbPresHumTemp(double pressure, double humidity, double temperature);

        IGestureReceiver     *fGestureReceiver;
        IBrightnessReceiver  *fBrightnessReceiver;
        IPresHumTempReceiver *fPresHumTempReceiver;
};

#endif /* UBUS_SERVER_H */
