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
#include "esp_spiffs.h"
// #include "dto.hpp"

#define LEFT_TURN_OUTPUT GPIO_NUM_22
#define RIGHT_TURN_OUTPUT GPIO_NUM_21
#define BREAK_OUTPUT GPIO_NUM_17
#define REVERSE_OUTPUT GPIO_NUM_16


// THIS MAC = e4:65:b8:4a:11:55
// other mac = e4:65:b8:4a:0b:f0 sta f1 ap

uint8_t rx_address[6] = {0xE4, 0x65, 0xB8, 0x4A, 0x11, 0x54};
const int QUEUE_SIZE = 5;
EventGroupHandle_t s_wifi_event_group;
esp_event_loop_handle_t loop_handle;
Server server;

ESP_EVENT_DEFINE_BASE(LIGHT_EVENT);

void set_output(DTO *dto)
{
  if (dto->left)
  {
    gpio_set_level(LEFT_TURN_OUTPUT, 1);
  }
  else
  {
    gpio_set_level(LEFT_TURN_OUTPUT, 0);
  }

  if (dto->right)
  {
    gpio_set_level(RIGHT_TURN_OUTPUT, 1);
  }
  else
  {
    gpio_set_level(RIGHT_TURN_OUTPUT, 0);
  }

  if (dto->breaks)
  {
    gpio_set_level(BREAK_OUTPUT, 1);
  }
  else
  {
    gpio_set_level(BREAK_OUTPUT, 0);
  }

  if (dto->reverse)
  {
    gpio_set_level(REVERSE_OUTPUT, 1);
  }
  else
  {
    gpio_set_level(REVERSE_OUTPUT, 0);
  }
}

void run_on_event(void *handler_arg, esp_event_base_t base, int32_t id, void *event_data)
{
  ESP_LOGI("EVENT_HANDLE", "event recieved");
  DTO* dto = (DTO*)event_data;
  set_output(dto);
  ESP_ERROR_CHECK(esp_now_send(rx_address, (uint8_t*)dto, sizeof(DTO)));
}

static void send_cb(const esp_now_send_info_t *mac_addr, esp_now_send_status_t status)
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

extern "C" void app_main(void)
{
  gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
  gpio_set_direction(LEFT_TURN_OUTPUT, GPIO_MODE_OUTPUT);
  gpio_set_direction(RIGHT_TURN_OUTPUT, GPIO_MODE_OUTPUT);
  gpio_set_direction(BREAK_OUTPUT, GPIO_MODE_OUTPUT);
  gpio_set_direction(REVERSE_OUTPUT, GPIO_MODE_OUTPUT);

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  esp_vfs_spiffs_conf_t config = {
    .base_path = "/storage",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true
  };

  ESP_ERROR_CHECK(esp_vfs_spiffs_register(&config));

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
