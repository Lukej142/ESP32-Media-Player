#include "globals.h"

const char *WEB = "WEB";
const char *TOKEN = "TOKEN";

const char *REFRESH_TOKEN = "Refresh Token";
const char *API_TOKEN = "API Token";
const char *SSID = "SSID";
const char *PASSWORD = "PASSWORD";
const char *ID = "ID";
const char *SECRET = "SECRET";

char refresh_token[275];
char id[128];
char secret[128];
char token[275];
char ssid[33];
char password[32];

void saveWiFi()
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(WEB, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return;
    }
    if (nvs_set_str(my_handle, SSID, ssid) != ESP_OK)
        ESP_LOGE(WEB, "Failed to write ssid\n");
    if (nvs_set_str(my_handle, PASSWORD, password) != ESP_OK)
        ESP_LOGE(WEB, "Failed to write password\n");
    err = nvs_commit(my_handle);
    if (err != ESP_OK)
        ESP_LOGE(WEB, "Failed to commit NVS changes!");
    nvs_close(my_handle);

    printf("SSID: %s, Password: %s\n", ssid, password);
}

int readWiFi()
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(WEB, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return 1;
    }
    size_t size = sizeof(ssid);
    if (nvs_get_str(my_handle, SSID, ssid, &size) != ESP_OK)
    {
        ESP_LOGE(WEB, "Failed to read ssid\n");
        return 1;
    }
    size = sizeof(password);
    if (nvs_get_str(my_handle, PASSWORD, password, &size) != ESP_OK)
    {
        ESP_LOGE(WEB, "Failed to read password\n");
        return 1;
    }
    nvs_close(my_handle);

    printf("SSID: %s, Password: %s\n", ssid, password);
    return 0;
}

void save(const char *name, const char *dest)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TOKEN, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return;
    }
    if (nvs_set_str(my_handle, name, dest) != ESP_OK)
        ESP_LOGE(TOKEN, "Failed to write\n");
    err = nvs_commit(my_handle);
    if (err != ESP_OK)
        ESP_LOGE(TOKEN, "Failed to commit NVS changes!");
    nvs_close(my_handle);

    printf("Refresh Token: %s\n", refresh_token);
}

int read(const char *name, const char *dest)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TOKEN, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return 1;
    }
    size_t size = sizeof(dest);
    if (nvs_get_str(my_handle, name, dest, &size) != ESP_OK)
    {
        ESP_LOGE(TOKEN, "Failed to read ssid\n");
        return 1;
    }
    nvs_close(my_handle);

    printf("Refresh Token: %s\n", refresh_token);
    return 0;
}
