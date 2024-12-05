#include "server.hpp"

uint8_t esp_now[6];
int status = 0;

esp_err_t Server::send_web_page(httpd_req_t *req)
{
    int response;
    
    FILE* f = fopen("/storage/index.html", "r");
    if(f == NULL){
        ESP_LOGI("app_main", "failed to read");
        return httpd_resp_send(req, "FAIL TO READ FILE", HTTPD_RESP_USE_STRLEN);;
    }
    
    fseek(f, 0, SEEK_END);
    long numOfByte = ftell(f);
    fseek(f, 0, SEEK_SET);

    void *content = malloc(numOfByte);
    fread(content, numOfByte, 1, f);
    fclose(f);
    return httpd_resp_send(req, (char*)content, HTTPD_RESP_USE_STRLEN);
}
esp_err_t Server::get_req_handler(httpd_req_t *req)
{
    Server *ctx = (Server*)req->user_ctx;
    return ctx->send_web_page(req);
}

esp_err_t Server::toggleBreak(httpd_req_t *req)
{
    ESP_LOGI("server", "toggle break");
    Server *ctx = (Server*)req->user_ctx;
    ctx->dto.breaks = !ctx->dto.breaks;

    Server::sendEvent(&ctx->dto, ctx);

    return httpd_resp_send(req, nullptr, 0);
}

esp_err_t Server::left(httpd_req_t *req)
{
    ESP_LOGI("server", "left");
    Server *ctx = (Server*)req->user_ctx;
    ctx->dto.left = !ctx->dto.left;

    Server::sendEvent(&ctx->dto, ctx);
    
    return httpd_resp_send(req, nullptr, 0);
}

esp_err_t Server::right(httpd_req_t *req)
{
        ESP_LOGI("server", "right");
    Server *ctx = (Server*)req->user_ctx;
    ctx->dto.right = !ctx->dto.right;

    Server::sendEvent(&ctx->dto, ctx);
    
    return httpd_resp_send(req, nullptr, 0);
}

esp_err_t Server::reverse(httpd_req_t *req)
{
    ESP_LOGI("server", "reverse");
    Server *ctx = (Server*)req->user_ctx;
    ctx->dto.reverse = !ctx->dto.reverse;

    Server::sendEvent(&ctx->dto, ctx);
    
    return httpd_resp_send(req, nullptr, 0);
}


esp_err_t Server::sendEvent(DTO *dto, Server *ctx){
    return esp_event_post_to(*(ctx->appEvent), LIGHT_EVENT, 0, dto, sizeof(DTO), 50);
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
        .uri = "/break",
        .method = HTTP_GET,
        .handler = Server::toggleBreak,
        .user_ctx = this
        };
    
    httpd_uri_t uri_left = {
        .uri = "/left",
        .method = HTTP_GET,
        .handler = Server::left,
        .user_ctx = this
        };

    httpd_uri_t uri_right = {
        .uri = "/right",
        .method = HTTP_GET,
        .handler = Server::right,
        .user_ctx = this
        };

    httpd_uri_t uri_reverse = {
        .uri = "/reverse",
        .method = HTTP_GET,
        .handler = Server::reverse,
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
        httpd_register_uri_handler(server, &uri_left);
        httpd_register_uri_handler(server, &uri_right);
        httpd_register_uri_handler(server, &uri_reverse);
    }
    else
    {
        ESP_ERROR_CHECK(http_server);
    }

    return server;
}