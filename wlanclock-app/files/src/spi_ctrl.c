#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "spi_ctrl.h"

#include <dirent.h>
#include <errno.h>

#define SPI_MAX_LENGTH 4096

/* Write to spidev */
void spi_write(int file, void *buf, uint32_t sz)
{
    int status;

    struct spi_ioc_transfer xfer;
    xfer.cs_change = 0;   /* Keep CS activated */
    xfer.delay_usecs = 0;
    xfer.speed_hz = 10000000;
    xfer.bits_per_word = 8;

    xfer.rx_buf = (unsigned long)NULL;
    xfer.pad = 0;

    uint32_t cnt = 0;
    while (sz)
    {
        uint32_t to_send;
        if (sz > SPI_MAX_LENGTH)
        {
            to_send = SPI_MAX_LENGTH;
        }
        else
        {
            to_send = sz;
        }
        xfer.tx_buf = (unsigned long)&((char *)buf)[cnt];
        xfer.len = to_send;

        status = ioctl(file, SPI_IOC_MESSAGE(1), &xfer);
        if (status < 0)
        {
            perror("SPI_IOC_MESSAGE");
            return;
        }
        cnt += to_send;
        sz -= to_send;
    }
}

/* Init spidev */
int spi_init(const char *spidev, int speed)
{
    int file;
    uint8_t  mode, lsb, bits;
    uint32_t speed_max;

    if ((file = open(spidev,O_RDWR)) < 0)
    {
        fprintf(stderr, "Failed to open the bus\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        return 1;
    }

    // possible modes: mode |= SPI_LOOP; mode |= SPI_CPHA; mode |= SPI_CPOL; mode |= SPI_LSB_FIRST; mode |= SPI_CS_HIGH; mode |= SPI_3WIRE; mode |= SPI_NO_CS; mode |= SPI_READY;
    // multiple possibilities using |
    /*
    if (ioctl(file, SPI_IOC_WR_MODE, &mode)<0)   {
        perror("can't set spi mode");
        return;
    }
    */

    if (ioctl(file, SPI_IOC_RD_MODE, &mode) < 0)
    {
        fprintf(stderr, "SPI rd_mode\n");
        return 1;
    }
    if (ioctl(file, SPI_IOC_RD_LSB_FIRST, &lsb) < 0)
    {
        fprintf(stderr, "SPI rd_lsb_fist\n");
        return 1;
    }
    if (ioctl(file, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0)
    {
        fprintf(stderr, "SPI bits_per_word\n");
        return 1;
    }
    if (ioctl(file, SPI_IOC_RD_MAX_SPEED_HZ, &speed_max) < 0)
    {
        fprintf(stderr, "SPI max_speed_hz\n");
        return 1;
    }
    if (speed > speed_max)
    {
        fprintf(stderr, "maximum SPI speed is %d\n", speed_max);
    }
 

    printf("%s: spi mode %d, %d bits %sper word, %d Hz\n",spidev, mode, bits, lsb ? "(lsb first) " : "", speed);

    return file;
}
