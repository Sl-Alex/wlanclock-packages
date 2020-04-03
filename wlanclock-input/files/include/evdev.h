#ifndef EVDEV_H
#define EVDEV_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>

typedef struct
{
    char device[LINE_MAX];
    int threshold;
    int timeout;
    int swap_axes;
    int invert_x;
    int invert_y;
} touchpad_config_t;

int evdev_loop(touchpad_config_t *cfg);

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* EVDEV_H */
