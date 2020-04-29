#ifndef ABSTRACTCANVAS_H
#define ABSTRACTCANVAS_H

#include <cstdint>
#include <string>
#include "Config.h"

typedef uint32_t rgba32_t;

typedef struct {
    int x1;
    int y1;
    int x2;
    int y2;
} Area;

class AbstractCanvas
{
    public:
        AbstractCanvas(int width, int height)
            :mWidth(width)
            ,mHeight(height)
            ,mSize(0)
            ,mData(nullptr)
        {}
        virtual ~AbstractCanvas() {};

        virtual void setPixel(int x, int y, rgba32_t value) = 0;
        virtual rgba32_t getPixel(int x, int y) = 0;
        virtual void setPixelRaw(int x, int y, uint32_t value) = 0;
        virtual uint32_t getPixelRaw(int x, int y) = 0;

        void drawLine(int x1, int y1, int x2, int y2, rgba32_t value);
        void drawRect(int x1, int y1, int x2, int y2, rgba32_t value);
        void drawFilledRect(int x1, int y1, int x2, int y2, rgba32_t value);
        int  drawText(Config::Fonts::Params params, std::string text, rgba32_t color);
        int  drawText(Config::Fonts::Params params, std::wstring text, rgba32_t color);
        void copyAreaTo(Area area, AbstractCanvas &canvas, int to_x = 0, int to_y = 0);
        void copyTo(AbstractCanvas &canvas, int to_x = 0, int to_y = 0);
        void clear(void);

        int   getSize() { return mSize; }
        char *getData() { return (char *)mData; }
        int   getWidth() { return mWidth; }
        int   getHeight() { return mHeight; }

    protected:
        int mWidth;
        int mHeight;

        int mSize;
        uint8_t *mData;
};

#endif /* ABSTRACTCANVAS_H */
