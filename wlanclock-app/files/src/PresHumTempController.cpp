#include "main.h"
#include "PresHumTempController.h"
#include <iostream>

PresHumTempController::PresHumTempController()
    :mNewDataAvailable(false)
{
    UbusServer::getInstance().subscribePresHumTemp(*this);
}

PresHumTempController::~PresHumTempController()
{
    UbusServer::getInstance().unsubscribePresHumTemp();
}

bool PresHumTempController::getPresHumTemp(float &pres, float &hum, float &temp)
{
    std::lock_guard<std::mutex> lock(mDataMutex);
    /* Update anyway */
    pres = mLastPres;
    hum  = mLastHum;
    temp = mLastTemp;

    /* Return true if data was updated before */
    if (!mNewDataAvailable)
    {
        return false;
    }
    mNewDataAvailable = false;
    return true;
}


void PresHumTempController::onPresHumTemp(double pres, double hum, double temp)
{
    std::lock_guard<std::mutex> lock(mDataMutex);
    mLastPres = pres;
    mLastHum  = hum;
    mLastTemp = temp;
    mNewDataAvailable = false;
}
