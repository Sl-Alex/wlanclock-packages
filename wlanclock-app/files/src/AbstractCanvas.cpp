#include "AbstractCanvas.h"
#include <cstdlib>
#include <algorithm>
#include <ft2build.h>
#include FT_FREETYPE_H

void AbstractCanvas::drawLine(int x1, int y1, int x2, int y2, rgba32_t value)
{
    /* integer part of x */
    auto iPartOfNumber = [](float x){
        return (int)x;
    };
    /* fractional part of x */
    auto fPartOfNumber = [iPartOfNumber](float x)
    {
        return x - iPartOfNumber(x);
    };
    auto rfPartOfNumber = [fPartOfNumber](float x)
    {
        return 1 - fPartOfNumber(x);
    };

    /* Calculate the steep */
    int steep = std::abs(y2 - y1) > std::abs(x2 - x1) ;

    /* swap the coordinates if delta y is more that delta x */
    if (steep)
    {
        std::swap(x1 , y1);
        std::swap(x2 , y2);
    }
    if (x1 > x2)
    {
        std::swap(x1 ,x2);
        std::swap(y1 ,y2);
    }

    /* prepare parameters */
    float dx = x2-x1;
    float dy = y2-y1;
    float gradient = dy/dx;
    if (dx == 0.0)
        gradient = 1;

    int xpxl1 = x1;
    int xpxl2 = x2;
    float interY = y1;

    if (steep)
    {
        for (int x = xpxl1 ; x <= xpxl2 ; x++)
        {
            int a_value = (float)(value & 0xFF)*rfPartOfNumber(interY);
            int set_value = value & 0xFFFFFF00;
            set_value |= (int)a_value & 0xFF;
            if (a_value)
                setPixel(iPartOfNumber(interY), x, set_value);
            a_value = (float)(value & 0xFF)*fPartOfNumber(interY);
            set_value = value & 0xFFFFFF00;
            set_value |= (int)a_value & 0xFF;
            if (a_value)
               setPixel(iPartOfNumber(interY)+1, x, set_value);
            interY += gradient;
        }
    }
    else
    {
        for (int x = xpxl1 ; x <= xpxl2 ; x++)
        {
            int a_value = (float)(value & 0xFF)*rfPartOfNumber(interY);
            int set_value = value & 0xFFFFFF00;
            set_value |= (int)a_value & 0xFF;
            if (a_value)
                setPixel(x, iPartOfNumber(interY), set_value);
            a_value = (float)(value & 0xFF)*fPartOfNumber(interY);
            set_value = value & 0xFFFFFF00;
            set_value |= (int)a_value & 0xFF;
            if (a_value)
                setPixel(x, iPartOfNumber(interY)+1, set_value);
            interY += gradient;
        }
    }
}

void AbstractCanvas::drawRect(int x1, int y1, int x2, int y2, rgba32_t value)
{
    drawLine(x1,y1, x2,y1, value);
    if (std::abs(y2-y1) > 1)
    {
        drawLine(x1,y1+1, x1,y2-1, value);
        drawLine(x2,y1+1, x2,y2-1, value);
    }
    drawLine(x1,y2, x2,y2, value);
}

int AbstractCanvas::drawText(int x1, int y1, std::string font, int sz, std::string text)
{
    FT_Library  library;
    int rc = FT_Init_FreeType(&library);
    if (rc != 0)
    {
        return rc;
    }

    FT_Done_FreeType( library );
    return 0;
}
