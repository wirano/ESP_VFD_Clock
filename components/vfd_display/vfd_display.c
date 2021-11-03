#include <sys/cdefs.h>
#include "vfd_display.h"

#include <string.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "esp_log.h"

#include "../13st84gink_driver/include/13st84gink_driver.h"


#define VFD_CS_PIN  11
#define VFD_CP_PIN  12
#define VFD_DA_PIN  13
#define VFD_RST_PIN 10

#define VFD_POWER_PIN 14


static const char TAG[] = "vfd_display";

spi_device_handle_t spi;
vfd_13st84gink_st vfd;

static void vfd_spi_init(void)
{
    esp_err_t ret;
    spi_bus_config_t buscfg = {
            .miso_io_num=-1,
            .mosi_io_num=VFD_DA_PIN,
            .sclk_io_num=VFD_CP_PIN,
            .quadwp_io_num=-1,
            .quadhd_io_num=-1,
            .max_transfer_sz=1024
    };
    spi_device_interface_config_t devcfg = {
            .clock_speed_hz=250 * 1000,               //Clock out at 500 kHz
            .mode=3,                                //SPI mode 3
            .spics_io_num=-1,               //CS pin
            .queue_size=2,                          //We want to be able to queue 7 transactions at a time
            .flags = SPI_DEVICE_BIT_LSBFIRST,
    };
    //Initialize the SPI bus
    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_DISABLED);
    ESP_ERROR_CHECK(ret);
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);
}

static void vfd_gpio_init(void)
{
    esp_err_t ret;
    gpio_config_t vfd_gpio_cfg;

    vfd_gpio_cfg.intr_type = GPIO_INTR_DISABLE;
    vfd_gpio_cfg.pull_up_en = GPIO_PULLUP_DISABLE;
    vfd_gpio_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    vfd_gpio_cfg.mode = GPIO_MODE_OUTPUT;
    vfd_gpio_cfg.pin_bit_mask = 1 << VFD_RST_PIN;

    ret = gpio_config(&vfd_gpio_cfg);
    ESP_ERROR_CHECK(ret);

    vfd_gpio_cfg.intr_type = GPIO_INTR_DISABLE;
    vfd_gpio_cfg.pull_up_en = GPIO_PULLUP_DISABLE;
    vfd_gpio_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    vfd_gpio_cfg.mode = GPIO_MODE_OUTPUT;
    vfd_gpio_cfg.pin_bit_mask = 1 << VFD_CS_PIN;

    ret = gpio_config(&vfd_gpio_cfg);
    ESP_ERROR_CHECK(ret);

    gpio_set_level(VFD_CS_PIN,1);

    vfd_gpio_cfg.intr_type = GPIO_INTR_DISABLE;
    vfd_gpio_cfg.pull_up_en = GPIO_PULLUP_DISABLE;
    vfd_gpio_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    vfd_gpio_cfg.mode = GPIO_MODE_OUTPUT;
    vfd_gpio_cfg.pin_bit_mask = 1 << VFD_POWER_PIN;

    ret = gpio_config(&vfd_gpio_cfg);
    ESP_ERROR_CHECK(ret);

    gpio_set_level(VFD_POWER_PIN,0);
}

static void vfd_spi_trans(const uint8_t *buffer, int len)
{
    esp_err_t ret;
    spi_transaction_t t;

    if (len == 0) return;             //no need to send anything
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length = len * 8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer = buffer;               //Data
    ret = spi_device_polling_transmit(spi, &t);  //Transmit!

    ESP_ERROR_CHECK(ret);            //Should have had no issues.
}

static void vfd_hardware_rst(void)
{
    gpio_set_level(VFD_RST_PIN, 0);
    vTaskDelay(5 / portTICK_RATE_MS);
    gpio_set_level(VFD_RST_PIN, 1);
    vTaskDelay(5 / portTICK_RATE_MS);

    ESP_LOGI(TAG,"vfd reset");
}

static void vfd_cs_set(int level){
    gpio_set_level(VFD_CS_PIN,level);
}

static void vfd_power_ctrl(int ctrl){
    gpio_set_level(VFD_POWER_PIN,ctrl);
}

_Noreturn static void vfd_display_task(void *args){
    vfd_13st84gink_driver_st vfd_driver;

    time_t time_now;
    struct tm time_info;

    char str_buffer[13];

    int brightness[] = {2,3,4,5,7,9,13,17,25,35,50,70,100,130,160,200,240};

    vfd_driver.vfd_write_data = vfd_spi_trans;
    vfd_driver.vfd_hardware_reset = vfd_hardware_rst;
    vfd_driver.vfd_cs_set = vfd_cs_set;
    vfd_driver.vfd_power_ctrl = vfd_power_ctrl;
    vfd.driver = &vfd_driver;

    vfd_13st84gink_init_default(&vfd);

    vfd_13st84gink_display_str(&vfd,"  Hi Honey");

    for (int i = 0; i < sizeof(brightness) / sizeof(int); ++i) {
        vfd_13st84gink_set_dimming(&vfd,(uint8_t )brightness[i]);
        vTaskDelay((50)/ portTICK_PERIOD_MS);
    }

    while(1){
        time(&time_now);
        localtime_r(&time_now,&time_info);
        strftime(str_buffer,sizeof(str_buffer),"%a %X",&time_info);

        vfd_13st84gink_display_str(&vfd,str_buffer);

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void vfd_display_init(void)
{
    vfd_gpio_init();
    vfd_spi_init();

    xTaskCreate(vfd_display_task,"vfd_display",4096,NULL,CONFIG_ESP32_PTHREAD_TASK_PRIO_DEFAULT,NULL);
}
