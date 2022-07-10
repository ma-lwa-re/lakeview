/* MIT License
*
* Copyright (c) 2022 ma-lwa-re
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "wifi.h"
#include "sensors.h"
#if defined(OTA_UPDATES_ON)
#include "ota.h"
#endif
#include "esp_log.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_sleep.h"
#include "esp_chip_info.h"

#define UART_STACK_SIZE         (4096)
#define OTA_STACK_SIZE          (UART_STACK_SIZE * 2)

static const char *LAKEVIEW_TAG = "lakeview";

void chip_info() {
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
  
    ESP_LOGI(LAKEVIEW_TAG, "%s with %d CPU cores, WiFi%s%s, revision %d, %dMB %s flash",
        CONFIG_IDF_TARGET, chip_info.cores, (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
        (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "", chip_info.revision,
        spi_flash_get_chip_size() / (1024 * 1024),
        (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external"
    );
}

void memory_init() {
    esp_err_t flash_error = nvs_flash_init();
    if(flash_error == ESP_ERR_NVS_NO_FREE_PAGES || flash_error == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        flash_error = nvs_flash_init();
    }
    ESP_ERROR_CHECK(flash_error);
}

void app_main() {
    esp_log_level_set("*", ESP_LOG_INFO);
    ESP_LOGI(LAKEVIEW_TAG, "Hello there!");
    ESP_LOGI(LAKEVIEW_TAG, "Wakeup cause %d", esp_sleep_get_wakeup_cause());

    chip_info();
    memory_init();

    app_wifi_credentials();
    app_wifi_init();
    app_wifi_connect();

    xTaskCreate(sensors_task, "sensors_task", UART_STACK_SIZE, NULL, configMAX_PRIORITIES-3, NULL);

    #if defined(OTA_UPDATES_ON)
    const esp_partition_t *running_partition = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;

    if(esp_ota_get_state_partition(running_partition, &ota_state) == ESP_OK) {
        ESP_LOGI(LAKEVIEW_TAG, "Booting partition %s @ 0x%2X 0x%2X %sencrypted", running_partition->label,
                 running_partition->address, running_partition->size, running_partition->encrypted ? "" : "not ");

        if(ota_state == ESP_OTA_IMG_VALID) {
            ESP_LOGI(LAKEVIEW_TAG, "Running OTA app is valid, boot successful");
        } else if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {

            if(esp_ota_mark_app_valid_cancel_rollback() == ESP_OK) {
                ESP_LOGI(LAKEVIEW_TAG, "Running OTA app is valid, rollback cancelled successfully!");
            } else {
                ESP_LOGE(LAKEVIEW_TAG, "Failed to cancel rollback!");
                esp_ota_mark_app_invalid_rollback_and_reboot();
            }
        }
    }
    xTaskCreate(ota_task, "ota_task", OTA_STACK_SIZE, NULL, configMAX_PRIORITIES-8, NULL);
    #endif

    /*gpio_config(&(gpio_config_t){
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = ((1ULL << LED_STATUS) | (1ULL << LED_ACTIVITY))
    });

    gpio_set_level(LED_ACTIVITY, OFF);
    gpio_set_level(LED_STATUS, ON);*/
}
