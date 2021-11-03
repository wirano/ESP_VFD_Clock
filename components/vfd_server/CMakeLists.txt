idf_component_register(SRCS "vfd_server.c"
                    INCLUDE_DIRS "include"
                    REQUIRES spiffs esp_http_server json)
