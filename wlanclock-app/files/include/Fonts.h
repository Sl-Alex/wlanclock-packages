#ifndef FONTS_H
#define FONTS_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <vector>

class Fonts
{
    public:
        struct FontParams {
            unsigned int index;
            unsigned int size_h;
            unsigned int size_v;
            unsigned int base_x;
            unsigned int base_y;
        };

        static Fonts& getInstance()
        {
            // Guaranteed to be destroyed.
            // Instantiated on first use.
            static Fonts instance;

            return instance;
        }

        int init(void);

        FT_Face getFontFace(size_t index);
        int loadFontParams(std::string paramsName, FontParams &params);

        virtual ~Fonts();
        Fonts(Fonts const&)  = delete;
        void operator=(Fonts const&) = delete;
    private:
        Fonts();
        std::string mFontsDir;

        int loadFont(std::string fontName);
        static void uciFonstCb(char* value);

        FT_Library  mFtLibrary;
        std::vector<FT_Face> mFtFaces;

        static constexpr const char* CONFIG_FILE          = "wlanclock";
        static constexpr const char* CONFIG_SECTION       = "app";
        static constexpr const char* CONFIG_KEY_FONTS_DIR = "fonts_dir";
        static constexpr const char* CONFIG_KEY_FONTS     = "fonts";

        static constexpr const char* FONT_KEY_INDEX       = "index";
        static constexpr const char* FONT_KEY_SIZE_H      = "size_h";
        static constexpr const char* FONT_KEY_SIZE_V      = "size_v";
        static constexpr const char* FONT_KEY_BASE_X      = "base_x";
        static constexpr const char* FONT_KEY_BASE_Y      = "base_y";

};

#endif /* FONTS_H */

