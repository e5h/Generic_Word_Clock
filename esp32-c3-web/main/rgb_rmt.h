/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * NAME:
 *      rgb_rmt.h
 *
 * PURPOSE:
 *      This module is designed to provide simple functions to control RGB leds
 *      using the RMT peripheral of the ESP32.
 *
 * DEPENDENCIES:
 *      ESP32 GPIO
 *      ESP32 RMT
 *
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
 * (C) Andrew Bright 2023, github.com/e5h
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

#ifndef WC_RGB_RMT_H

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ Include Files ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

#include "lib_includes.h"

#include "driver/gpio.h"
#include "driver/rmt_encoder.h"
#include "driver/rmt_tx.h"

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ GLOBAL : Constants and Types ][~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

#define RGB_LED_HZ          (10000000)  /* 10Mhz = 0.1us ticks */
#define RGB_LED_PIN         WC_RGB_LED_PIN
#define RGB_LED_COUNT       WC_RGB_LED_COUNT
#define RGB_LED_TYPE        WC_RGB_LED_TYPE

/**
 * Struct to hold a brightness adjustable color
 * 
 * Each double value is 0.0 - 1.0, representing 0 - 100% of that
 * color. This is used to multiply the result of a 0 - 255 valued
 * gamma corrected lookup table. Lookup table indices are the
 * brightness (0 - 100).
 */
typedef struct {
    double r;
    double g;
    double b;
} RGB_COLOR_PCT;

typedef enum{
    LED_WS2812_V1 = 0,
    LED_WS2812B_V1,
    LED_WS2812B_V2,
    LED_WS2812B_V3,
    LED_WS2813_V1,
    LED_WS2813_V2,
    LED_WS2813_V3,
    LED_SK6812_V1,
    LED_PI554FCH,
} RGB_LED_TYPE_E;

/* Struct to defined RGB LED parameters */
typedef struct{
    INT32               pixel_sz_bytes; /* Size of a pixel in bytes (usually 3 or 4) */
    double              T0H;            /* Time (us) spent high for an encoded "0 bit" */
    double              T1H;            /* Time (us) spent high for an encoded "1 bit" */
    double              T0L;            /* Time (us) spent low for an encoded "0 bit" */
    double              T1L;            /* Time (us) spent low for an encoded "1 bit" */
    double              TRS;            /* Time (us) spent low to reset */
} RGB_LED_TYPE_PARAMS;

typedef enum{
    COLOR_Red = 0,
    COLOR_Orange,
    COLOR_Yellow,
    COLOR_Lime,
    COLOR_Green,
    COLOR_Mint,
    COLOR_Cyan,
    COLOR_Azure,
    COLOR_Blue,
    COLOR_Purple,
    COLOR_Pink,
    COLOR_Rose,

    /* Number of default colors */
    NUM_DEFAULT_COLORS,
} RGB_LED_DEFAULT_COLOR_E;

/* Array to access */
extern const RGB_COLOR_PCT RGB_LED_default_colors_S[];

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ LOCAL : Constants and Types ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ GLOBAL : Exportable Variables ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ GLOBAL : Exportable Function Prototypes ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/* Core functions */
extern STATUS_E RGB_LED_Init();
extern STATUS_E RGB_LED_SetPixelColor(INT32 index, RGB_COLOR_PCT color, UINT8 brightness);
extern STATUS_E RGB_LED_ModifyPixelBrightness(INT32 index, UINT8 brightness);
extern STATUS_E RGB_LED_TransmitColors();
extern STATUS_E RGB_LED_Wipe();

#define WC_RGB_RMT_H
#endif