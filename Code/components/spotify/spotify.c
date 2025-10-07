#include "spotify.h"
#include "globals.h"
#include <esp_http_client.h>

const char *spotifyApiUrl = "https://api.spotify.com/v1/me/player";
const char *spotifyTokenURL = "https://accounts.spotify.com/api/token";
const char *CONTENT_TYPE = "application/x-www-form-urlencoded";

#define RESP_SIZE 15360
#define SEND_SIZE 352
char local_response_buffer[RESP_SIZE]; // 15KB
char send_buffer[SEND_SIZE];

esp_http_client_config_t token_config = {
    .url = spotifyTokenURL,
    .username = id,
    .password = secret,
    .auth_type = HTTP_AUTH_TYPE_BASIC,
    .method = HTTP_METHOD_POST,
    .user_data = local_response_buffer,
};

esp_http_client_config_t api_config = {
    .url = spotifyApiUrl,
};

static void updateToken()
{
    esp_http_client_handle_t client = esp_http_client_init(&token_config);

    snprintf(send_buffer, SEND_SIZE, "{\"grant_type\":\"refresh_token\", \"refresh_token\":\"%s\"}", refresh_token);
    esp_http_client_set_header(client, "Content-Type", CONTENT_TYPE);
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %" PRId64,
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

void getRequest(const char *URL)
{
    esp_http_client_handle_t client = esp_http_client_init(&api_config);

    // GET
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %" PRId64,
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    ESP_LOG_BUFFER_HEX(TAG, local_response_buffer, strlen(local_response_buffer));

    esp_http_client_cleanup(client);
}