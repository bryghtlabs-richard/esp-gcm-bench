#pragma once
#include "aes/esp_aes_gcm.h"

void gcm_mult_old( esp_gcm_context *ctx, const unsigned char x[16],
                      unsigned char output[16] );
void gcm_mult_largetable(uint8_t *output, const uint8_t *x, uint64_t H[256][2]);
void gcm_mult_smalltable(uint8_t *output, const uint8_t *x, uint64_t H[16][2]);
void gcm_mult_richard( esp_gcm_context *ctx, const unsigned char x[16],
                      unsigned char output[16] );