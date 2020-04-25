#ifndef PRESHUMTEMPCONTROLLER_H
#define PRESHUMTEMPCONTROLLER_H

#include "UbusServer.h"
#include "AbstractDisplayInterface.h"

#include <cstdint>
#include <vector>
#include <mutex>

class PresHumTempController: public IPresHumTempReceiver
{
    public:
        static PresHumTempController& getInstance()
        {
            // Guaranteed to be destroyed.
            // Instantiated on first use.
            static PresHumTempController instance;

            return instance;
        }

        virtual ~PresHumTempController();

        bool getPresHumTemp(float &pres, float &hum, float &temp);
        void onPresHumTemp(double pres, double hum, double temp);

        PresHumTempController(PresHumTempController const&)  = delete;
        void operator=(PresHumTempController const&) = delete;
    private:
        PresHumTempController();

        float mLastPres;
        float mLastHum;
        float mLastTemp;
        bool  mNewDataAvailable;

        std::mutex mDataMutex;
};

#endif /* PRESHUMTEMPCONTROLLER_H */
