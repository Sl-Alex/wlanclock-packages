#include "Fonts.h"
#include "AbstractCanvas.h"
#include <cstdlib>
#include <utility>
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H

void AbstractCanvas::clear()
{
    memset(mData, 0, mSize);
}

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

void AbstractCanvas::drawFilledRect(int x1, int y1, int x2, int y2, rgba32_t value)
{
    if (x1 > x2)
    {
        int x = x2;
        x2 = x1;
        x1 = x;
    }

    for (int x = x1; x <= x2; x++)
    {
        drawLine(x,y1, x,y2, value);
    }
}

int AbstractCanvas::drawText(Config::Fonts::Params params, std::string text, rgba32_t color)
{
    std::wstring wtext(text.begin(), text.end());
    return drawText(params, wtext, color);
}

int AbstractCanvas::drawText(Config::Fonts::Params params, std::wstring text, rgba32_t color)
{
    FT_Face face = Fonts::getInstance().getFontFace(params.index);
    FT_Set_Pixel_Sizes(face, params.size_h, params.size_v);
    int off_x = 0;
    int off_y = 0;
    for (size_t i = 0; i < text.length(); i++)
    {
        unsigned int glyph_index = FT_Get_Char_Index( face, text[i] );
        int rc = FT_Load_Glyph(
                  face,          /* handle to face object */
                  glyph_index,   /* glyph index           */
                  FT_LOAD_DEFAULT );
        if (rc)
        {
            std::cout << "Can't load glyph, rc = " << rc << std::endl;
            return rc;
        }
        rc = FT_Render_Glyph( face->glyph,   /* glyph slot  */
                                 FT_RENDER_MODE_NORMAL ); /* render mode */
        unsigned char *pdata = face->glyph->bitmap.buffer;

        off_y = face->glyph->bitmap_top;

        unsigned int horiBearX = face->glyph->metrics.horiBearingX >> 6;

        for (unsigned int y = 0; y < face->glyph->bitmap.rows; y++)
        {
            for (unsigned int x = 0; x < face->glyph->bitmap.width; x++)
            {
                setPixel(params.base_x + x + off_x + horiBearX, params.base_y + y - off_y, (color & 0xFFFFFF00) | *pdata);
                pdata++;
            }
        }
        off_x += (face->glyph->advance.x >> 6);
    }

    return 0;
}

void AbstractCanvas::copyAreaTo(Area area, AbstractCanvas &canvas, int to_x, int to_y)
{
    for (int y = area.y1; y <= area.y2; y++)
    {
        for (int x = area.x1; x <= area.x2; x++)
        {
            canvas.setPixelRaw(to_x + x - area.x1, to_y + y - area.y1, getPixelRaw(x,y));
        }
    }
}

void AbstractCanvas::copyTo(AbstractCanvas &canvas, int to_x, int to_y)
{
    copyAreaTo((Area){.x1 = 0, .y1 = 0, .x2 = mWidth - 1, .y2 = mHeight - 1}, canvas, to_x, to_y);
}
