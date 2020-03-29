#ifndef BME280_WRAPPER_H
#define BME280_WRAPPER_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

/* Header includes */
#include "bme280_defs.h"
#include "bme280.h"

int bme280_wrap_init();
int bme280_wrap_get_data(float *temperature, float *humidity, float *pressure);

#ifdef __cplusplus
}
#endif
#endif /* BME280_WRAPPER_H */
