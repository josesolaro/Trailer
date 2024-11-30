#include <driver/gpio.h>
// #include <esp_now.h>
// #include <esp_event.h>
#include <esp_log.h>
#include <nvs_flash.h>
// #include <esp_wifi.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include "wifi.hpp"
#include "server.hpp"
#include "now.hpp"
// #include "dto.hpp"

// THIS MAC = e4:65:b8:4a:11:55
// other mac = e4:65:b8:4a:0b:f0 sta f1 ap
uint8_t rx_address[6] = {0xE4, 0x65, 0xB8, 0x4A, 0x0B, 0xF0};
const int QUEUE_SIZE = 5;
EventGroupHandle_t s_wifi_event_group;
// const esp_event_base_t EVENT_TYPE = "light";
esp_event_loop_handle_t loop_handle;
Server server;

ESP_EVENT_DEFINE_BASE(LIGHT_EVENT);

// void wifiEventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
// {
//   if (event_base == WIFI_EVENT) {
//         switch (event_id) {
//             case WIFI_EVENT_AP_STACONNECTED:
//                 ESP_LOGI("WIFI_HANDLER", "WIFI_EVENT_AP_START");
//                 // esp_wifi_connect();
//                 break;
//             case WIFI_EVENT_AP_STADISCONNECTED:
//                 ESP_LOGI("WIFI_HANDLER", "WIFI_EVENT_AP_DISCONNECTED");
//                 // esp_wifi_disconnect();
//                 break;
//         }
//     }
// }
void run_on_event(void *handler_arg, esp_event_base_t base, int32_t id, void *event_data)
{
  ESP_LOGI("EVENT_HANDLE", "event recieved");
  DTO* dto = (DTO*)event_data;
  // bool data = (bool)event_data;
  if(dto->breaks){
  // if (data){
    gpio_set_level(GPIO_NUM_2, 1);
  }
  else
  {
    gpio_set_level(GPIO_NUM_2, 0);
  }
  // ESP_ERROR_CHECK(esp_now_send(rx_address, (uint8_t*)dto, sizeof(DTO)));
}

static void send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  if (mac_addr == NULL)
  {
    ESP_LOGI("send_cb", "Send cb arg error");
    return;
  }
  if (status == ESP_NOW_SEND_FAIL)
  {
    ESP_LOGI("send_cb", "Send FAIL");
  }
}

// void toggleLED(void *parameters)
// {
//   DTO *dto = new DTO();
//   dto->breaks = false;
//   // uint8_t *data = {};
//   while (1)
//   {
//     gpio_set_level(GPIO_NUM_2, 1);
//     dto->breaks = true;
//     // xthal_memcpy(data, dto, sizeof(DTO));
//     ESP_ERROR_CHECK(esp_now_send(rx_address, (uint8_t*)dto, sizeof(DTO)));
//     vTaskDelay(1000 / portTICK_PERIOD_MS);
//     gpio_set_level(GPIO_NUM_2, 0);
//     dto->breaks = false;
//     ESP_ERROR_CHECK(esp_now_send(rx_address, (uint8_t*)dto, sizeof(DTO)));
//     vTaskDelay(500 / portTICK_PERIOD_MS);
//   }
// }

extern "C" void app_main(void)
{
  gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  config_wifi();

  ESP_LOGI("app_main", "Config wifi end");

  now_init(send_cb, rx_address);

  ESP_LOGI("app_main", "Config esp-now end");

  esp_event_loop_args_t loop_args = {
      .queue_size = 5,
      .task_name = "loop_task", // task will be created
      .task_priority = uxTaskPriorityGet(NULL),
      .task_stack_size = 3072,
      .task_core_id = tskNO_AFFINITY};


  ESP_ERROR_CHECK(esp_event_loop_create(&loop_args, &loop_handle));

  ESP_ERROR_CHECK(esp_event_handler_register_with(loop_handle, ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, run_on_event, NULL));
  server.setup_server(rx_address, &loop_handle);

}
