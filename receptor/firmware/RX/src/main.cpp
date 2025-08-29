#include <now.hpp>
#include <driver/gpio.h>

const int QUEUE_SIZE = 5;
const int MAX_DElAY = 512;

ESP_EVENT_DECLARE_BASE(LIGHT_EVENT); 
enum {
    EVENT_ID_1,
};

ESP_EVENT_DEFINE_BASE(LIGHT_EVENT);

#define LEFT_TURN_OUTPUT GPIO_NUM_22
#define RIGHT_TURN_OUTPUT GPIO_NUM_21
#define BREAK_OUTPUT GPIO_NUM_17
#define REVERSE_OUTPUT GPIO_NUM_16

uint8_t tx_address[6] = {0xE4, 0x65, 0xB8, 0x4A, 0x0B, 0xF1};


static esp_event_loop_handle_t loop_handle;

void set_output(DTO* dto){
    if(dto->left){
        gpio_set_level(LEFT_TURN_OUTPUT, 1);
    }
    else
    {
        gpio_set_level(LEFT_TURN_OUTPUT, 0);
    }
    if(dto->right){
        gpio_set_level(RIGHT_TURN_OUTPUT, 1);
    }
    else
    {
        gpio_set_level(RIGHT_TURN_OUTPUT, 0);
    }
    if(dto->reverse){
        gpio_set_level(REVERSE_OUTPUT, 1);
    }
    else
    {
        gpio_set_level(REVERSE_OUTPUT, 0);
    }
    if (dto->breaks)
    {
        gpio_set_level(BREAK_OUTPUT, 1);
    }
    else
    {
        gpio_set_level(BREAK_OUTPUT, 0);
    }
}

void recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{
    DTO* dto = (DTO*)data;
    esp_err_t ret = esp_event_post_to(loop_handle, LIGHT_EVENT, 0, dto, sizeof(DTO), 50);
    if(ret != ESP_OK){
      ESP_LOGE("recv_cb", "Send send queue fail");
    }
}

void run_on_event(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data)
{
    DTO* dto = (DTO*)event_data;
    set_output(dto);
}


EventGroupHandle_t s_wifi_event_group;
extern "C" void app_main(void)
{
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    gpio_set_direction(LEFT_TURN_OUTPUT, GPIO_MODE_OUTPUT);
    gpio_set_direction(RIGHT_TURN_OUTPUT, GPIO_MODE_OUTPUT);
    gpio_set_direction(BREAK_OUTPUT, GPIO_MODE_OUTPUT);
    gpio_set_direction(REVERSE_OUTPUT, GPIO_MODE_OUTPUT);

    set_output(new DTO{false, false, false, false});
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    set_output(new DTO{true, true, false, false});
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    set_output(new DTO{true, true, true, false});
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    set_output(new DTO{false, false, false, false});

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    now_init(recv_cb, tx_address);
    
    esp_event_loop_args_t loop_args = {
        .queue_size = QUEUE_SIZE,
        .task_name = "loop_task",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 3072,
        .task_core_id = tskNO_AFFINITY
    };

    ESP_ERROR_CHECK(esp_event_loop_create(&loop_args, &loop_handle));

    ESP_ERROR_CHECK(esp_event_handler_register_with(loop_handle, ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, run_on_event, nullptr));
    
    ESP_LOGI("app_main", "Config esp-now end");
}