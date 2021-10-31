//
// Created by wirano on 2021/10/28.
//

#ifndef INC_13ST84GINK_DRIVER_H
#define INC_13ST84GINK_DRIVER_H

#include <stdint.h>


#define VFD_AD_ICON_TIME   0x01
#define VFD_AD_ICON_SHIFT  0x02
#define VFD_AD_ICON_CLOCK  0x03
#define VFD_AD_ICON_HD     0x04
#define VFD_AD_ICON_USB    0x05
#define VFD_AD_ICON_LOCK   0x06
#define VFD_AD_ICON_DOLBY  0x07
#define VFD_AD_ICON_MUTE   0x08
#define VFD_AD_ICON_TU1    0x09
#define VFD_AD_ICON_TU2    0x0a
#define VFD_AD_ICON_MP3    0x0b
#define VFD_AD_ICON_LOOP   0x0c
#define VFD_AD_ICON_COLON1 0x10
#define VFD_AD_ICON_COLON2 0x11
#define VFD_AD_ICON_COLON3 0x12

#define VFD_CG_ICON_POWER   0x0000000000000001
#define VFD_CG_ICON_SAT     0x0000000000000100
#define VFD_CG_ICON_REC     0x0000000000010000
#define VFD_CG_ICON_RADIO   0x0000000001000000
#define VFD_CG_ICON_TER     0x0000000000000000
#define VFD_CG_ICON_TV      0x0000000000000002
#define VFD_CG_ICON_FILE    0x0000000000000200
#define VFD_CG_ICON_S1      0x0000000000020000
#define VFD_CG_ICON_S2      0x0000000002000000
#define VFD_CG_ICON_S3      0x0000000800000000
#define VFD_CG_ICON_S4      0x0000000000000004
#define VFD_CG_ICON_S5      0x0000000000000400
#define VFD_CG_ICON_S6      0x0000000000040000
#define VFD_CG_ICON_S7      0x0000000004000000
#define VFD_CG_ICON_S8      0x0000000400000000
#define VFD_CG_ICON_S9      0x0000000000000008
#define VFD_CG_ICON_S10     0x0000000000000800
#define VFD_CG_ICON_1080p   0x0000000000080000
#define VFD_CG_ICON_1080i   0x0000000008000000
#define VFD_CG_ICON_720p    0x0000000200000000
#define VFD_CG_ICON_576     0x0000000000000010
#define VFD_CG_ICON_576_i   0x0000000000001000
#define VFD_CG_ICON_576_p   0x0000000000100000
#define VFD_CG_ICON_480     0x0000000010000000
#define VFD_CG_ICON_480_i   0x0000000100000000
#define VFD_CG_ICON_480_p   0x0000000000000020


typedef struct
{
    void (*vfd_hardware_reset)(void);

    void (*vfd_write_data)(const uint8_t *data, uint8_t len);

    void (*vfd_cs_set)(uint8_t pin_level);
} vfd_13st84gink_driver_st;

typedef struct
{
    vfd_13st84gink_driver_st *driver;
} vfd_13st84gink_st;


void vfd_13st84gink_set_dimming(vfd_13st84gink_st *vfd_instance, uint8_t dimming);

int vfd_13st84gink_init_default(vfd_13st84gink_st *vfd_instance);

int vfd_13st84gink_init(vfd_13st84gink_st *vfd_instance);

int vfd_13st84gink_display_str(vfd_13st84gink_st *vfd_instance, const char *str);

int vfd_13st84gink_display_load_CGRAM(vfd_13st84gink_st *vfd_instance, const uint8_t *cgram_buffer, int len);

int vfd_13st84gink_icon_ctrl(vfd_13st84gink_st *vfd_instance, uint8_t icon, uint8_t ctrl);


#endif //INC_13ST84GINK_DRIVER_H
