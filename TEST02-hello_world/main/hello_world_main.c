/* Hello World Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"

void hello_task(void *pvParameter)
{
    printf("\n****************"); 
    printf("\n* Hello world! *"); 
    printf("\n****************");
    fflush(stdout);
    int i = 30;
    printf("\n# Restarting in %d seconds ", i); fflush(stdout);
    for (int j=i; j >= 0; j--) {
        printf("."); fflush(stdout);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
    printf("\nEnd of Process.\n\n"); fflush(stdout);
    esp_restart();
}

void app_main()
{
    xTaskCreate(&hello_task, "hello_task", 2048, NULL, 5, NULL);
}
