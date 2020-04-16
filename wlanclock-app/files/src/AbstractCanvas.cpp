#include "Fonts.h"
#include "AbstractCanvas.h"
#include <cstdlib>
#include <algorithm>
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

int AbstractCanvas::drawText(int x1, int y1, int fontIndex, int size_h, int size_v, std::string text)
{
    FT_Face face = Fonts::getInstance().getFontFace(fontIndex);
    FT_Set_Pixel_Sizes(face, size_h, size_v);
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
                setPixel(x1 + x + off_x + horiBearX, y1 + y - off_y, 0xFF800000 | *pdata);
                pdata++;
            }
        }
        off_x += (face->glyph->advance.x >> 6);
    }

    return 0;
}
