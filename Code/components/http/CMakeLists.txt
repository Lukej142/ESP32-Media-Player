idf_component_register(SRCS "http.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_http_server esp_wifi nvs_flash esp_eth esp_common heap)
