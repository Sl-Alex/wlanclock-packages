#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <stdio.h>

#include "i2c.h"

static struct i2c_rdwr_ioctl_data work_queue;
static struct i2c_msg             msg[2];
static int fd = -1;

/* Init i2cdev */
int i2c_init(const char *i2cdev)
{
    if ((fd = open(i2cdev,O_RDWR)) < 0)
    {
        perror("Failed to open the bus.");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        return 1;
    }

    return fd;
}

/* Write to i2cdev, then read */
int i2c_rw(char addr, char *wr_buf, uint32_t wr_sz, char *rd_buf, uint32_t rd_sz)
{
    int status;
    work_queue.nmsgs = 2;

    work_queue.msgs = msg;

    if (!wr_buf || !wr_sz)
    {
        work_queue.nmsgs -= 1;
    }
    if (!rd_buf || !rd_sz)
    {
        work_queue.nmsgs -= 1;
    }
    if (!work_queue.nmsgs)
    {
        perror("Empty wr and rd buffers");
        return -1;
    }

    if (!wr_buf || !wr_sz)
    {
        work_queue.msgs[0].addr  = addr;
        work_queue.msgs[0].len   = rd_sz;
        work_queue.msgs[0].flags = I2C_M_RD;
        work_queue.msgs[0].buf   = (uint8_t *)rd_buf;
    }
    else
    {
        work_queue.msgs[0].addr  = addr;
        work_queue.msgs[0].len   = wr_sz;
        work_queue.msgs[0].flags = 0;
        work_queue.msgs[0].buf   = (uint8_t *)wr_buf;
        work_queue.msgs[1].addr  = addr;
        work_queue.msgs[1].len   = rd_sz;
        work_queue.msgs[1].flags = I2C_M_RD;
        work_queue.msgs[1].buf   = (uint8_t *)rd_buf;
    }

    status = ioctl(fd,I2C_RDWR,&work_queue);
    if (status < 0)
    {
        perror("I2C_RDWR");
        return -1;
    }
    return 0;
}

/* Deinit i2cdev */
void i2c_deinit(void)
{
    close(fd);
}
