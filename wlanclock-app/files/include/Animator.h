#ifndef SM_ANIMATOR_H
#define SM_ANIMATOR_H

#include "AbstractCanvas.h"

/// @brief GUI canvas/image animator class
class Animator
{
public:
    /// @brief Animation type
    typedef enum {
        ANIM_TYPE_OPACITY,  /* Increase FG opacity */
        ANIM_TYPE_SLIDE,    /* Slide FG over BG */
        ANIM_TYPE_SLIDE_BG, /* Slide BG over FG */
        ANIM_TYPE_SHIFT     /* Shift FG in while shifting BG out */
    } AnimType;

    /// @brief Animation direction
    typedef enum {
        ANIM_DIR_UP,    ///< Animate to the top
        ANIM_DIR_DOWN,  ///< Animate to the bottom
        ANIM_DIR_LEFT,  ///< Animate to the left
        ANIM_DIR_RIGHT  ///< Animate to the right
    } AnimDir;

    /// @brief Constructor
    Animator()
        :mTick(0),
        mRunning(false)
    {}

    /* Set foreground, background and destination canvas */
    inline void setFgBgDest(
            AbstractCanvas &foreground,
            AbstractCanvas &background,
            AbstractCanvas &destination
            )
    {
        pDest = &destination;
        pForeground = &foreground;
        pBackground = &background;
        mArea.x1 = 0; mArea.y1 = 0;
        mArea.x2 = pDest->getWidth() - 1;
        mArea.y2 = pDest->getHeight() - 1;
    }
    /// @brief Set animation type
    inline void setType(AnimType type) {mAnimType = type;}
    /// @brief Set animation speed
    inline void setSpeed(int speed) {mSpeed = speed;}
    /// @brief Set animation direction
    inline void setDir(AnimDir dir) {mAnimDir = dir;}
    inline void setArea(Area area) {mArea = area;}
    void start();
    /// @brief Display a single animation tick
    /// @returns False when animation is done
    bool tick(void);
    /// @brief Finalize animation immediately
    void finish(void);
    /// @brief Returns true when animation is running
    inline bool isRunning(void) { return mRunning; }

private:
    AbstractCanvas * pBackground;
    AbstractCanvas * pForeground;
    AbstractCanvas * pDest;

    AnimType mAnimType;
    AnimDir  mAnimDir;
    Area     mArea;

    int mXfg;
    int mYfg;

    int mSpeed;
    int mTick;
    int mLimit;

    bool mRunning;
};

#endif // SM_ANIMATOR_H
