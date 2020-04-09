#ifndef SPI_CTRL_H
#define SPI_CTRL_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

/* Write to spidev */
void spi_write(int file, void *buf, uint32_t sz);
/* Init spidev */
int spi_init(const char *spidev, int speed);

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* SPI_CTRL_H */
