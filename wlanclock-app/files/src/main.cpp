/*
    spidevlib.c - A user-space program to comunicate using spidev.
                Gustavo Zamboni
*/
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <iostream>

#include "UbusServer.h"
#include "ubus_server.h"
 
char buf[10];
char buf2[10];
 
struct spi_ioc_transfer xfer;
 
/* Init spidev */
int spi_init(const char *spidev)
{
    int file;
    uint8_t  mode, lsb, bits;
    uint32_t speed;

    if ((file = open(spidev,O_RDWR)) < 0)
    {
        printf("Failed to open the bus.");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
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
        perror("SPI rd_mode");
        exit(1);
    }
    if (ioctl(file, SPI_IOC_RD_LSB_FIRST, &lsb) < 0)
    {
        perror("SPI rd_lsb_fist");
        exit(1);
    }
    //sunxi supports only 8 bits
    /*
        if (ioctl(file, SPI_IOC_WR_BITS_PER_WORD, (__u8[1]){8})<0)
            {
            perror("can't set bits per word");
            return;
            }
    */
    if (ioctl(file, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0)
    {
        perror("SPI bits_per_word");
        exit(1);
    }
    /*
        if (ioctl(file, SPI_IOC_WR_MAX_SPEED_HZ, &speed)<0)
            {
            perror("can't set max speed hz");
            return;
            }
    */
    if (ioctl(file, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0)
    {
        perror("SPI max_speed_hz");
        exit(1);
    }
 

    printf("%s: spi mode %d, %d bits %sper word, %d Hz max\n",spidev, mode, bits, lsb ? "(lsb first) " : "", speed);

    xfer.cs_change = 0;   /* Keep CS activated */
    xfer.delay_usecs = 0;
    xfer.speed_hz = 10000000;
    xfer.bits_per_word = 8;

    return file;
}

/* Write to spidev */
void spi_write(int file, char *buf, uint32_t sz)
{
    int status;

    xfer.tx_buf = (unsigned long)buf;
    xfer.len = sz;
    status = ioctl(file, SPI_IOC_MESSAGE(1), &xfer);
    if (status < 0)
    {
        perror("SPI_IOC_MESSAGE");
        return;
    }
}

int main(void)
{
    char const *spidev = "/dev/spidev0.1";
    char buf[4096];

    for (size_t i = 0; i < sizeof(buf); i++)
    {
        buf[i] = i % 0x100;
    }

    int f=spi_init(spidev);
    spi_write(f, buf, sizeof(buf));
    close(f);

    std::cout << "SPI access done" << std::endl;

    std::cout << "Starting ubus server" << std::endl;
    if (0 == UbusServer::getInstance().start())
    {
        std::cout << "Server started" << std::endl;
    }
    else
    {
        exit(1);
    }

    while(UbusServer::getInstance().isRunning())
    {
        usleep(10000);
    }
    UbusServer::getInstance().stop();
    std::cout << "Terminated" << std::endl;
}
