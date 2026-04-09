/*
* NAME: Abhiram Vadali
* DATE: 04/07/2026
* PURPOSE: A low-level script that initializes Wi-Fi and queries the Carbon Aware SDK
*/

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define WIFI_SSID "YOUR_SSID" 
#define WIFI_PASS "YOUR_PASS" 
#define ERR_GPIO 7
static const char *TAG = "wifi_app";

static void ERROR(void) {
    gpio_reset_pin(ERR_GPIO);
    gpio_set_direction(ERR_GPIO, GPIO_MODE_OUTPUT);

    while (1) {
        gpio_set_level(ERR_GPIO, 1);          
        vTaskDelay(pdMS_TO_TICKS(100));       
        gpio_set_level(ERR_GPIO, 0);         
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ERROR(); 
    }
}

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) ERROR();

    if (esp_netif_init() != ESP_OK) ERROR();
    if (esp_event_loop_create_default() != ESP_OK) ERROR();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    if (esp_wifi_init(&cfg) != ESP_OK) ERROR();

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    
    if (esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK) ERROR();
    if (esp_wifi_set_config(WIFI_IF_STA, &wifi_config) != ESP_OK) ERROR();
    if (esp_wifi_start() != ESP_OK) ERROR();
}

static void SDK_query(void) {
}
