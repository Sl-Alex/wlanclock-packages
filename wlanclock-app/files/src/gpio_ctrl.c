#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

#include "gpio_ctrl.h"

#include <dirent.h>
#include <errno.h>

#define GPIO_PREFIX    "/sys/class/gpio/"
#define GPIO_EXPORT    "export"
#define GPIO_DIRECTION "direction"
#define GPIO_VALUE     "value"

#define GPIO_DIRECTION_IN  "in"
#define GPIO_DIRECTION_OUT "out"

#define GPIO_OUT_HIGH  "1"
#define GPIO_OUT_LOW   "0"

static char *get_gpio_folder(int gpio_nr)
{
    static char gpio_path[LINE_MAX];
    int rc = snprintf((char *)gpio_path, sizeof(gpio_path) - 1, "%sgpio%d", GPIO_PREFIX, gpio_nr);
    if ((rc < 0) || (rc > sizeof(gpio_path)))
    {
        return NULL;
    }
    return (char *)gpio_path;
}

int gpio_ctrl_export(int gpio_nr)
{
    char gpio_nr_str[4];

    if ((gpio_nr < 0) || (gpio_nr > 999))
    {
        return 1;
    }

    int rc = snprintf(gpio_nr_str, sizeof(gpio_nr_str) - 1, "%d", gpio_nr);
    if ((rc < 0) || (rc > sizeof(gpio_nr_str)))
    {
        return 1;
    }

    /* Check if gpio export was done before */
    char *gpio_path = get_gpio_folder(gpio_nr);
    DIR* dir = opendir(gpio_path);
    if (!dir)
    {
        if (ENOENT != errno)
        {
            return errno;
        }

        /* Export */
        int fd = open(GPIO_PREFIX GPIO_EXPORT, O_WRONLY);
        if (fd == -1) {
            fprintf(stderr, "Unable to open " GPIO_PREFIX GPIO_EXPORT "\n");
            return 1;
        }

        if (write(fd, gpio_nr_str, 2) != 2) {
            fprintf(stderr, "Error writing to " GPIO_PREFIX GPIO_EXPORT "\n");
            return 1;
        }

        close(fd);
    }
    else
    {
        closedir(dir);
    }
    return 0;
}

int gpio_ctrl_init(int gpio_nr)
{
    return gpio_ctrl_export(gpio_nr);
}

int gpio_ctrl_set_dir(int gpio_nr, gpio_ctrl_dir_t dir)
{
    char gpio_path[LINE_MAX];

    int rc = snprintf((char *)gpio_path, sizeof(gpio_path) - 1, "%sgpio%d/%s", GPIO_PREFIX, gpio_nr, GPIO_DIRECTION);
    if ((rc < 0) || (rc > sizeof(gpio_path)))
    {
        return 1;
    }

    /* Change direction */
    int fd = open(gpio_path, O_WRONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Unable to open %s\n", gpio_path);
        return 1;
    }

    if (dir == GPIO_CTRL_DIR_IN)
    {
        if (write(fd, GPIO_DIRECTION_IN, strlen(GPIO_DIRECTION_IN)) != strlen(GPIO_DIRECTION_IN))
        {
            fprintf(stderr, "Error writing to %s\n", gpio_path);
            return 1;
        }
    }
    else
    {
        if (write(fd, GPIO_DIRECTION_OUT, strlen(GPIO_DIRECTION_OUT)) != strlen(GPIO_DIRECTION_OUT))
        {
            fprintf(stderr, "Error writing to %s\n", gpio_path);
            return 1;
        }
    }


    close(fd);

    return 0;
}

int gpio_ctrl_set_value(int gpio_nr, int value)
{
    char gpio_path[LINE_MAX];

    int rc = snprintf((char *)gpio_path, sizeof(gpio_path) - 1, "%sgpio%d/%s", GPIO_PREFIX, gpio_nr, GPIO_VALUE);
    if ((rc < 0) || (rc > sizeof(gpio_path)))
    {
        return 1;
    }

    /* Change value */
    int fd = open(gpio_path, O_WRONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Unable to open %s\n", gpio_path);
        return 1;
    }

    if (value)
    {
        if (write(fd, GPIO_OUT_HIGH, strlen(GPIO_OUT_HIGH)) != strlen(GPIO_OUT_HIGH))
        {
            fprintf(stderr, "Error writing to %s\n", gpio_path);
            return 1;
        }
    }
    else
    {
        if (write(fd, GPIO_OUT_LOW, strlen(GPIO_OUT_LOW)) != strlen(GPIO_OUT_LOW))
        {
            fprintf(stderr, "Error writing to %s\n", gpio_path);
            return 1;
        }
    }


    close(fd);

    return 0;
}
