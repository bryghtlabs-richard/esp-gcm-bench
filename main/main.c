/*
 * Author: Richard Allen @ Bryght Labs
 *
 * Created in 2024
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include <xtensa/hal.h>

#include "esp_log.h"
#include "esp_attr.h"

#include "gcm_funcs.h"

#define TAG "main"


IRAM_ATTR __attribute__((noinline))  void gcmDemo(){
    esp_gcm_context ctx = {};
    uint8_t input[16];
    uint64_t H_SMALL[16][2];
    uint64_t H_LARGE[256][2];
    for(unsigned i = 0; i <16; ++i){
        input[i] = i;
        ctx.HH[i] = i;
        H_SMALL[i][0] = i;
        ctx.HL[i] = 16 + i;
        H_SMALL[i][1] = 16 + i;
    }
    for(unsigned i = 0; i < 256; ++i){
        H_LARGE[i][0] = i;
        H_LARGE[i][1] = i + 256;
    }
    uint8_t output_old[16];
    uint8_t output_richard[16];
    uint8_t output_smalltable[16];
    uint8_t output_largetable[16];
    uint32_t t0 = xthal_get_ccount();
    uint32_t t1 = xthal_get_ccount();
    gcm_mult_old( &ctx, input, output_old);
    uint32_t t2 = xthal_get_ccount();
    gcm_mult_smalltable(output_smalltable, input, H_SMALL);
    uint32_t t3 = xthal_get_ccount();
    gcm_mult_largetable(output_largetable, input, H_LARGE);
    uint32_t t4 = xthal_get_ccount();
    gcm_mult_richard(&ctx, input, output_richard);
    uint32_t t5 = xthal_get_ccount();

    uint32_t tbase = t1-t0;
    uint32_t tmbedold       = t2 - t1 - tbase;
    uint32_t tmbednewsmall  = t3 - t2 - tbase;
    uint32_t tmbednewlarge  = t4 - t3 - tbase;
    uint32_t trichard       = t5 - t4 - tbase;

    ESP_LOGI(TAG, "Timing report:"
        "\n\tcMbedOld:%" PRIu32 "(%.1fc/B)"
        "\n\tcMbedSmall:%" PRIu32 "(%.1fc/B)"
        "\n\tcMbedLarge:%" PRIu32 "(%.1fc/B)"
        "\n\tcRichard:%" PRIu32 "(%.1fc/B)",
        tmbedold, tmbedold/16.0,
        tmbednewsmall, tmbednewsmall/16.0,
        tmbednewlarge, tmbednewlarge/16.0,
        trichard, trichard/16.0);
}

void app_main(void)
{
    while (1) {
        gcmDemo();
        vTaskDelay(10000);
    }
}
