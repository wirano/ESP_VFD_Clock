#include "vfd_rtc_time.h"
#include <time.h>
#include "esp_sntp.h"
#include "esp_log.h"


static const char *TAG = "vfd_rtc_time";


void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
//    sntp_stop();
}

void vfd_rtc_init(void)
{
    int retry = 0;
    const int retry_count = 10;

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "ntp1.aliyun.com");
    sntp_setservername(1, "ntp.tuna.tsinghua.edu.cn");
    sntp_setservername(2, "cn.pool.ntp.org");
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();

    setenv("TZ", "CST-8", 1);
    tzset();

    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "sntp init");
}
