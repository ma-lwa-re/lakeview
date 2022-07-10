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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define UBIDOTS_BASE_URL                    ("https://industrial.api.ubidots.com")
#define UBIDOTS_AUTH_TOKEN                  ("")
#define UBIDOTS_API_ENDPOINT_VARIABLE       ("/api/v1.6/variables/")
#define UBIDOTS_VAR_WATER_TEMPERATURE       ("")
#define UBIDOTS_VAR_AIR_TEMPERATURE         ("")
#define UBIDOTS_VAR_AIR_RELATIVE_HUMIDITY   ("")
#define ELASTIC_ENDPOINT_URL                ("http://192.168.1.92:9200/lakeview/_bulk")
#define BACKEND_USER_AGENT                  ("ESP32 HTTP Client/1.0 - " PROJECT_NAME " v" PROJECT_VER)
#define MAX_HTTP_OUTPUT_BUFFER              (2048)

bool send_ubidots_data(const char *variable, const char *key, const float value);

bool send_elastic_data(const char *key, const float value);
