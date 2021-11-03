//
// Created by wirano on 2021/10/28.
//

#include "include/13st84gink_driver.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define VFD_13ST84GINK_CMD_DCRAM_WRITE      0x20
#define VFD_13ST84GINK_CMD_DISPLAY_MODE     0x00
#define VFD_13ST84GINK_CMD_CGRAM_WRITE      0x40
#define VFD_13ST84GINK_CMD_ADRAM_WRITE      0x60
#define VFD_13ST84GINK_CMD_URAM_WRITE       0x80
#define VFD_13ST84GINK_CMD_TIMING_SET       0xe0
#define VFD_13ST84GINK_CMD_DIMMING_SET      0xe4
#define VFD_13ST84GINK_CMD_GRAY_SET         0xa0
#define VFD_13ST84GINK_CMD_GRAY_CTRL        0xc0
#define VFD_13ST84GINK_CMD_DISP_CTRL        0xe8
#define VFD_13ST84GINK_CMD_STBY             0xec

#define VFD_13ST84GINK_TIMING_DEFAULT       0xac
#define GRAY_LEVEL_ZONE_D                   0x00
#define GRAY_LEVEL_ZONE_AD1                 0x01
#define GRAY_LEVEL_ZONE_AD2                 0x02


static void vfd_13st84gink_set_display_timing(vfd_13st84gink_st *vfd_instance)
{
    uint8_t cmd1 = VFD_13ST84GINK_CMD_TIMING_SET;
    vfd_instance->driver->vfd_cs_set(0);
    vfd_instance->driver->vfd_write_data(&cmd1, sizeof(cmd1));
    uint8_t cmd2 = VFD_13ST84GINK_TIMING_DEFAULT;
    vfd_instance->driver->vfd_write_data(&cmd2, sizeof(cmd2));
    vfd_instance->driver->vfd_cs_set(1);
}

void vfd_13st84gink_set_dimming(vfd_13st84gink_st *vfd_instance, uint8_t dimming)
{
    uint8_t cmd = VFD_13ST84GINK_CMD_DIMMING_SET;
    vfd_instance->driver->vfd_cs_set(0);
    vfd_instance->driver->vfd_write_data(&cmd, sizeof(cmd));
    vfd_instance->driver->vfd_write_data(&dimming, sizeof(dimming));
    vfd_instance->driver->vfd_cs_set(1);
}

static void vfd_13st84gink_set_gray_level(vfd_13st84gink_st *vfd_instance, uint8_t gray_zone, uint8_t level)
{
    uint8_t cmd = VFD_13ST84GINK_CMD_GRAY_SET | gray_zone;

    vfd_instance->driver->vfd_cs_set(0);
    vfd_instance->driver->vfd_write_data(&cmd, sizeof(cmd));
    vfd_instance->driver->vfd_write_data(&level, sizeof(level));
    vfd_instance->driver->vfd_cs_set(1);
}

static void vfd_13st84gink_gray_ctrl(vfd_13st84gink_st *vfd_instance, uint8_t gray_zone, uint8_t level)
{
    //todo
}

static void vfd_13st84gink_disp_ctrl(vfd_13st84gink_st *vfd_instance, uint8_t ctrl)
{
    uint8_t cmd;

    vfd_instance->driver->vfd_cs_set(0);

    if (ctrl == 1) {
        cmd = VFD_13ST84GINK_CMD_DISP_CTRL;
        vfd_instance->driver->vfd_write_data(&cmd, sizeof(cmd));
    } else if (ctrl == 0) {
        cmd = VFD_13ST84GINK_CMD_DISP_CTRL | 1 << 1;
        vfd_instance->driver->vfd_write_data(&cmd, sizeof(cmd));
    }
    vfd_instance->driver->vfd_cs_set(1);
}

