#include <stdio.h>
#include "vfd_wifi.h"

#include "wifi_ap.h"
#include "wifi_sta.h"

void wifi_init(void)
{
//    wifi_init_softap();
    wifi_init_sta();
}
