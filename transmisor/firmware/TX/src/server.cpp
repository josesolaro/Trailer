#include "server.hpp"

char off_resp[] = "<!DOCTYPE html><html><head></head><body>  HELLO WOLRD BYE WORLD</body></html>";
esp_err_t send_web_page(httpd_req_t *req)
{
    int response;
    response = httpd_resp_send(req, off_resp, HTTPD_RESP_USE_STRLEN);
    return response;
}
esp_err_t get_req_handler(httpd_req_t *req)
{
    return send_web_page(req);
}

httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_req_handler,
    .user_ctx = NULL
    };

httpd_handle_t setup_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    ESP_LOGI("HTTP_SERVER","Setting up server");
    esp_err_t http_server = httpd_start(&server, &config);
    if (http_server == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
    }else{
      ESP_ERROR_CHECK(http_server);
    }

    return server;
}