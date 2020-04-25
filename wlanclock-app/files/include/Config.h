#ifndef CONFIG_H
#define CONFIG_H

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
    class Fonts
    {
        public:
            struct Params
            {
                unsigned int index;
                unsigned int base_x;
                unsigned int base_y;
                unsigned int size_h;
                unsigned int size_v;
            };
            enum {
                FONT_HOURS_MINS = 0,
                FONT_SECONDS = 1,
                FONT_WEATHER = 2
            };
            static constexpr const char* FONTS_DIR  = "/usr/share/wlanclock/fonts/";
            static constexpr const char* IMAGES_DIR = "/usr/share/wlanclock/images/";
            static constexpr const char* FONTS[] = {"NanumGothic-ExtraBold.ttf"};
            static const Params PARAMS[];
    };
    class Weather
    {
        public:
            static constexpr const char* UPDATE_URL = "https://api.openweathermap.org/data/2.5/onecall?lat={your_lat}&lon={your_lon}&units=metric&appid={your_key}";
            static constexpr int UPDATE_INTERVAL = 10 * 60 * 1000; /* 10 minutes */
    };
}

#endif /* CONFIG_H */
