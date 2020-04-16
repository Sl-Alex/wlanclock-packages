#ifndef DEKSTOP_H
#define DEKSTOP_H

#include "AbstractCanvas.h"
#include "AbstractDisplayInterface.h"
#include "SysTimer.h"
#include "UbusServer.h"
#include "Fonts.h"

class Desktop: public ISysTimer, public IGestureReceiver
{
    public:
        Desktop(AbstractDisplayInterface &displayIface);
        ~Desktop();

        int init(void);

    private:
        void onTimer();
        void onGesture(uint32_t gesture);

        static constexpr int DESKTOP_WIDTH     = 128;
        static constexpr int DESKTOP_HEIGHT    = 32;

        const std::string FONT_HOURS_MINS = "HOURS_MINS";
        const std::string FONT_SECONDS    = "SECONDS";

        AbstractCanvas *mBgCanvas;
        AbstractDisplayInterface & mDisplayInterface;
        Fonts::FontParams mHourMinParams;
        Fonts::FontParams mSecParams;
};

#endif /* DESKTOP_H */
