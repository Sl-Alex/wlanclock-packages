#ifndef MAIN_H
#define MAIN_H

#include <string>

namespace Config
{
    class Display
    {
        public:
            static constexpr int WIDTH     = 128;
            static constexpr int HEIGHT    = 32;
            static constexpr int UPDATE_INTERVAL = 10;
    };
    class Weather
    {
    public:
        static constexpr const char* UPDATE_URL = "https://api.openweathermap.org/data/2.5/onecall?lat={your_lat}&lon={your_lon}&units=metric&appid={your key}";
        static constexpr int UPDATE_INTERVAL = 10 * 60 * 1000; /* 10 minutes */
    };
}

#endif /* MAIN_H */
