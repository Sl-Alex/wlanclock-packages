#include "Fonts.h"
#include "Config.h"
#include <cstring>
#include <unistd.h>
#include <iostream>

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
    for (size_t i = 0; i < sizeof(Config::Fonts::FONTS)/sizeof(Config::Fonts::FONTS[0]); i++)
    {
        std::string fontName = Config::Fonts::FONTS[i];
        Fonts::getInstance().loadFont(fontName);
    }
    return 0;
}

FT_Face Fonts::getFontFace(size_t index)
{
    if (index >= mFtFaces.size())
    {
        return nullptr;
    }
    return mFtFaces[index];
}

int Fonts::loadFont(std::string &fontName)
{
    std::string path = std::string(Config::Fonts::FONTS_DIR)+ fontName;
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

