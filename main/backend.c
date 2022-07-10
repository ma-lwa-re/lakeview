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
#include "backend.h"
#include "esp_log.h"
#include "esp_http_client.h"

static const char *BACKEND_TAG = "backend";

static char *local_response_buffer = "";//[MAX_HTTP_OUTPUT_BUFFER] = {0};

esp_http_client_config_t backend_config = {
    .url = UBIDOTS_BASE_URL,
    /*
    * Certificate:
    *     Data:
    *         Version: 3 (0x2)
    *         Serial Number:
    *             82:10:cf:b0:d2:40:e3:59:44:63:e0:bb:63:82:8b:00
    *         Signature Algorithm: sha256WithRSAEncryption
    *         Issuer: C = US, O = Internet Security Research Group, CN = ISRG Root X1
    *         Validity
    *             Not Before: Jun  4 11:04:38 2015 GMT
    *             Not After : Jun  4 11:04:38 2035 GMT
    *         Subject: C = US, O = Internet Security Research Group, CN = ISRG Root X1
    */
    .cert_pem = "-----BEGIN CERTIFICATE-----\n"
                "MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
                "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
                "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
                "WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
                "ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
                "MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
                "h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n"
                "0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n"
                "A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n"
                "T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n"
                "B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n"
                "B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n"
                "KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n"
                "OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n"
                "jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n"
                "qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n"
                "rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n"
                "HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n"
                "hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n"
                "ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n"
                "3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n"
                "NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n"
                "ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n"
                "TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n"
                "jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n"
                "oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n"
                "4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n"
                "mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n"
                "emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n"
                "-----END CERTIFICATE-----",
    .user_agent = BACKEND_USER_AGENT,
    //.user_data = local_response_buffer,
    .method = HTTP_METHOD_POST
};

bool send_ubidots_data(const char *variable_id, const char *key, const float value) {
    char *api_endpoint;
    char *post_field;
    asprintf(&post_field, "{\"%s\": %.1f}", key, value);
    asprintf(&api_endpoint, "%s%s%s/values", UBIDOTS_BASE_URL, UBIDOTS_API_ENDPOINT_VARIABLE, variable_id);
    esp_http_client_handle_t client = esp_http_client_init(&backend_config);

    ESP_ERROR_CHECK(esp_http_client_set_url(client, api_endpoint));
    ESP_ERROR_CHECK(esp_http_client_set_header(client, "Content-Type", "application/json"));
    ESP_ERROR_CHECK(esp_http_client_set_header(client, "X-Auth-Token", UBIDOTS_AUTH_TOKEN));
    ESP_ERROR_CHECK(esp_http_client_set_post_field(client, post_field, strlen(post_field)));

    esp_err_t err = esp_http_client_perform(client);

    if(err != ESP_OK) {
        return false;
    }

    HttpStatus_Code status_code = esp_http_client_get_status_code(client);
    ESP_ERROR_CHECK(esp_http_client_cleanup(client));

    if((status_code & HttpStatus_Ok) == HttpStatus_Ok) {
        ESP_LOGI(BACKEND_TAG, "Ubidots HTTP status code %d", status_code);
    } else {
        ESP_LOGW(BACKEND_TAG, "Ubidots HTTP status code %d", status_code);
    }

    return (status_code & HttpStatus_Ok) == HttpStatus_Ok ? true : false;
}

bool send_elastic_data(const char *key, const float value) {
    char *post_field;
    asprintf(&post_field, "{\"%s\": %.1f}", key, value);
    esp_http_client_handle_t client = esp_http_client_init(&backend_config);

    ESP_ERROR_CHECK(esp_http_client_set_url(client, ELASTIC_ENDPOINT_URL));
    ESP_ERROR_CHECK(esp_http_client_set_post_field(client, post_field, strlen(post_field)));

    esp_err_t err = esp_http_client_perform(client);

    if(err != ESP_OK) {
        return false;
    }

    HttpStatus_Code status_code = esp_http_client_get_status_code(client);
    ESP_ERROR_CHECK(esp_http_client_cleanup(client));

    if((status_code & HttpStatus_Ok) == HttpStatus_Ok) {
        ESP_LOGI(BACKEND_TAG, "Elastic HTTP status code %d", status_code);
    } else {
        ESP_LOGW(BACKEND_TAG, "Elastic HTTP status code %d", status_code);
    }

    return (status_code & HttpStatus_Ok) == HttpStatus_Ok ? true : false;
}
