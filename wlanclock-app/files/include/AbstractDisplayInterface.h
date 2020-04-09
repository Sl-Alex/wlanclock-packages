#ifndef ABSTRACTDISPLAYINTERFACE_H
#define ABSTRACTDISPLAYINTERFACE_H

#include "AbstractCanvas.h"

class AbstractDisplayInterface
{
    public:
        virtual ~AbstractDisplayInterface() {};

        /* Start communication loop */
        virtual int start(void) = 0;

        virtual void setBrightness(int brightness) = 0;
        virtual void update(AbstractCanvas &canvas) = 0;
};

#endif /* ABSTRACTDISPLAYINTERFACE_H */
