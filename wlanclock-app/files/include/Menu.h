#ifndef MENU_H
#define MENU_H

#include "UbusServer.h"

class IMenuInteraction
{
  public:
    typedef enum
    {
        MENUACTION_EXIT = 0
    } MenuAction;
    virtual ~IMenuInteraction() {};
    virtual void onMenuAction(IMenuInteraction::MenuAction action, IGestureReceiver::Gesture gesture) = 0;
};

#endif /* MENU_H */
