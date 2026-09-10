/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

#include "tool.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "NFC";

void Sleep(unsigned int ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void PrintBuf(unsigned char *prefix, unsigned char *buf, unsigned int len) {
    char line[128];
    int pos = snprintf(line, sizeof(line), "%s", (char*)prefix);
    int n = (len < 30) ? len : 30;
    for (int i = 0; i < n; i++) {
        pos += snprintf(line + pos, sizeof(line) - pos, "%02x ", buf[i]);
    }
    if (len >= 30) snprintf(line + pos, sizeof(line) - pos, "...");
    ESP_LOGI(TAG, "%s", line);
}