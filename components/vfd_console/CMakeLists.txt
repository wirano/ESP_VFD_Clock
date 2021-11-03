idf_component_register(SRCS "vfd_console.c" "cmd.c"
                        INCLUDE_DIRS "include"
                        REQUIRES console vfd_display cmd_system cmd_nvs
        )
