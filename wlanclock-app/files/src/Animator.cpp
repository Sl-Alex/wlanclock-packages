#include "Animator.h"
#include <iostream>

void Animator::start()
{
    mTick = 0;
    mXfg = 0;
    mYfg = 0;
    switch(mAnimDir)
    {
        case ANIM_DIR_UP:
            mYfg = -pDest->getHeight();
            break;
        case ANIM_DIR_DOWN:
            mYfg = pDest->getHeight();
            break;
        case ANIM_DIR_LEFT:
            mXfg = -pDest->getWidth();
            break;
        case ANIM_DIR_RIGHT:
            mXfg = pDest->getWidth();
            break;
        default:
            std::cerr << "Animation dir not implemented: " << mAnimDir << std::endl;
            mRunning = false;
    }
    if ((mAnimType == ANIM_TYPE_SHIFT) || (mAnimType == ANIM_TYPE_OPACITY))
    {
        memcpy(pBackground->getData(), pDest->getData(), pBackground->getSize());
    }
    if (mAnimType == ANIM_TYPE_SLIDE_BG)
    {
        /* Foreground goes to back, dest goes to front */
        memcpy(pBackground->getData(), pForeground->getData(), pBackground->getSize());
        memcpy(pForeground->getData(), pDest->getData(),       pForeground->getSize());
    }
    mRunning = true;
}

bool Animator::tick()
{
    if (! mRunning)
        return mRunning;

    int width  = pDest->getWidth();
    int height = pDest->getHeight();
    switch (mAnimType)
    {
        case ANIM_TYPE_OPACITY:
            pDest->clear();
            for (int y = mArea.y1; y <= mArea.y2; y++)
            {
                for (int x = mArea.x1; x <= mArea.x2; x++)
                {
                    rgba32_t fg_pix = pForeground->getPixel(x,y);
                    uint32_t bg_pix = pBackground->getPixelRaw(x,y);
                    fg_pix &= 0xFFFFFF00;
                    fg_pix |= mTick;
                    pDest->setPixelRaw(x,y,bg_pix);
                    pDest->setPixel(x,y,fg_pix);
                }
            }
            if (mTick == 255)
            {
                mRunning = false;
            }
            else
            {
                mTick+=20;
                if(mTick >255)
                    mTick = 255;
            }
            break;
        case ANIM_TYPE_SLIDE_BG:
            /* Step 1. Update offset according to direction */
            switch(mAnimDir)
            {
                case ANIM_DIR_UP:
                    mYfg += mSpeed;
                    if (mYfg > 0) mYfg = 0;
                    break;
                case ANIM_DIR_DOWN:
                    mYfg -= mSpeed;
                    if (mYfg < 0) mYfg = 0;
                    break;
                case ANIM_DIR_LEFT:
                    mXfg += mSpeed;
                    if (mXfg > 0) mXfg = 0;
                    break;
                case ANIM_DIR_RIGHT:
                    mXfg -= mSpeed;
                    if (mXfg < 0) mXfg = 0;
                    break;
                default:
                    std::cerr << "Animation dir not implemented: " << mAnimDir << std::endl;
                    mRunning = false;
            }
            /* Step 2. Update pDest */
            for (int y = mArea.y1; y <= mArea.y2; y++)
            {
                for (int x = mArea.x1; x <= mArea.x2; x++)
                {
                    uint32_t pix;
                    if ((x + mXfg < 0) || (x + mXfg >= width) || (y + mYfg < 0) || (y + mYfg >= height))
                    {
                        switch(mAnimDir)
                        {
                            case ANIM_DIR_DOWN:
                                pix = pForeground->getPixelRaw(x,y-height+mYfg);
                                break;
                            case ANIM_DIR_UP:
                                pix = pForeground->getPixelRaw(x,y+height+mYfg);
                                break;
                            case ANIM_DIR_RIGHT:
                                pix = pForeground->getPixelRaw(x-width+mXfg,y);
                                break;
                            case ANIM_DIR_LEFT:
                                pix = pForeground->getPixelRaw(x+width+mXfg,y);
                                break;
                        }
                        pDest->setPixelRaw(x,y,pix);
                    }
                    else
                    {
                        pix = pBackground->getPixelRaw(x,y);
                        pDest->setPixelRaw(x,y,pix);
                    }
                }
            }
            if ((mXfg == 0) && (mYfg == 0))
            {
                mRunning = false;
            }
            break;
        /* Intentional fall-through */
        case ANIM_TYPE_SLIDE:
        case ANIM_TYPE_SHIFT:
            /* Step 1. Update offset according to direction */
            switch(mAnimDir)
            {
                case ANIM_DIR_UP:
                    mYfg += mSpeed;
                    if (mYfg > 0) mYfg = 0;
                    break;
                case ANIM_DIR_DOWN:
                    mYfg -= mSpeed;
                    if (mYfg < 0) mYfg = 0;
                    break;
                case ANIM_DIR_LEFT:
                    mXfg += mSpeed;
                    if (mXfg > 0) mXfg = 0;
                    break;
                case ANIM_DIR_RIGHT:
                    mXfg -= mSpeed;
                    if (mXfg < 0) mXfg = 0;
                    break;
                default:
                    std::cerr << "Animation dir not implemented: " << mAnimDir << std::endl;
                    mRunning = false;
            }
            /* Step 2. Update pDest */
            for (int y = mArea.y1; y <= mArea.y2; y++)
            {
                for (int x = mArea.x1; x <= mArea.x2; x++)
                {
                    uint32_t pix;
                    if ((x + mXfg < 0) || (x + mXfg >= width) || (y + mYfg < 0) || (y + mYfg >= height))
                    {
                        if (mAnimType == ANIM_TYPE_SHIFT)
                        {
                            switch(mAnimDir)
                            {
                                case ANIM_DIR_DOWN:
                                    pix = pBackground->getPixelRaw(x,y-height+mYfg);
                                    break;
                                case ANIM_DIR_UP:
                                    pix = pBackground->getPixelRaw(x,y+height+mYfg);
                                    break;
                                case ANIM_DIR_RIGHT:
                                    pix = pBackground->getPixelRaw(x-width+mXfg,y);
                                    break;
                                case ANIM_DIR_LEFT:
                                    pix = pBackground->getPixelRaw(x+width+mXfg,y);
                                    break;
                            }
                            pDest->setPixelRaw(x,y,pix);
                        }
                    }
                    else
                    {
                        pix = pForeground->getPixelRaw(x+mXfg,y+mYfg);
                        pDest->setPixelRaw(x,y,pix);
                    }
                }
            }
            if ((mXfg == 0) && (mYfg == 0))
            {
                mRunning = false;
            }
            break;
        default:
            std::cerr << "Animation type not implemented: " << mAnimType << std::endl;
            mRunning = false;
    }

    return mRunning;
}

void Animator::finish(void)
{
    if (! mRunning)
        return;

    switch (mAnimType)
    {
        case ANIM_TYPE_OPACITY:
            mTick = 255;
            break;
        /* Intentional fall-through */
        case ANIM_TYPE_SLIDE:
        case ANIM_TYPE_SLIDE_BG:
        case ANIM_TYPE_SHIFT:
            mXfg = 0;
            mYfg = 0;
            break;
    }
    tick();
    mRunning = false;
}
