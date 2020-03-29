#ifndef BH1750_H
#define BH1750_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

int bh1750_init();
int bh1750_read(uint16_t *brightness);

#ifdef __cplusplus
}
#endif
#endif /* BME280_WRAPPER_H */
