/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * NAME:
 *      ESP_RGB.h
 *
 * PURPOSE:
 *      This module is designed to provide simple functions to control RGB leds
 *      using the RMT peripheral of the ESP32.
 *
 * DEPENDENCIES:
 *      template.h
 *
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
 * (C) Andrew Bright 2023, github.com/e5h
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

#ifndef NXN_ESPRGB_H
#define NXN_ESPRGB_H

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ Feature Switches ][~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

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

#define RGB_LED_HZ          (10000000)  /* 10Mhz = 100ns ticks */
#define RGB_LED_PIN         (4)         /* Connected to pin 4 */
#define RGB_LED_COUNT       (100)       /* 100 LEDs total */

typedef enum{
  LED_WS2812_V1,
  LED_WS2812B_V1,
  LED_WS2812B_V2,
  LED_WS2812B_V3,
  LED_WS2813_V1,
  LED_WS2813_V2,
  LED_WS2813_V3,
  LED_SK6812_V1,
  LED_PI554FCH,
} RGBLED_type;

/**
 * Struct to hold a brightness adjustable color
 * 
 * Each double value is 0.0 - 1.0, representing 0 - 100% of that
 * color. This is internally translated into a value of 0 - 255.
 */
typedef struct {
    double r;
    double g;
    double b;
} COLOR_pct;

/* Struct to defined RGB LED parameters */
typedef struct{
    INT32               pixel_sz_bytes; /* Size of a pixel in bytes (usually 3 or 4) */
    double              T0H;            /* Time (us) spent high for an encoded "0 bit" */
    double              T1H;            /* Time (us) spent high for an encoded "1 bit" */
    double              T0L;            /* Time (us) spent low for an encoded "0 bit" */
    double              T1L;            /* Time (us) spent low for an encoded "1 bit" */
    double              TRS;            /* Time (us) spent low to reset */
} RGBLED_params;

/* Predefined colors */
extern const COLOR_pct Red;
extern const COLOR_pct Orange;
extern const COLOR_pct Yellow;
extern const COLOR_pct Lime;
extern const COLOR_pct Green;
extern const COLOR_pct Mint;
extern const COLOR_pct Cyan;
extern const COLOR_pct Azure;
extern const COLOR_pct Blue;
extern const COLOR_pct Purple;
extern const COLOR_pct Pink;
extern const COLOR_pct Rose;

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
void RGBLED_Init();
void RGBLED_SetColor(INT32 index, COLOR_pct color, UINT8 brightness);
void RGBLED_TransmitColors();

#endif