static void vfd_13st84gink_set_disp_standby(vfd_13st84gink_st *vfd_instance, uint8_t stby)
{
    uint8_t cmd = VFD_13ST84GINK_CMD_STBY | stby;
    vfd_instance->driver->vfd_cs_set(0);
    vfd_instance->driver->vfd_write_data(&cmd, sizeof(cmd));
    vfd_instance->driver->vfd_cs_set(1);
}

void
vfd_13st84gink_ram_write(vfd_13st84gink_st *vfd_instance, uint8_t ram_write_type, uint8_t start_addr,
                         const uint8_t *buffer,
                         uint8_t len)
{
    uint8_t cmd = ram_write_type | start_addr;
    vfd_instance->driver->vfd_cs_set(0);
    vfd_instance->driver->vfd_write_data(&cmd, sizeof(cmd));
    vfd_instance->driver->vfd_write_data(buffer, len);
    vfd_instance->driver->vfd_cs_set(1);
}

int vfd_13st84gink_init_default(vfd_13st84gink_st *vfd_instance)
{
    unsigned char VFD_URAM[6] = {0xc0, 0x00, 0x00, 0x03, 0x00, 0x0c};

    vfd_13st84gink_set_display_timing(vfd_instance);

    vfd_instance->driver->vfd_power_ctrl(1);

    vfd_instance->driver->vfd_hardware_reset();

    vfd_13st84gink_set_display_timing(vfd_instance);

    vfd_13st84gink_ram_write(vfd_instance, VFD_13ST84GINK_CMD_URAM_WRITE, 0x00, VFD_URAM, sizeof(VFD_URAM));

    vfd_13st84gink_set_dimming(vfd_instance, 240);

    vfd_13st84gink_disp_ctrl(vfd_instance, 1);

    return 0;
}

int vfd_13st84gink_init(vfd_13st84gink_st *vfd_instance)
{
    vfd_instance->driver->vfd_hardware_reset();

    return 0;
}

int vfd_13st84gink_display_str(vfd_13st84gink_st *vfd_instance, const char *str)
{
    int str_len = strlen(str);

    if (str_len > 12) str_len = 12;

    vfd_13st84gink_ram_write(vfd_instance, VFD_13ST84GINK_CMD_DCRAM_WRITE, 0x01, (uint8_t *) str, str_len);

    return 0;
}

int vfd_13st84gink_display_load_CGRAM(vfd_13st84gink_st *vfd_instance, const uint8_t *cgram_buffer, int len)
{
    vfd_13st84gink_ram_write(vfd_instance, VFD_13ST84GINK_CMD_CGRAM_WRITE, 0x00, cgram_buffer, len);

    return 0;
}

int vfd_13st84gink_adicon_ctrl(vfd_13st84gink_st *vfd_instance, uint8_t adicon, int ctrl)
{

    if (ctrl > 1) ctrl = 1;

    if (adicon < VFD_AD_ICON_COLON1) ctrl <<= 1;

    vfd_13st84gink_ram_write(vfd_instance, VFD_13ST84GINK_CMD_ADRAM_WRITE, adicon, &ctrl, sizeof(ctrl));

    return 0;
}

int vfd_13st84gink_cgicon_ctrl(vfd_13st84gink_st *vfd_instance, uint64_t cgicon, int ctrl)
{
    union {
        uint64_t icon_code;
        uint8_t icon_buffer[8];
    }buffer;

    uint8_t addr = VFD_CGRAM_ICON_ADDR_OFFSET;

    if(ctrl) {
        vfd_instance->_CGRAM_icon |= cgicon;
    } else{
    vfd_instance->_CGRAM_icon &= (~cgicon);
    }

    buffer.icon_code = vfd_instance->_CGRAM_icon;

    vfd_13st84gink_ram_write(vfd_instance,VFD_13ST84GINK_CMD_CGRAM_WRITE,addr,buffer.icon_buffer,5);
    vfd_13st84gink_ram_write(vfd_instance,VFD_13ST84GINK_CMD_DCRAM_WRITE,0,&addr,sizeof(addr));

    return 0;
}
