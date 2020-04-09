#ifndef GPIO_CTRL_H
#define GPIO_CTRL_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    GPIO_CTRL_DIR_IN = 0,
    GPIO_CTRL_DIR_OUT
} gpio_ctrl_dir_t;

int gpio_ctrl_init(int gpio_nr);
int gpio_ctrl_set_dir(int gpio_nr, gpio_ctrl_dir_t dir);
int gpio_ctrl_set_value(int gpio_nr, int value);

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* GPIO_CTRL_H */
