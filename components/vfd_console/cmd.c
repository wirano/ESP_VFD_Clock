//
// Created by wirano on 2021/10/30.
//

#include "include/cmd.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "../13st84gink_driver/include/13st84gink_driver.h"
#include "cmd_system.h"
#include "cmd_nvs.h"

static struct {
    struct arg_int *icon_num;
    struct arg_int *ctrl;
    struct arg_end *end;
} icon_args;

static int ad_icon_show(int argc, char **argv){
    extern vfd_13st84gink_st vfd;

    int nerrors = arg_parse(argc, argv, (void **) &icon_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, icon_args.end, argv[0]);
        return 1;
    }

    return vfd_13st84gink_adicon_ctrl(&vfd, icon_args.icon_num->ival[0], icon_args.ctrl->ival[0]);
}

static void register_vfd_ad_icon_show(void){
    icon_args.icon_num = arg_int0(NULL, "icon_index", "<i>", "ad icon index");
    icon_args.ctrl = arg_int0(NULL, "icon_ctrl", "<c>", "ad icon ctrl 1:on 0:off");
    icon_args.end = arg_end(2);

    const esp_console_cmd_t show_icon_cmd = {
            .command = "ad_icon",
            .help = "ctrl ad icon",
            .hint = NULL,
            .func = &ad_icon_show,
            .argtable = &icon_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&show_icon_cmd) );
}

static struct {
    struct arg_int *addr;
    struct arg_int *data;
    struct arg_end *end;
} reg_args;

extern void
vfd_13st84gink_ram_write(vfd_13st84gink_st *vfd_instance, uint8_t ram_write_type, uint8_t start_addr, const uint8_t *buffer,
                         uint8_t len);

static int vfd_write(int argc, char **argv){
    extern vfd_13st84gink_st vfd;

    int nerrors = arg_parse(argc, argv, (void **) &reg_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, reg_args.end, argv[0]);
        return 1;
    }

    vfd_13st84gink_ram_write(&vfd,0x00,reg_args.addr->ival[0],(uint8_t *)reg_args.data->ival,1);

    return 0;
}

static void register_vfd_write(void){
    reg_args.addr = arg_int0("r", "reg_addr", "<r>", "vfd reg addr");
    reg_args.data = arg_int0("d", "reg_data", "<d>", "vfd reg data to write");
    reg_args.end = arg_end(2);

    const esp_console_cmd_t vfd_write_cmd = {
            .command = "vfd_reg_write",
            .help = "write vfd regs",
            .hint = NULL,
            .func = &vfd_write,
            .argtable = &reg_args
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&vfd_write_cmd) );
}

void vfd_console_cmd_register(void){
    register_system();
    register_nvs();

    register_vfd_ad_icon_show();
    register_vfd_write();
}
