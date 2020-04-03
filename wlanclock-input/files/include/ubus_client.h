#ifndef UBUS_CLIENT_H
#define UBUS_CLIENT_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void ubus_client_send(uint32_t brightness, float pressure, float humidity, float temperature);

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* UBUS_CLIENT_H */
