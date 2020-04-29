#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace Config
{
    class Display
    {
        public:
            static constexpr int WIDTH           = 128;
            static constexpr int HEIGHT          = 32;
            static constexpr int UPDATE_INTERVAL = 10;
            static constexpr int MENU_TIMEOUT    = 15 * 1000;
            static uint32_t      color;
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
            static constexpr const char* FONTS[] = {"NanumGothic-ExtraBold.ttf"};
            static const Params PARAMS[];
    };
    class Images
    {
        public:
            static constexpr const char* IMAGES_DIR = "/usr/share/wlanclock/images/";
            static const char* IMAGES[20];
    };
    class Weather
    {
        public:
            static constexpr int UPDATE_INTERVAL = 10 * 60 * 1000; /* 10 minutes */
    };
    class UciPaths
    {
        public:
            /* SPI interface settings */
            static constexpr const char* SPIDEV = "wlanclock.app.device";
            static constexpr const char* GPIO   = "wlanclock.app.dat_ncfg_gpio";
            /* Weather URL */
            static constexpr const char* WEATHER_URL = "wlanclock.app.weather_url";
            /* Main clock color */
            static constexpr const char* COLOR  = "wlanclock.app.color";
    };
}

#endif /* CONFIG_H */
