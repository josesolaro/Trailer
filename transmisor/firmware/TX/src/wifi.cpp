#include "wifi.hpp"

void config_wifi()
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&config));
    esp_netif_create_default_wifi_ap();

    // AUTH_WPA2 -> 12 chars
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "ESP_32_TX",
            .password = "josejosejose",
            .authmode = WIFI_AUTH_WPA2_PSK,
            .max_connection = 2,
        },

    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    // ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR));


    ESP_LOGI("WIFI_CONFIG", "wifi_init_AP finished.");
}