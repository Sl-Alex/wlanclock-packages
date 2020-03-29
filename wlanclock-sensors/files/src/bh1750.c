#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include "i2c.h"
#include "bh1750.h"

#define BH1750_ADDR 0x23

#define BH1750_CMD_POWERDOWN 0b00000000 // No active state.
#define BH1750_CMD_POWERON   0b00000001 // Power on and wait for measurement command.
#define BH1750_CMD_RESET     0b00000111 // Reset Data register value
#define BH1750_CMD_HR1_CONT  0b00010000 // Start measurement at 1lx resolution.
#define BH1750_CMD_HR2_CONT  0b00010001 // Start measurement at 0.5lx resolution.
#define BH1750_CMD_LOW_CONT  0b00010011 // Start measurement at 4lx resolution.
#define BH1750_CMD_HR1_ONCE  0b00100000 // Start measurement at 1lx resolution.
#define BH1750_CMD_HR2_ONCE  0b00100001 // Start measurement at 0.5lx resolution.
#define BH1750_CMD_LOW_ONCE  0b00100011 // Start measurement at 4lx resolution.

static int bh1750_write(char byte)
{
    return i2c_rw(BH1750_ADDR, &byte, 1, NULL, 0);
}

int bh1750_init()
{
    int rc;
    rc  = bh1750_write(BH1750_CMD_POWERON);
    rc |= bh1750_write(BH1750_CMD_HR2_ONCE);
    return rc;
}

int bh1750_read(uint16_t *brightness)
{
    uint8_t data[2];

    /* Read brightness */
    if (i2c_rw(BH1750_ADDR, 0, 0, (char *)data, 2) != 0)
    {
        return 1;
    }
    *brightness  = data[0] << 8;
    *brightness |= data[1];

    /* Reinitialize and start next measurement */
    return bh1750_init();
}
