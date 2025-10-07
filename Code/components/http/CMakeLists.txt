idf_component_register(SRCS "http.c"
                    INCLUDE_DIRS "." "../../main/"
                    PRIV_REQUIRES esp_https_server esp_wifi nvs_flash)
