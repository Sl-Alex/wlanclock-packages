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

#include "ubus_client.h"

int main(void)
{
    char const *i2cdev = "/dev/i2c-0";
    int rc;

    int f = i2c_init(i2cdev);

    /* Initialize BH1750 */
    rc = bh1750_init();
    if (rc != 0)
    {
        fprintf(stderr, "Can't initialize BH1750\n");
    }

    /* Wait a bit until measurement results are available */
    usleep(200000);

    while(1)
    {
        uint16_t brightness;
        float temp, hum, press;

        int rc_bh1750 = bh1750_read(&brightness);
        if (rc_bh1750 != 0)
        {
            fprintf(stderr, "Can't read brightness\n");
        }


        int rc_bme280 = bme280_wrap_init();
        rc_bme280 |= bme280_wrap_get_data(&temp, &hum, &press);

        if (rc_bme280 != 0)
        {
            fprintf(stderr, "Failed to get data (code %+d).\n", rc);
        }

        if (!rc_bh1750  && !rc_bme280)
        {
            ubus_client_send(brightness, press, hum, temp);
        }

        /* Sleep for 1 second */
        usleep(1000000);
    }

    close(f);

    printf("Done\n");

    return 0;
}
