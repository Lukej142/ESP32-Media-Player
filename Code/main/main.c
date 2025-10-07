/* Simple HTTP + SSL Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"

#include <esp_http_server.h>
#include "esp_tls.h"
#include "sdkconfig.h"

#include "globals.h"

/*
 * Web server for authentication
 */

#define AP_SSID "Music Hub"
#define ESP_WIFI_CHANNEL 1
#define MAX_STA_CONN 1

/* Event handler for catching system events */
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == ESP_HTTP_SERVER_EVENT)
    {
        if (event_id == HTTP_SERVER_EVENT_ERROR)
        {
            esp_tls_last_error_t *last_error = (esp_tls_last_error_t *)event_data;
            ESP_LOGE(WEB, "Error event triggered: last_error = %s, last_tls_err = %d, tls_flag = %d", esp_err_to_name(last_error->last_error), last_error->esp_tls_error_code, last_error->esp_tls_flags);
        }
    }
}

/* HTTP GET handler */
static esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    char resp[900];
    char refresh_preview[10];
    strncpy(refresh_preview, refresh_token, 10);

    snprintf(resp, sizeof(resp),
             "<html><body>"
             "<h2>Music Controller Configuration</h2>"
             "<form action=\"/saveToken\" method=\"post\">"
             "Refresh Token: <input name=\"Refresh Token\" value=\"%s\"><br>"
             "<input type=\"submit\" value=\"Save Refresh Token\">"
             "</form>"
             "<form action=\"/saveWiFi\" method=\"post\">"
             "Current WiFi: %s<br>"
             "WiFi SSID: <select id=\"ssidSelect\" name=\"ssid\">"
             "<option>Networks</option>"
             "</select>"
             "<button type=\"button\" onclick=\"fetchScan()\">Scan Networks</button><br>"
             "Password: <input name=\"Password\" value=\"\"><br><br>"
             "<input type=\"submit\" value=\"Save WiFi\">"
             "</form>"
             "<script>"
             "function fetchScan(){"
             "  const sel = document.getElementById('ssidSelect');"
             "  sel.innerHTML = '<option>Scanning...</option>';"
             "  fetch('/scan').then(r=>r.text()).then(html => {"
             "    sel.innerHTML = html;"
             "  }).catch(e => {"
             "    sel.innerHTML = '<option>Error scanning</option>';"
             "  });"
             "}"
             "</script>"
             "</body></html>",
             refresh_preview, ssid);
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

/* GET for WiFi scan handler */
esp_err_t wifi_scan_handler(httpd_req_t *req)
{
    int err = esp_wifi_scan_start(NULL, true);
    if (err != ESP_OK) // true = block until done
    {
        char msg[40];
        snprintf(msg, sizeof(msg), "Scan failed %s", esp_err_to_name(err));
        printf("%s\n", msg);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, msg);
        return ESP_FAIL;
    }

    uint16_t ap_count = 0;
    esp_wifi_scan_get_ap_num(&ap_count);
    if (ap_count > 20)
        ap_count = 20;
    wifi_ap_record_t ap_records[ap_count];
    esp_wifi_scan_get_ap_records(&ap_count, ap_records);
    printf("Got %d networks\n", ap_count);

    // Create HTML for dropdown
    char buf[1024];
    int len = 0;
    for (int i = 0; i < ap_count; i++)
    {
        len += snprintf(buf + len, sizeof(buf) - len,
                        "<option value=\"%s\">%s (%d dBm)</option>",
                        (char *)ap_records[i].ssid,
                        (char *)ap_records[i].ssid,
                        ap_records[i].rssi);
        if (len >= sizeof(buf))
            break;
    }

    httpd_resp_send(req, buf, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* WiFi POST handler */
static esp_err_t save_WiFi_post_handler(httpd_req_t *req)
{
    char buf[256];
    int remaining = req->content_len;

    int received = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)));
    if (received <= 0)
    {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    buf[received] = '\0';

    // Example form body: f1=abc&f2=def&f3=ghi&f4=jkl
    char *token = strtok(buf, "&");
    while (token != NULL)
    {
        if (strncmp(token, "ssid", 3) == 0)
            strncpy(ssid, token + 5, sizeof(ssid));
        else if (strncmp(token, "Password", 3) == 0)
            strncpy(password, token + 9, sizeof(password));
        token = strtok(NULL, "&");
    }

    saveWiFi();

    const char *resp = "<html><body><h3>Saved! Attempting to connect to WiFi</h3><a href=\"/\">Go back</a></body></html>";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Token POST handler */
static esp_err_t save_token_post_handler(httpd_req_t *req)
{
    char buf[350];
    int remaining = req->content_len;

    int received = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)));
    if (received <= 0)
    {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    buf[received] = '\0';

    // Example form body: f1=abc&f2=def&f3=ghi&f4=jkl
    if (strncmp(buf, "Refresh Token", 3) == 0)
        strncpy(refresh_token, buf + 14, sizeof(refresh_token));

    saveToken();

    const char *resp = "<html><body><h3>Saved Token!</h3><a href=\"/\">Go back</a></body></html>";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static const httpd_uri_t root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler};

static const httpd_uri_t scan_uri = {
    .uri = "/scan",
    .method = HTTP_GET,
    .handler = wifi_scan_handler};

static const httpd_uri_t save_token_uri = {
    .uri = "/saveToken",
    .method = HTTP_POST,
    .handler = save_token_post_handler,
    .user_ctx = NULL};

static const httpd_uri_t save_WiFi_uri = {
    .uri = "/saveWiFi",
    .method = HTTP_POST,
    .handler = save_WiFi_post_handler,
    .user_ctx = NULL};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;

    // Start the httpd server
    ESP_LOGI(WEB, "Starting server");

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    esp_err_t ret = httpd_start(&server, &config);
    if (ESP_OK != ret)
    {
        ESP_LOGI(WEB, "Error starting server!");
        return NULL;
    }

    // Set URI handlers
    ESP_LOGI(WEB, "Registering URI handlers");
    httpd_register_uri_handler(server, &root);
    httpd_register_uri_handler(server, &scan_uri);
    httpd_register_uri_handler(server, &save_token_uri);
    httpd_register_uri_handler(server, &save_WiFi_uri);
    return server;
}

static esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_stop(server);
}

/**
 * Wifi Disconnect Handler
 */
static void disconnect_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server)
    {
        if (stop_webserver(*server) == ESP_OK)
        {
            *server = NULL;
        }
        else
        {
            ESP_LOGE(WEB, "Failed to stop https server");
        }
    }
}

/**
 * Wifi Connect Handler
 */
static void connect_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server == NULL)
    {
        *server = start_webserver();
    }
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    printf("Event nr: %ld!\n", event_id);
}

static void setupAP()
{
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = strlen(AP_SSID),
            .channel = ESP_WIFI_CHANNEL,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_OPEN,
            .pmf_cfg = {
                .required = true,
            },
        },
    };

    esp_wifi_stop();
    esp_wifi_set_mode(WIFI_MODE_APSTA);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    esp_wifi_start();
}

static void connectWiFi()
{
    esp_wifi_stop();
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();

    wifi_config_t sta_cfg = {0};
    strcpy((char *)sta_cfg.sta.ssid, ssid);
    strcpy((char *)sta_cfg.sta.password, password);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_cfg));
    if (esp_wifi_connect() != ESP_OK)
        setupAP();
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &wifi_event_handler,
                                        NULL,
                                        NULL);

    if (readToken() || readWiFi())
        setupAP();
    else
        connectWiFi();

    httpd_handle_t server = start_webserver();

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
}
