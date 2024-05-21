/* Header file for clock configuration */

#ifndef AUTOGEN_CONFIG_CLOCK_H

#include "rgb_rmt.h"
#include "time.h"

#define MAX_WORD_LENGTH (10)

typedef enum{
    WORD_PREFIX         = 0x01,
    WORD_CUSTOM         = 0x02,
    WORD_HOUR           = 0x04,
    WORD_SUFFIX         = 0x08
} CLOCK_WORD_TYPE;

typedef struct{
    STRING              word_str;
    UINT8               word_length_u8;
    CLOCK_WORD_TYPE     word_type_E;
    UINT8               word_pixels_u8[ MAX_WORD_LENGTH ];
} CLOCK_WORD;

typedef struct{
    struct tm           last_time_s;
} CLOCK_CONFIG;

extern const CLOCK_WORD     CLOCK_words_S[];
extern const UINT8          CLOCK_num_words_u8;
extern const UINT8          CLOCK_xy_pixel_u8[10][10];

#define WC_RGB_LED_PIN      (7)
#define WC_RGB_LED_COUNT    (100)
#define WC_RGB_LED_TYPE     (LED_WS2812B_V1)

#define AUTOGEN_CONFIG_CLOCK_H
#endif