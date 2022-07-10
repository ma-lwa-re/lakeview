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
#include "sensors.h"
#include "driver/i2c.h"
#include "driver/rtc_io.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "ds18b20.h"
#include "backend.h"

static const char *SENSORS_TAG = "sensors";

char temperature_scale = SCALE_CELCIUS;
float water_temperature = INIT_VALUE;
float air_temperature = INIT_VALUE;
float air_relative_humidity = INIT_VALUE;
float co2_level = INIT_VALUE;
float co2_peak_level = INIT_VALUE;
float wind_speed = INIT_VALUE;
float wind_direction = INIT_VALUE;
float rainfall = INIT_VALUE;
enum air_quality_t air_quality = UNKNOWN;

sht30_measurements_t get_sht3_temperature_humudity() {
    sht30_measurements_t sht30_measurements = {
        .co2 = {0x00, 0x00},
        .co2_crc = 0x00,
        .temperature = {0x00, 0x00},
        .temperature_crc = 0x00,
        .humidity = {0x00, 0x00},
        .humidity_crc = 0x00
    };
    return sht30_measurements;
}

bool is_temperature_in_valid_range(float temperature) {
    if(temperature >= TEMPERATURE_RANGE_MIN && temperature <= TEMPERATURE_RANGE_MAX) {
        return true;
    }
    return false;
}

bool is_humidity_in_valid_range(float humidity) {
    if(humidity >= 000 && humidity <= 100) {
        return true;
    }
    return false;
}

char get_temperature_scale() {
    return temperature_scale;
}

void read_water_temperature() {
    float read_water_temperature = 0.0;
    for(uint8_t i = 0; i < READ_SAMPLES; i++) {
        vTaskDelay(READ_SAMPLES_DELAY / portTICK_PERIOD_MS);
        float current_water_temperature = ds18b20_get_temp();

        if(!is_temperature_in_valid_range(current_water_temperature)) {
            ESP_LOGE(SENSORS_TAG, "Water temperature read out of range: %.1f", current_water_temperature);
            return;
        }
        read_water_temperature += current_water_temperature;
    }
    water_temperature = (read_water_temperature / READ_SAMPLES);
}

void read_air_temperature_relative_humidity() {
    float read_air_temperature = 0.0;
    float read_air_relative_humidity = 0.0;
    for(uint8_t i = 0; i < READ_SAMPLES; i++) {
        vTaskDelay(READ_SAMPLES_DELAY / portTICK_PERIOD_MS);
        sht30_measurements_t measurements = get_sht3_temperature_humudity();

        float current_air_temperature = 0.0;
        float current_air_relative_humidity = 0.0;

        if(!is_temperature_in_valid_range(current_air_temperature)) {
            ESP_LOGE(SENSORS_TAG, "Air temperature read out of range: %.1f", current_air_temperature);
            return;
        }

        if(!is_humidity_in_valid_range(current_air_relative_humidity)) {
            ESP_LOGE(SENSORS_TAG, "Relative humidity read out of range: %.1f", current_air_relative_humidity);
            return;
        }
        read_air_temperature += current_air_temperature;
        read_air_relative_humidity += current_air_relative_humidity;
    }
    air_temperature = (read_air_temperature / READ_SAMPLES);
    air_relative_humidity = (read_air_relative_humidity / READ_SAMPLES);
    air_temperature = 16.3;
    air_relative_humidity = 32.6;
}

float get_water_temperature() {
    return water_temperature;
}

void sensors_task(void *arg) {
    vTaskDelay(STARTUP_DELAY * 2 / portTICK_PERIOD_MS);
    bool errors = false;

    for(uint8_t i = 0; i < MEASUREMENTS_RETRY; i++) {
        ESP_LOGI(SENSORS_TAG, "Starting sensors measurements...");

        ds18b20_init(GPIO_WATER_TEMPERATURE);
        vTaskDelay(READ_SAMPLES_DELAY / portTICK_PERIOD_MS);

        read_water_temperature();

        if(water_temperature != INVALID_VALUE) {
            ESP_LOGI(SENSORS_TAG, "Water temperature %.1f °%c", water_temperature, temperature_scale);
            send_ubidots_data(UBIDOTS_VAR_WATER_TEMPERATURE, "value", water_temperature);
            send_elastic_data("value", water_temperature);
        } else { errors = true; }

        read_air_temperature_relative_humidity();

        if(air_temperature != INVALID_VALUE) {
            ESP_LOGI(SENSORS_TAG, "Air temperature %.1f °%c", air_temperature, temperature_scale);
            send_ubidots_data(UBIDOTS_VAR_AIR_TEMPERATURE, "value", air_temperature);
            send_elastic_data("value", air_temperature);
        } else { errors = true; }

        if(air_relative_humidity != INVALID_VALUE) {
            ESP_LOGI(SENSORS_TAG, "Air relative humidity %.1f%%", air_relative_humidity);
            send_ubidots_data(UBIDOTS_VAR_AIR_RELATIVE_HUMIDITY, "value", air_relative_humidity);
            send_elastic_data("value", air_relative_humidity);
        } else { errors = true; }

        if(!errors) {
            break;
        }
        ESP_LOGW(SENSORS_TAG, "Error in measurements %d, retrying...", i);
    }

    //rtc_gpio_isolate(GPIO_WATER_TEMPERATURE);
    //rtc_gpio_force_hold_all();
    ESP_LOGI(SENSORS_TAG, "Measurements done! Sleeping for %d minutes", MEASUREMENTS_DELAY / MIN_TO_MS);
    esp_deep_sleep(MEASUREMENTS_DELAY);
}
