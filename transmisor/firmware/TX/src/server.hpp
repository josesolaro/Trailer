#include <esp_https_server.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include "dto.hpp"

ESP_EVENT_DECLARE_BASE(LIGHT_EVENT); 
enum {
    EVENT_ID_1,
};

class Server{
    public:
        httpd_handle_t setup_server(uint8_t rx_address[6], esp_event_loop_handle_t *appEvent);

    private:
        esp_err_t send_web_page(httpd_req_t *req);
        static esp_err_t get_req_handler(httpd_req_t *req);
        static esp_err_t toggleBreak(httpd_req_t *req);

    private:
        const char* html_content = "<!DOCTYPE html> <html>  <head> <style type='text.css'> .content{     display: flex;     flex-direction: row; } </style> <script type='application/javascript'> function toggleBreak(){     console.log('about to call');     var xhttp = new XMLHttpRequest();     xhttp.open('GET', 'http://192.168.4.1/togglebreak');     xhttp.send(); } </script> </head>  <body>     <div class='content'>         <h1>HELLO WOLRD BYE WORLD</h1>         <button type='button' onclick='toggleBreak()'>Toggle</button>     </div> </body>  </html>";
        uint8_t esp_now[6];
        int status = 0;
        esp_event_loop_handle_t *appEvent;
};