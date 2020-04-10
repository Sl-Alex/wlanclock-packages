#ifndef UBUS_SERVER_H
#define UBUS_SERVER_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ubus_cb_gesture_t      )(uint32_t value);
typedef void (*ubus_cb_brightness_t   )(uint32_t value);
typedef void (*ubus_cb_pres_hum_temp_t)(double pressure, double humidity, double temperature);

void ubus_server_set_cb_gesture      (ubus_cb_gesture_t       cb);
void ubus_server_set_cb_brightness   (ubus_cb_brightness_t    cb);
void ubus_server_set_cb_pres_hum_temp(ubus_cb_pres_hum_temp_t cb);

int  ubus_server_init();
void ubus_server_deinit();
int  ubus_server_is_running();

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* UBUS_SERVRER_H */
