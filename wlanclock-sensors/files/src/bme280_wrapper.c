#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include "i2c.h"
#include "bme280.h"

static struct bme280_dev dev;

static int8_t bme280_wrap_i2c_read (uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len);
static int8_t bme280_wrap_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len);
static void   bme280_wrap_delay_ms(uint32_t period);
/*!
 * @brief Function for print the temperature, humidity and pressure data.
 *
 * @param[out] comp_data    :   Structure instance of bme280_data
 *
 * @note Sensor data whose can be read
 *
 * sens_list
 * --------------
 * Pressure
 * Temperature
 * Humidity
 *
 */
void print_sensor_data(struct bme280_data *comp_data);

int bme280_wrap_init()
{
    dev.dev_id = BME280_I2C_ADDR_PRIM;
    dev.intf = BME280_I2C_INTF;
    dev.read = bme280_wrap_i2c_read;
    dev.write = bme280_wrap_i2c_write;
    dev.delay_ms = bme280_wrap_delay_ms;

    /* Initialize the bme280 */
    int rc = bme280_init(&dev);
    if (rc != BME280_OK)
    {
        fprintf(stderr, "Failed to initialize the device (code %+d)\n", rc);
        return 1;
    }
    return 0;
}

/*!
 * @brief This function reading the sensor's registers through I2C bus.
 */
int8_t bme280_wrap_i2c_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    int rc = i2c_rw(id, (char *)&reg_addr, 1, (char *)data, len);
    return rc ? BME280_E_COMM_FAIL: BME280_OK;
}

/*!
 * @brief This function provides the delay for required time (Microseconds) as per the input provided in some of the
 * APIs
 */
void bme280_wrap_delay_ms(uint32_t period)
{
    /* Milliseconds convert to microseconds */
    usleep(period * 1000);
}

/*!
 * @brief This function for writing the sensor's registers through I2C bus.
 */
int8_t bme280_wrap_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    int8_t *buf;

    buf = malloc(len + 1);

    buf[0] = reg_addr;
    memcpy(buf + 1, data, len);
    int rc = i2c_rw(id, (char *)buf, len + 1, NULL, 0);

    free(buf);

    return rc ? BME280_E_COMM_FAIL: BME280_OK;
}

int bme280_wrap_get_data(float *temperature, float *humidity, float *pressure)
{
    /* Variable to define the result */
    int8_t rslt = BME280_OK;

    /* Variable to define the selecting sensors */
    uint8_t settings_sel = 0;

    /* Variable to store minimum wait time between consecutive measurement in force mode */
    uint32_t req_delay;

    /* Structure to get the pressure, temperature and humidity values */
    struct bme280_data comp_data;

    /* Recommended mode of operation: Indoor navigation */
    dev.settings.osr_h = BME280_OVERSAMPLING_1X;
    dev.settings.osr_p = BME280_OVERSAMPLING_16X;
    dev.settings.osr_t = BME280_OVERSAMPLING_2X;
    dev.settings.filter = BME280_FILTER_COEFF_16;

    settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

    /* Set the sensor settings */
    rslt = bme280_set_sensor_settings(settings_sel, &dev);
    if (rslt != BME280_OK)
    {
        fprintf(stderr, "Failed to set sensor settings (code %+d).", rslt);

        return 1;
    }

    /*Calculate the minimum delay required between consecutive measurement based upon the sensor enabled
     *  and the oversampling configuration. */
    req_delay = bme280_cal_meas_delay(&dev.settings);

    /* Set the sensor to forced mode */
    rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
    if (rslt != BME280_OK)
    {
        fprintf(stderr, "Failed to set sensor mode (code %+d).", rslt);
        return 1;
    }

    /* Wait for the measurement to complete and print data */
    dev.delay_ms(req_delay);
    rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
    if (rslt != BME280_OK)
    {
        fprintf(stderr, "Failed to get sensor data (code %+d).", rslt);
        return 1;
    }

    float temp, press, hum;

#ifdef BME280_FLOAT_ENABLE
    temp = comp_data.temperature;
    press = 0.01 * comp_data.pressure;
    hum = comp_data.humidity;
#else
#ifdef BME280_64BIT_ENABLE
    temp = 0.01f * comp_data.temperature;
    press = 0.0001f * comp_data.pressure;
    hum = 1.0f / 1024.0f * comp_data.humidity;
#else
    temp = 0.01f * comp_data.temperature;
    press = 0.01f * comp_data.pressure;
    hum = 1.0f / 1024.0f * comp_data.humidity;
#endif
#endif

    if (temperature) *temperature = temp;
    if (humidity)    *humidity    = hum;
    if (pressure)    *pressure    = press;

    return 0;
}
