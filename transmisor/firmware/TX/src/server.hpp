#include <esp_https_server.h>
#include <esp_log.h>

esp_err_t send_web_page(httpd_req_t *req);
esp_err_t get_req_handler(httpd_req_t *req);
httpd_handle_t setup_server(void);