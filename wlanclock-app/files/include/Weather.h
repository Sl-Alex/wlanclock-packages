#ifndef WEATHER_H
#define WEATHER_H

#include <cstdint>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include "SysTimer.h"
#include <json-c/json.h>

class Weather: public ISysTimer
{
    public:
        struct WeatherInfo
        {
            uint64_t timestamp;
            float    temperature;
            float    pressure;
            float    humidity;
            float    wind_speed;
            uint32_t weatherId;
            float rain;
            float snow;
            float clouds;
            std::string weatherIcon;
        };
        static Weather& getInstance()
        {
            // Guaranteed to be destroyed.
            // Instantiated on first use.
            static Weather instance;

            return instance;
        }

        virtual ~Weather() {};

        Weather(Weather const&)  = delete;
        void operator=(Weather const&) = delete;

        virtual void onTimer();
        std::vector<WeatherInfo> getWeatherForecast();

    private:
        Weather();

        void updateWeather();
        std::string getRawWeatherData();
        int parseRawWeatherData(std::string json);
        int parseWeatherInfo(WeatherInfo &info, json_object *);
        std::string mWeatherUrl;

        WeatherInfo                 mCurrentWeather;
        std::vector<WeatherInfo>    mWeatherForecast;
        std::thread                *mThread;
        std::atomic<bool>           mUpdateDone;
        std::mutex                  mWeatherMutex;
};

#endif /* WEATHER_H */
