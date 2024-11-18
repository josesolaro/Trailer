#include<now.hpp>

void now_init(esp_now_recv_cb_t cb, uint8_t txAdrress[6])
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());

    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&config));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_LR));

    // uint8_t mac[6];
    // ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_STA, mac));
    // ESP_LOGI("app_main", "%02x:%02x:%02x:%02x:%02x:%02x\n",
    //          mac[0], mac[1], mac[2],
    //          mac[3], mac[4], mac[5]);

    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(cb));

    esp_now_peer_info_t peer;
    peer.channel = 0;
    peer.ifidx = WIFI_IF_STA;
    peer.encrypt = false;
    xthal_memcpy(peer.peer_addr, txAdrress, 6);
    ESP_ERROR_CHECK(esp_now_add_peer(&peer));
}