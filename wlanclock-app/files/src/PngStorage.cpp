/*
 * A simple libpng example program
 * http://zarb.org/~gc/html/libpng.html
 *
 * Modified by Yoshimasa Niwa to make it much simpler
 * and support all defined color_type.
 *
 * Wrapped in C++ class by Oleksii Slabchenko (Sl-Alex)
 *
 * Copyright 2002-2010 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

#include "PngStorage.h"
#include "Canvas.h"
#include "Config.h"
#include <iostream>


#include <stdlib.h>
#include <stdio.h>
#include <png.h>

int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers = NULL;

PngStorage::PngStorage()
{
}

PngStorage::~PngStorage()
{
    for (auto it = mPngStorage.begin(); it != mPngStorage.end(); it++)
    {
        delete it->canvas;
    }
}

int PngStorage::loadPng(PngElement &element)
{
    int rc = 1;
    png_structp png;
    png_infop info;
    FILE *fp = fopen(element.name.c_str(), "rb");
    if (fp == nullptr)
        return 1;

    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png)
        goto FCLOSE;

    info = png_create_info_struct(png);
    if(!info)
        goto DESTROY_READ_STRUCT;

    if(setjmp(png_jmpbuf(png)))
        goto DESTROY_READ_STRUCT;

    png_init_io(png, fp);

    png_read_info(png, info);

    width      = png_get_image_width(png, info);
    height     = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth  = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if(bit_depth == 16)
      png_set_strip_16(png);

    if(color_type == PNG_COLOR_TYPE_PALETTE)
      png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
      png_set_expand_gray_1_2_4_to_8(png);

    if(png_get_valid(png, info, PNG_INFO_tRNS))
      png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xFF.
    if(color_type == PNG_COLOR_TYPE_RGB ||
       color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_PALETTE)
      png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if(color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for(int y = 0; y < height; y++) {
      row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    }

    png_read_image(png, row_pointers);

    element.canvas = new Canvas<CANVAS_COLOR_4BIT>(width, height);
    for(int y = 0; y < height; y++) {
      png_bytep row = row_pointers[y];
      for(int x = 0; x < width; x++) {
        png_bytep px = &(row[x * 4]);
        rgba32_t pix = ((rgba32_t)px[0] << 24) | ((rgba32_t)px[1] << 16)  | ((rgba32_t)px[2] << 8) | (rgba32_t)px[3] ;
        element.canvas->setPixel(x,y, pix);
      }
    }

    for(int y = 0; y < height; y++) {
      free(row_pointers[y]);
    }
    free(row_pointers);

    rc = 0;

DESTROY_READ_STRUCT:
    png_destroy_read_struct(&png, &info, NULL);

FCLOSE:
    fclose(fp);
    return rc;
}

void PngStorage::init()
{
    std::string imagesPath = Config::Images::IMAGES_DIR;
    for (size_t i = 0; i < sizeof(Config::Images::IMAGES)/sizeof(Config::Images::IMAGES[0]); i++)
    {
        loadPng(std::string(Config::Images::IMAGES[i]));
    }
}

int PngStorage::loadPng(std::string filename)
{
    std::string imagesPath = Config::Images::IMAGES_DIR;
    filename = imagesPath + filename;
    for (auto it = mPngStorage.begin(); it != mPngStorage.end(); it++)
    {
        if (it->name == filename)
        {
            std::cerr << filename << "is already loaded, skipping" << std::endl;
            return 1;
        }
    }
    /* load PNG */
    PngElement element;
    element.canvas = nullptr;
    element.name = filename;
    int rc = loadPng(element);
    if (rc)
    {
        delete element.canvas;
        std::cerr << "Can't load " << filename << ", skipping" << std::endl;
    }
    mPngStorage.push_back(element);
    return rc;
}

AbstractCanvas *PngStorage::getCanvas(std::string filename)
{
    std::string imagesPath = Config::Images::IMAGES_DIR;
    filename = imagesPath + filename;
    for (auto it = mPngStorage.begin(); it != mPngStorage.end(); it++)
    {
        if (it->name == filename)
        {
            return it->canvas;
        }
    }
    return nullptr;
}
