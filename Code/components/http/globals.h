#ifndef GLOBALS
#define GLOBALS

#include <esp_log.h>
#include <nvs_flash.h>
#include "sdkconfig.h"

extern const char *WEB;
extern const char *TOKEN;

extern const char *REFRESH_TOKEN;
extern const char *API_TOKEN;
extern const char *SSID;
extern const char *PASSWORD;
extern const char *ID;
extern const char *SECRET;

extern char refresh_token[275];
extern char id[128];
extern char secret[128];
extern char token[275];
extern char ssid[33];
extern char password[32];

void saveWiFi();
int readWiFi();

void saveID();
int readID();

void saveToken();
int readToken();

#endif // GLOBALS
