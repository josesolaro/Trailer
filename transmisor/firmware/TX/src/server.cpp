#include "server.hpp"


// char off_resp[] = "<!DOCTYPE html><html><head></head><body>  HELLO WOLRD BYE WORLD</body></html>";
const char html_content[] = "<!DOCTYPE html> <html>  <head> <style type='text.css'> .content{     display: flex;     flex-direction: row; } </style> <script type='application/javascript'> function toggleBreak(){     console.log('about to call');     var xhttp = new XMLHttpRequest();     xhttp.open('GET', 'http://192.168.4.1/togglebreak');     xhttp.send(); } </script> </head>  <body>     <div class='content'>         <h1>HELLO WOLRD BYE WORLD</h1>         <button type='button' onclick='toggleBreak()'>Toggle</button>     </div> </body>  </html>";
uint8_t esp_now[6];
int status = 0;

esp_err_t Server::send_web_page(httpd_req_t *req)
{
    int response;
    response = httpd_resp_send(req, html_content, HTTPD_RESP_USE_STRLEN);
    return response;
}
esp_err_t Server::get_req_handler(httpd_req_t *req)
{
    Server *ctx = (Server*)req->user_ctx;
    return ctx->send_web_page(req);
}

esp_err_t Server::toggleBreak(httpd_req_t *req)
{
    Server *ctx = (Server*)req->user_ctx;
    DTO *dto = new DTO();
    dto->left = false;
    dto->right = false;
    dto->reverse = false;
    if (ctx->status == 1)
    {
        dto->breaks = false;
        ctx->status = 0;
    }
    else
    {
        dto->breaks = true;
        ctx->status = 1;
    }
    ESP_LOGI("HTTP_SERVER", "sending...");
    ESP_ERROR_CHECK(esp_event_post_to(*(ctx->appEvent), LIGHT_EVENT, 0, dto, sizeof(DTO), 50));
    // bool v = ctx->status == 1;
    // ESP_ERROR_CHECK(esp_event_post_to(*(ctx->appEvent), LIGHT_EVENT, EVENT_ID_1, &v, sizeof(v), portMAX_DELAY));
    return httpd_resp_send(req, nullptr, 0);
}


httpd_handle_t Server::setup_server(uint8_t rx_address[6], esp_event_loop_handle_t *appEvent)
{
    this->appEvent = appEvent;
    httpd_uri_t uri_get = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = Server::get_req_handler,
        .user_ctx = this
        };

    httpd_uri_t uri_toggle_break = {
        .uri = "/togglebreak",
        .method = HTTP_GET,
        .handler = Server::toggleBreak,
        .user_ctx = this
        };

    xthal_memcpy(esp_now, rx_address, 6);
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    ESP_LOGI("HTTP_SERVER", "Setting up server");
    esp_err_t http_server = httpd_start(&server, &config);
    if (http_server == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_toggle_break);
    }
    else
    {
        ESP_ERROR_CHECK(http_server);
    }

    return server;
}