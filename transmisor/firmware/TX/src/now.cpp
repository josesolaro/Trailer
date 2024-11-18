#include "now.hpp"

void now_init(esp_now_send_cb_t cb, uint8_t rxAddress[6]){
 ESP_ERROR_CHECK(esp_now_init());
  ESP_ERROR_CHECK(esp_now_register_send_cb(cb));
  uint8_t mac[6];
  esp_now_peer_info_t peer;
  peer.channel = 0;
  peer.ifidx = WIFI_IF_AP;
  peer.encrypt = false;
  xthal_memcpy(peer.peer_addr, rxAddress, 6);
  ESP_ERROR_CHECK(esp_now_add_peer(&peer));

  ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_AP, mac));
  ESP_LOGI("app_main", "%02x:%02x:%02x:%02x:%02x:%02x\n",
           mac[0], mac[1], mac[2],
           mac[3], mac[4], mac[5]);
}