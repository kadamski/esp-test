#include <osapi.h>
#include <user_interface.h>
#include "ets_sys.h"

void ICACHE_FLASH_ATTR user_init(void)
{
    uart_init(9600);
    const char ssid[32] = "ssid";
    const char password[32] = "password";

    struct station_config stationConf;

    wifi_set_opmode(STATION_MODE);
    os_memcpy(&stationConf.ssid, ssid, sizeof(ssid));
    os_memcpy(&stationConf.password, password, sizeof(password));
    wifi_station_set_config(&stationConf);
    wifi_station_connect();
}
