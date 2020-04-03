/*
 * Copyright © 2013 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

//#include "config.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <linux/input.h>
#include "libevdev/libevdev.h"
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#include "evdev.h"
#include "uci_reader.h"

#define CONFIG_FILE     "wlanclock"
#define CONFIG_SECTION  "input"

touchpad_config_t cfg = {
    .device = "",
    .threshold = 30,
    .timeout = 50,
    .swap_axes = 0,
    .invert_x = 0,
    .invert_y = 0
};

#define CONFIG_KEY_DEVICE    "device"
#define CONFIG_KEY_THRESHOLD "threshold"
#define CONFIG_KEY_TIMEOUT   "timeout"
#define CONFIG_KEY_SWAP_AXES "swap_axes"
#define CONFIG_KEY_INVERT_X  "invert_x"
#define CONFIG_KEY_INVERT_Y  "invert_y"

int
main(int argc, char **argv)
{
    touchpad_config_t cfg;
    int *int_key;

    if (argc < 2)
    {
        char *device = uci_reader_get(CONFIG_FILE, CONFIG_SECTION, CONFIG_KEY_DEVICE);
        if (!device)
        {
            fprintf(stderr, CONFIG_KEY_DEVICE "device is not specified in the config file\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            strcpy(cfg.device, device);
        }
    }
    else
    {
        strcpy(cfg.device, argv[1]);
    }

    int_key = uci_reader_get_int(CONFIG_FILE, CONFIG_SECTION, CONFIG_KEY_THRESHOLD);
    if (!int_key)
    {
        fprintf(stderr, CONFIG_KEY_THRESHOLD " is not specified in the config file, use default\n");
    }
    else
    {
        cfg.threshold = *int_key;
    }

    int_key = uci_reader_get_int(CONFIG_FILE, CONFIG_SECTION, CONFIG_KEY_TIMEOUT);
    if (!int_key)
    {
        fprintf(stderr, CONFIG_KEY_TIMEOUT " is not specified in the config file, use default\n");
    }
    else
    {
        cfg.timeout = *int_key;
    }

    int_key = uci_reader_get_int(CONFIG_FILE, CONFIG_SECTION, CONFIG_KEY_SWAP_AXES);
    if (!int_key)
    {
        fprintf(stderr, CONFIG_KEY_SWAP_AXES " is not specified in the config file, use default\n");
    }
    else
    {
        cfg.swap_axes = *int_key;
    }

    int_key = uci_reader_get_int(CONFIG_FILE, CONFIG_SECTION, CONFIG_KEY_INVERT_X);
    if (!int_key)
    {
        fprintf(stderr, CONFIG_KEY_INVERT_X " is not specified in the config file, use default\n");
    }
    else
    {
        cfg.invert_x = *int_key;
    }

    int_key = uci_reader_get_int(CONFIG_FILE, CONFIG_SECTION, CONFIG_KEY_INVERT_Y);
    if (!int_key)
    {
        fprintf(stderr, CONFIG_KEY_INVERT_Y " is not specified in the config file, use default\n");
    }
    else
    {
        cfg.invert_y = *int_key;
    }

    int rc = evdev_loop(&cfg);

    return rc;
}
