#ifndef FONTS_H
#define FONTS_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <vector>

class Fonts
{
    public:

        static Fonts& getInstance()
        {
            // Guaranteed to be destroyed.
            // Instantiated on first use.
            static Fonts instance;

            return instance;
        }

        int init(void);

        FT_Face getFontFace(size_t index);

        virtual ~Fonts();
        Fonts(Fonts const&)  = delete;
        void operator=(Fonts const&) = delete;
    private:
        Fonts();

        int loadFont(std::string fontName);

        FT_Library  mFtLibrary;
        std::vector<FT_Face> mFtFaces;
};

#endif /* FONTS_H */

