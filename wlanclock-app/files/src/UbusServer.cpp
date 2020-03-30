#include "UbusServer.h"
#include "ubus_server.h"

UbusServer::UbusServer()
{
}

int UbusServer::start(void)
{
    return ubus_server_init();
}

int UbusServer::stop(void)
{
    ubus_server_deinit();
    return 0;
}

bool UbusServer::isRunning(void)
{
    return ubus_server_is_running();
}
