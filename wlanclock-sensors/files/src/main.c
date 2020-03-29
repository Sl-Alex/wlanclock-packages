/*
    spidevlib.c - A user-space program to comunicate using spidev.
                Gustavo Zamboni
*/
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#include "i2c.h"
#include "bh1750.h"
#include "bme280_wrapper.h"

int main(void)
{
    char const *i2cdev = "/dev/i2c-0";
    int rc;

    int f = i2c_init(i2cdev);

    rc = bh1750_init();
    if (rc != 0)
    {
        fprintf(stderr, "Can't initialize BH1750\n");
    }

    usleep(200000);

    uint16_t brightness;
    rc = bh1750_read(&brightness);
    if (rc != 0)
    {
        fprintf(stderr, "Can't read brightness\n");
    }
    else
    {
        printf("Brightness = 0x%04X\n", brightness);
    }


    bme280_wrap_init();

    float temp, hum, press;
    rc = bme280_wrap_get_data(&temp, &hum, &press);
    if (rc != 0)
    {
        fprintf(stderr, "Failed to stream sensor data (code %+d).\n", rc);
    }
    else
    {
        printf("%0.2lf deg C, %0.2lf%%, %0.2lf hPa\n", temp, hum, press);
    }

    close(f);

    printf("Done\n");
}
