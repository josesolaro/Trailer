#include <esp_wifi.h>
#include <esp_log.h>
#include <esp_err.h>
#include <nvs_flash.h>
#include <esp_now.h>
#include <dto.hpp>
#include <esp_event.h>

void now_init(esp_now_recv_cb_t cb, uint8_t txAdrress[6]);