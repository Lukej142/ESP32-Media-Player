#ifndef HTTP_H
#define HTTP_H

#include "globals.h"

static esp_err_t root_get_handler(httpd_req_t *req);
esp_err_t wifi_scan_handler(httpd_req_t *req);

static esp_err_t save_WiFi_post_handler(httpd_req_t *req);
static esp_err_t save_token_post_handler(httpd_req_t *req);

static httpd_handle_t start_webserver(void);
static esp_err_t stop_webserver(httpd_handle_t server);

static void disconnect_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data);
static void connect_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data);
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data);

static void setupAP();
static void connectWiFi();

void setupWiFi();

#endif // HTTP_H
