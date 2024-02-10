/* Header file for clock configuration */

#ifndef AUTOGEN_CONFIG_CLOCK_H

#include "task_display.h"
#include "rgb_rmt.h"

extern const CLOCK_WORD     CLOCK_words_S[];
extern const UINT8          CLOCK_num_words_u8;
extern const UINT8          CLOCK_xy_pixel_u8[10][10];

#define WC_RGB_LED_PIN      (7)
#define WC_RGB_LED_COUNT    (100)
#define WC_RGB_LED_TYPE     (LED_WS2812B_V1)

#define AUTOGEN_CONFIG_CLOCK_H
#endif