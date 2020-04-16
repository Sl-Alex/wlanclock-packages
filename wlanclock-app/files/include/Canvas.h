#ifndef CANVAS_H
#define CANVAS_H

#include "AbstractCanvas.h"

typedef enum
{
    CANVAS_COLOR_4BIT,
    CANVAS_COLOR_5BIT,
} canvas_color_bit_t;

template<canvas_color_bit_t BPC>
class Canvas: public AbstractCanvas
{
    public:
        Canvas(int width, int height);
        virtual ~Canvas() { delete[] mData; }

        virtual void setPixel(int x, int y, rgba32_t value);
        virtual rgba32_t getPixel(int x, int y);

    private:
};

#endif /* CANVAS_H */

