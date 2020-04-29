#include "Config.h"

namespace Config
{
    const Fonts::Params Fonts::PARAMS[] = {
            /* FONT_HOURS_MINS */
            {0,  2, 28, 34, 38},
            /* FONT_SECONDS */
            {0, 97, 29, 24, 24},
            /* FONT_WEATHER */
            {0, 36, 10, 12, 12}
    };
    uint32_t Display::color;
    const char * Images::IMAGES[20] = {
            "refresh.png",
            "house.png",
            "weather/01d.png",
            "weather/01n.png",
            "weather/02d.png",
            "weather/02n.png",
            "weather/03d.png",
            "weather/03n.png",
            "weather/04d.png",
            "weather/04n.png",
            "weather/09d.png",
            "weather/09n.png",
            "weather/10d.png",
            "weather/10n.png",
            "weather/11d.png",
            "weather/11n.png",
            "weather/13d.png",
            "weather/13n.png",
            "weather/50d.png",
            "weather/50n.png"
    };

}
