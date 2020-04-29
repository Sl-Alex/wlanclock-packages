#ifndef UBUS_CLIENT_H
#define UBUS_CLIENT_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void ubus_client_send_gesture(uint32_t gesture);
void ubus_client_send_rel(int32_t x, int32_t y);

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* UBUS_CLIENT_H */
