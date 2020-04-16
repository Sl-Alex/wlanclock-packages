#include "Canvas.h"
#include <iostream>

/***************************************************************
 * Template instantiations for 4 bit per color
 **************************************************************/
template<>
Canvas<CANVAS_COLOR_4BIT>::Canvas(int width, int height)
    :AbstractCanvas(width, height)
{
    /* 2 bytes per pixel */
    mSize = width*height*2;
    mData = new char[mSize];
    memset(mData, 0, mSize);
    std::cout << "Allocating " << mSize << " bytes for canvas" << std::endl;
}

template<>
rgba32_t Canvas<CANVAS_COLOR_4BIT>::getPixel(int x, int y)
{
    if ((x >= mWidth) || (y >= mHeight) || (x < 0) || (y < 0))
    {
        return 0;
    }
    uint32_t dst_r, dst_g, dst_b;
    uint16_t dst_value = mData[y * mWidth * 2 + x * 2];
    dst_value = dst_value << 8;
    dst_value |= mData[y * mWidth * 2 + x * 2 + 1];

    dst_r = (dst_value >> 8) & 0x0F;
    dst_g = (dst_value >> 4) & 0x0F;
    dst_b = dst_value & 0x0F;
    dst_r = dst_r * 255 / 0x0F;
    dst_g = dst_g * 255 / 0x0F;
    dst_b = dst_b * 255 / 0x0F;

    rgba32_t ret_value = ((uint32_t)dst_r << 24) | ((uint32_t)dst_g << 16) | ((uint32_t)dst_b << 8);
    return ret_value;
}

template<>
void Canvas<CANVAS_COLOR_4BIT>::setPixel(int x, int y, rgba32_t value)
{
    if ((x >= mWidth) || (y >= mHeight) || (x < 0) || (y < 0))
    {
        return;
    }

    rgba32_t dst_value;
    uint32_t dst_r, dst_g, dst_b;
    uint32_t src_r, src_g, src_b, src_a;

    src_r = (value >> 24) & 0xFF;
    src_g = (value >> 16) & 0xFF;
    src_b = (value >>  8) & 0xFF;
    src_a = value & 0xFF;

    dst_value = getPixel(x,y);
    dst_r = (dst_value >> 24) & 0xFF;
    dst_g = (dst_value >> 16) & 0xFF;
    dst_b = (dst_value >>  8) & 0xFF;

    dst_r = (src_r * src_a + dst_r * (255 - src_a))/255;
    dst_g = (src_g * src_a + dst_g * (255 - src_a))/255;
    dst_b = (src_b * src_a + dst_b * (255 - src_a))/255;
    dst_r = dst_r * 0x0F / 255;
    dst_g = dst_g * 0x0F / 255;
    dst_b = dst_b * 0x0F / 255;
    dst_value = (dst_r << 8) | (dst_g << 4) | dst_b;
    mData[y * mWidth * 2 + x * 2] = (dst_value >> 8) & 0xFF;
    mData[y * mWidth * 2 + x * 2 + 1] = dst_value & 0xFF;
}

/***************************************************************
 * Template instantiations for 4 bits per color
 **************************************************************/
template<>
Canvas<CANVAS_COLOR_5BIT>::Canvas(int width, int height)
    :AbstractCanvas(width, height)
{
    std::cerr << "5 bit canvas is not implemented" << std::endl;
}

template<>
rgba32_t Canvas<CANVAS_COLOR_5BIT>::getPixel(int x, int y)
{
    std::cerr << "5 bit canvas is not implemented" << std::endl;
    return 0;
}

template<>
void Canvas<CANVAS_COLOR_5BIT>::setPixel(int x, int y, rgba32_t value)
{
    std::cerr << "5 bit canvas is not implemented" << std::endl;
}
