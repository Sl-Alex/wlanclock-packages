#include "Fonts.h"
#include <cstring>
#include <unistd.h>
#include <iostream>

#include "uci_reader.h"

Fonts::Fonts()
{
    int rc = FT_Init_FreeType(&mFtLibrary);
    if (rc != 0)
    {
        std::cout << "Can't initialize freetype library" << std::endl;
    }
}

Fonts::~Fonts()
{
    for(auto it = mFtFaces.begin(); it != mFtFaces.end(); ++it)
    {
        FT_Done_Face(*it);
    }
    FT_Done_FreeType(mFtLibrary);
}

int Fonts::init()
{
    char *ret = uci_reader_get(CONFIG_FILE, CONFIG_SECTION, CONFIG_KEY_FONTS_DIR);
    if (!ret)
    {
        return 1;
    }
    return uci_reader_get_list(CONFIG_FILE, CONFIG_SECTION, CONFIG_KEY_FONTS, uciFonstCb);
}

void Fonts::uciFonstCb(char* value)
{
    Fonts::getInstance().loadFont(value);
}

FT_Face Fonts::getFontFace(size_t index)
{
    if (index >= mFtFaces.size())
    {
        return nullptr;
    }
    return mFtFaces[index];
}

int Fonts::loadFontParams(std::string paramsName, FontParams &params)
{
    char *value;
    value = uci_reader_get(CONFIG_FILE, paramsName.c_str(), FONT_KEY_INDEX);
    if (!value)
        return 1;
    params.index = std::stoi(value);

    value = uci_reader_get(CONFIG_FILE, paramsName.c_str(), FONT_KEY_SIZE_H);
    if (!value)
        return 1;
    params.size_h = std::stoi(value);

    value = uci_reader_get(CONFIG_FILE, paramsName.c_str(), FONT_KEY_SIZE_V);
    if (!value)
        return 1;
    params.size_v = std::stoi(value);

    value = uci_reader_get(CONFIG_FILE, paramsName.c_str(), FONT_KEY_BASE_X);
    if (!value)
        return 1;
    params.base_x = std::stoi(value);

    value = uci_reader_get(CONFIG_FILE, paramsName.c_str(), FONT_KEY_BASE_Y);
    if (!value)
        return 1;
    params.base_y = std::stoi(value);
    return 0;
}

int Fonts::loadFont(std::string fontName)
{
    std::string path = mFontsDir + fontName;
    FT_Face face;
    int rc = FT_New_Face( mFtLibrary,
            path.c_str(),
            0,
            &face );
    if (rc)
    {
        std::cout << "Can't load font " << path << ", err = " << rc << std::endl;
        return rc;
    }
    mFtFaces.push_back(face);
    return rc;
}

