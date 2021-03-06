#ifndef UBUS_SERVER_H
#define UBUS_SERVER_H

#include <libubus.h>

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ubus_cb_gesture_t      )(uint32_t value);
typedef void (*ubus_cb_brightness_t   )(uint32_t value);
typedef void (*ubus_cb_pres_hum_temp_t)(double pressure, double humidity, double temperature);
typedef void (*ubus_cb_rel_pos_t)(int rel_x, int rel_y);

void ubus_server_set_cb_gesture      (ubus_cb_gesture_t       cb);
void ubus_server_set_cb_brightness   (ubus_cb_brightness_t    cb);
void ubus_server_set_cb_pres_hum_temp(ubus_cb_pres_hum_temp_t cb);
void ubus_server_set_cb_rel_pos      (ubus_cb_rel_pos_t cb);

int  ubus_server_init();
void ubus_server_deinit();
int  ubus_server_is_running();

extern struct ubus_object wlanclock_data_obj;

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* UBUS_SERVRER_H */
