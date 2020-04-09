#ifndef ABSTRACTCANVAS_H
#define ABSTRACTCANVAS_H

#include <cstdint>

typedef uint32_t rgba32_t;

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

        void drawLine(int x1, int y1, int x2, int y2, rgba32_t value);
        void drawRect(int x1, int y1, int x2, int y2, rgba32_t value);

        int   getSize() { return mSize; }
        char *getData() { return mData; }
    protected:
        int mWidth;
        int mHeight;

        int mSize;
        char *mData;
};

#endif /* ABSTRACTCANVAS_H */
