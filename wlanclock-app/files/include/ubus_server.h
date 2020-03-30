#ifndef UBUS_SERVER_H
#define UBUS_SERVER_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

int ubus_server_init();
int ubus_server_deinit();
int ubus_server_is_running();

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* UBUS_SERVRER_H */
