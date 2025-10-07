
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include "esp_log_level.h"
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_heap_caps.h"

#include "esp_http_server.h"
#include "esp_tls.h"
#include "sdkconfig.h"

#include "globals.h"

/*
 * Web server for authentication
 */

static const char *TAG = "http";

/* Event handler for catching system events */
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == ESP_HTTP_SERVER_EVENT)
    {
        if (event_id == HTTP_SERVER_EVENT_ERROR)
        {
            esp_tls_last_error_t *last_error = (esp_tls_last_error_t *)event_data;
            ESP_LOGE(TAG, "Error event triggered: last_error = %s, last_tls_err = %d, tls_flag = %d", esp_err_to_name(last_error->last_error), last_error->esp_tls_error_code, last_error->esp_tls_flags);
        }
    }
}

/* HTTP GET handler */
static esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    char resp[900];
    snprintf(resp, sizeof(resp),
             "<html><body>"
             "<h2>Music Controller Configuration</h2>"
             "<form action=\"/save\" method=\"post\">"
             "Field 2: <input name=\"Refresh Token\" value=\"%s\"><br>"
             "Field 3: <input name=\"SSID\" value=\"%s\"><br>"
             "Field 4: <input name=\"Password\" value=\"%s\"><br><br>"
             "<input type=\"submit\" value=\"Save\">"
             "</form></body></html>",
             refresh_token, ssid, password);
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

/* HTTP POST handler */
static esp_err_t save_post_handler(httpd_req_t *req)
{
    char buf[512];
    int ret, remaining = req->content_len;

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
        if (strncmp(token, "Refresh Token", 3) == 0)
            strncpy(refresh_token, token + 13, sizeof(refresh_token));
        else if (strncmp(token, "SSID", 3) == 0)
            strncpy(ssid, token + 5, sizeof(ssid));
        else if (strncmp(token, "Password", 3) == 0)
            strncpy(password, token + 8, sizeof(password));
        token = strtok(NULL, "&");
    }

    saveAll();

    const char *resp = "<html><body><h3>Saved! Attempting to connect to WiFi (if updated)</h3><a href=\"/\">Go back</a></body></html>";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static const httpd_uri_t root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler};

static const httpd_uri_t save_uri = {
    .uri = "/save",
    .method = HTTP_POST,
    .handler = save_post_handler,
    .user_ctx = NULL};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server");

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    esp_err_t ret = httpd_start(&server, &config);
    if (ESP_OK != ret)
    {
        ESP_LOGI(TAG, "Error starting server!");
        return NULL;
    }

    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &root);
    httpd_register_uri_handler(server, &save_uri);
    return server;
}

static esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_ssl_stop(server);
}

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
            ESP_LOGE(TAG, "Failed to stop https server");
        }
    }
}

static void connect_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server == NULL)
    {
        *server = start_webserver();
    }
}

void app_main(void)
{
    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Register event handlers to start server when Wi-Fi or Ethernet is connected,
     * and stop server when disconnection happens.
     */

#ifdef CONFIG_EXAMPLE_CONNECT_WIFI
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
#endif // CONFIG_EXAMPLE_CONNECT_WIFI
#ifdef CONFIG_EXAMPLE_CONNECT_ETHERNET
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &disconnect_handler, &server));
#endif // CONFIG_EXAMPLE_CONNECT_ETHERNET
    ESP_ERROR_CHECK(esp_event_handler_register(ESP_HTTP_SERVER_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
}
