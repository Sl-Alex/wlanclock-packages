#ifndef ABSTRACTCANVAS_H
#define ABSTRACTCANVAS_H

#include <cstdint>
#include <string>

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
        int  drawText(int x1, int y1, int fontIndex, int size_h, int size_v, std::string text, rgba32_t color);
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
