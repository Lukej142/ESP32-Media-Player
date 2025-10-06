
const char *REFRESH_TOKEN = "Refresh Token";
const char *TOKEN = "Token";
const char *SSID = "SSID";
const char *PASSWORD = "PASSWORD";

char refresh_token[275];
char token[275];
char ssid[20];
char password[20];

__attribute__((weak)) void setRefresh(char *new_refresh)
{
    *refresh_token = new_refresh;
}

__attribute__((weak)) void setToken(char *new_token)
{
    *token = new_token;
}

__attribute__((weak)) void setRefresh(char *new_ssid)
{
    *ssid = new_ssid;
}

__attribute__((weak)) void setRefresh(char *new_password)
{
    *password = new_password;
}

void saveAll()
{
}