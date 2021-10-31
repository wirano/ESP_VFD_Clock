idf_component_register(SRCS "vfd_wifi.c" "wifi_ap.c" "wifi_sta.c"
                        INCLUDE_DIRS "include"
                        REQUIRES nvs_flash
                        )
