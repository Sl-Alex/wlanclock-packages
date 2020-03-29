#ifndef I2C_H
#define I2C_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

/* Init i2cdev */
int i2c_init(const char *i2cdev);
/* Write to i2cdev, then read */
int i2c_rw(char addr, char *wr_buf, uint32_t wr_sz, char *rd_buf, uint32_t rd_sz);
/* Deinit i2cdev */
void i2c_deinit(void);

#ifdef __cplusplus
}
#endif
#endif /* I2C_H */
