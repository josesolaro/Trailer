#include <now.hpp>
#include <driver/gpio.h>

// this mac e4:65:b8:4a:0b:f0 =sta f1 = ap
// other mac e4:65:b8:4a:11:55
const int QUEUE_SIZE = 5;
const int MAX_DElAY = 512;
const esp_event_base_t EVENT_TYPE = "light";

// e4:65:b8:4a:11:55
uint8_t tx_address[6] = {0xE4, 0x65, 0xB8, 0x4A, 0x11, 0x55};

static esp_event_loop_handle_t loop_handle;

void recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{
    DTO* dto = (DTO*)data;
   gpio_set_level(GPIO_NUM_2, dto->breaks);
    esp_err_t ret = esp_event_post_to(loop_handle, EVENT_TYPE, 0, dto, sizeof(DTO), 50);
    if(ret != ESP_OK){
      ESP_LOGE("recv_cb", "Send send queue fail");
    }
}

void run_on_event(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data)
{
    DTO* dto = (DTO*)event_data;
    gpio_set_level(GPIO_NUM_2, dto->breaks);
}


EventGroupHandle_t s_wifi_event_group;
extern "C" void app_main(void)
{
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    now_init(recv_cb, tx_address);
    
    esp_event_loop_args_t loop_args = {
        .queue_size = QUEUE_SIZE,
        .task_name = NULL
    };

    esp_event_loop_create(&loop_args, &loop_handle);

    esp_event_handler_register_with(loop_handle, EVENT_TYPE, 0, run_on_event, nullptr);

    
    ESP_LOGI("app_main", "Config esp-now end");
}