#ifndef MENU_H
#define MENU_H

#include "AbstractCanvas.h"
#include "AbstractDisplayInterface.h"
#include "SysTimer.h"
#include "UbusServer.h"
#include "Fonts.h"
#include "Animator.h"
#include "Weather.h"
#include <mutex>

class IMenuInteraction
{
  public:
    typedef enum
    {
        MENUACTION_EXIT = 0
    } MenuAction;
    virtual ~IMenuInteraction() {};
    virtual void onMenuAction(MenuAction action) = 0;
};

#endif /* MENU_H */
