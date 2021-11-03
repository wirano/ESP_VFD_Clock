//
// Created by wirano on 2021/11/2.
//

#include "include/vfd_server.h"

#include <string.h>
#include <stdlib.h>

#include "esp_err.h"
#include "esp_log.h"

#include "esp_spiffs.h"
#include "esp_http_server.h"
#include "cJSON.h"


static const char *TAG = "vfd_server";


static char *file_load(long * file_size, const char *path, const char *type)
{
    FILE *fp = fopen(path, type);
    char *buf = NULL;
    long fs;

    fseek(fp, 0, SEEK_END);
    fs = ftell(fp);
    rewind(fp);

    buf = (char *) malloc(fs);
    fread(buf, sizeof(char), fs, fp);
    fclose(fp);

    *file_size = fs;
    return buf;
}

/* Handler to redirect incoming GET request for /index.html to /
 * This can be overridden by uploading file with same name */
static esp_err_t index_html_get_handler(httpd_req_t *req)
{
    char *buffer = NULL;
    long buffer_size;

    ESP_LOGI(TAG,"uri:%s",req->uri);

    if(strcmp(req->uri,"/") == 0){
        buffer = file_load(&buffer_size,"/spiffs/index.html","rb");
        httpd_resp_send(req,buffer,buffer_size);
        free(buffer);
    } else if(strcmp(req->uri,"/VFD_Clock.js") == 0){
        buffer = file_load(&buffer_size,"/spiffs/VFD_Clock.js","rb");
        httpd_resp_send(req,buffer,buffer_size);
        free(buffer);
    } else if(strcmp(req->uri,"/favicon.ico") == 0) {
        buffer = file_load(&buffer_size, "/spiffs/favicon.ico", "rb");
        httpd_resp_set_type(req, "image/x-icon");
        httpd_resp_send(req, buffer, buffer_size);
        free(buffer);
    }

//    FILE *fp = fopen("/spiffs/index.html","rb");
//
//    fseek(fp,0,SEEK_END);
//    long size = ftell(fp);
//    rewind(fp);
//
//    char *fh = (char *)malloc(size);
//    fread(fh, sizeof(char),size,fp);
//
//    ESP_LOGI(TAG,"%s",req->uri);
//    ESP_LOGI(TAG,"long:%u", sizeof(fh));
//
//    httpd_resp_send(req, fh, size);
//    free(fh);
//    fclose(fp);

    return ESP_OK;
}

static esp_err_t wifi_data_handler(httpd_req_t *req){
    char *buffer;
    cJSON *wifi_info;
    cJSON *ssid_list;
    cJSON *rssi_list;
    char *json_buffer;

    ESP_LOGI(TAG,"uri:%s",req->uri);

    if(strcmp(req->uri,"/wifi_data/send") == 0) {
        buffer = malloc(req->content_len);
        httpd_req_recv(req, buffer, req->content_len);

        free(buffer);

        httpd_resp_sendstr(req, "Post control value successfully");
    }else if(strcmp(req->uri,"/wifi_data/get_ssid") == 0){
        wifi_info = cJSON_CreateObject();
        cJSON_AddNumberToObject(wifi_info,"num",2);
        ssid_list = cJSON_CreateArray();
        cJSON_AddItemToArray(ssid_list, cJSON_CreateString("wifi1"));
        cJSON_AddItemToArray(ssid_list, cJSON_CreateString("wifi2"));
        cJSON_AddItemToObject(wifi_info,"info_ssid",ssid_list);
        rssi_list = cJSON_CreateArray();
        cJSON_AddItemToArray(rssi_list, cJSON_CreateString("0"));
        cJSON_AddItemToArray(rssi_list, cJSON_CreateString("-100"));
        cJSON_AddItemToObject(wifi_info,"info_rssi",rssi_list);

        json_buffer = cJSON_Print(wifi_info);
        ESP_LOGI(TAG,"%s",json_buffer);
        httpd_resp_sendstr(req,json_buffer);
    }

    return ESP_OK;
}

static esp_err_t init_spiffs(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
            .base_path = "/spiffs",
            .partition_label = NULL,
            .max_files = 5,   // This decides the maximum number of files that can be created on the storage
            .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ESP_FAIL;
    }


    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    return ESP_OK;
}

esp_err_t start_http_server(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* Use the URI wildcard matching function in order to
     * allow the same handler to respond to multiple different
     * target URIs which match the wildcard scheme */
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(TAG, "Starting HTTP Server");
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start file server!");
        return ESP_FAIL;
    }

    httpd_uri_t get_index = {
            .uri       = "/*",
            .method    = HTTP_GET,
            .handler   = index_html_get_handler,
    };
    httpd_register_uri_handler(server, &get_index);

    httpd_uri_t wifi_data = {
            .uri       = "/wifi_data/*",   // Match all URIs of type /upload/path/to/file
            .method    = HTTP_POST,
            .handler   = &wifi_data_handler,
    };
    httpd_register_uri_handler(server, &wifi_data);

    return ESP_OK;
}

void vfd_server_init(void)
{
    init_spiffs();

    start_http_server();
}
