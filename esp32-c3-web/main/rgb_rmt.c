/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * NAME:
 *      rgb_rmt.c
 *
 * PURPOSE:
 *      This module is designed to provide simple functions to control RGB leds
 *      using the RMT peripheral of the ESP32.
 *
 * DEPENDENCIES:
 *      ---
 *
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
 * (C) Andrew Bright 2023, github.com/e5h
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ Include Files ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

#include "rgb_rmt.h"

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ LOCAL : Constants and Types ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/* Struct for an RGBLED encoder */
typedef struct{
    rmt_encoder_t       base;
    rmt_encoder_t       *byte_encoder;
    rmt_encoder_t       *copy_encoder;
    rmt_symbol_word_t   reset_code;
    INT32               encoder_state;
} RGB_LED_ENCODER;

/* Struct to hold a 24-bit RGB encoded color */
typedef struct __attribute__ ((packed)){
    UINT8 red_U8;
    UINT8 green_U8;
    UINT8 blue_U8;
} RGB_COLOR_24BIT;

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ LOCAL : Variables ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

// Gamma brightness lookup table <https://victornpb.github.io/gamma-table-generator>
// gamma = 2.00 steps = 101 range = 0-255
static const UINT8 gamma_lut[101] = {
     0,   0,   0,   0,   0,   1,   1,   1,   2,   2,   3,   3,   4,   4,   5,   6,
     7,   8,   8,   9,  10,  11,  13,  14,  15,  16,  18,  19,  20,  22,  23,  25,
    27,  28,  30,  32,  34,  36,  38,  40,  42,  44,  46,  48,  50,  53,  55,  57,
    60,  62,  65,  68,  70,  73,  76,  79,  82,  85,  88,  91,  94,  97, 100, 103,
   107, 110, 113, 117, 120, 124, 127, 131, 135, 139, 142, 146, 150, 154, 158, 162,
   167, 171, 175, 179, 184, 188, 192, 197, 201, 206, 211, 215, 220, 225, 230, 235,
   240, 245, 250, 255, 255 };

/* All known timing parameters for common RGB LEDs */
static const RGB_LED_TYPE_PARAMS RGB_LED_all_params_S[] = {
    [LED_WS2812_V1]   = { .pixel_sz_bytes = 3, .T0H = 0.35, .T1H = 0.70, .T0L = 0.80, .T1L = 0.60, .TRS =  50.0},
    [LED_WS2812B_V1]  = { .pixel_sz_bytes = 3, .T0H = 0.35, .T1H = 0.90, .T0L = 0.90, .T1L = 0.35, .TRS =  50.0},
    [LED_WS2812B_V2]  = { .pixel_sz_bytes = 3, .T0H = 0.40, .T1H = 0.85, .T0L = 0.85, .T1L = 0.40, .TRS =  50.0},
    [LED_WS2812B_V3]  = { .pixel_sz_bytes = 3, .T0H = 0.45, .T1H = 0.85, .T0L = 0.85, .T1L = 0.45, .TRS =  50.0},
    [LED_WS2813_V1]   = { .pixel_sz_bytes = 3, .T0H = 0.35, .T1H = 0.80, .T0L = 0.35, .T1L = 0.35, .TRS = 300.0},
    [LED_WS2813_V2]   = { .pixel_sz_bytes = 3, .T0H = 0.27, .T1H = 0.80, .T0L = 0.80, .T1L = 0.27, .TRS = 300.0},
    [LED_WS2813_V3]   = { .pixel_sz_bytes = 3, .T0H = 0.27, .T1H = 0.63, .T0L = 0.63, .T1L = 0.27, .TRS = 300.0},
    [LED_SK6812_V1]   = { .pixel_sz_bytes = 3, .T0H = 0.30, .T1H = 0.60, .T0L = 0.90, .T1L = 0.60, .TRS =  80.0},
    [LED_PI554FCH]    = { .pixel_sz_bytes = 3, .T0H = 0.30, .T1H = 0.60, .T0L = 0.90, .T1L = 0.60, .TRS =  80.0},
};

/* Definitions for default colors, (0.0 - 1.0) = (0 - 255) */
const RGB_COLOR_PCT RGB_LED_default_colors_S[] = {
    {.r = 1.0, .g = 0.0, .b = 0.0}, /* COLOR_Red */
    {.r = 1.0, .g = 0.5, .b = 0.0}, /* COLOR_Orange */
    {.r = 1.0, .g = 1.0, .b = 0.0}, /* COLOR_Yellow */
    {.r = 0.5, .g = 1.0, .b = 0.0}, /* COLOR_Lime */
    {.r = 0.0, .g = 1.0, .b = 0.0}, /* COLOR_Green */
    {.r = 0.0, .g = 1.0, .b = 0.5}, /* COLOR_Mint */
    {.r = 0.0, .g = 1.0, .b = 1.0}, /* COLOR_Cyan */
    {.r = 0.0, .g = 0.5, .b = 1.0}, /* COLOR_Azure */
    {.r = 0.0, .g = 0.0, .b = 1.0}, /* COLOR_Blue */
    {.r = 0.5, .g = 0.0, .b = 1.0}, /* COLOR_Purple */
    {.r = 1.0, .g = 0.0, .b = 1.0}, /* COLOR_Pink */
    {.r = 1.0, .g = 0.0, .b = 0.5}  /* COLOR_Rose */
};

static RGB_COLOR_PCT pixel_colors_pct_S[ RGB_LED_COUNT ];       /* Saved color for each pixel */
static RGB_COLOR_24BIT pixel_colors_24bit_S[ RGB_LED_COUNT ];   /* Converted color for each pixel */
static UINT8 pixel_tx_buffer_u8[ RGB_LED_COUNT * 3 ];           /* Entire LED pixel buffer to send */

static rmt_channel_handle_t RGB_channel_handle = NULL;
static rmt_encoder_handle_t RGB_encoder_handle = NULL;

static RGB_LED_TYPE_PARAMS RGB_LED_params_S;

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ Function Definitions ][~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/* Local encoder function */
static size_t RGB_RMT_encode(rmt_encoder_t *encoder, rmt_channel_handle_t channel,
    const void *primary_data, size_t data_size, rmt_encode_state_t *ret_state)
{
    RGB_LED_ENCODER* RGB_encoder = __containerof( encoder, RGB_LED_ENCODER, base );

    /* Set up encoder handles for the different stages */
    rmt_encoder_handle_t byte_encoder = RGB_encoder->byte_encoder;
    rmt_encoder_handle_t copy_encoder = RGB_encoder->copy_encoder;

    /* Record encoding states since this process may be broken up */
    rmt_encode_state_t session_state = 0;
    rmt_encode_state_t state = 0;

    /* Record number of encoded symbols */
    size_t encoded_symbols = 0;

    /* Behaviour based on encoder state */
    switch( RGB_encoder->encoder_state )
    {
        case 0: /* Send the RGB data */
            encoded_symbols += byte_encoder->encode(byte_encoder, channel,
                primary_data, data_size, &session_state);

            /* Switch to the reset state when the current encoding completes */
            if( session_state & RMT_ENCODING_COMPLETE )
            {
                RGB_encoder->encoder_state = 1;
            }
            /* Yield if there is no more space */
            if( session_state & RMT_ENCODING_MEM_FULL )
            {
                state |= RMT_ENCODING_MEM_FULL;
                *ret_state = state;
                return encoded_symbols;
            }
        /* Fall through... */
        case 1: /* Send the reset code */
            encoded_symbols += copy_encoder->encode(copy_encoder, channel,
                &RGB_encoder->reset_code, sizeof(RGB_encoder->reset_code), &session_state);

            /* Switch to the reset state when the current encoding completes */
            if( session_state & RMT_ENCODING_COMPLETE )
            {
                RGB_encoder->encoder_state = 0;
                state |= RMT_ENCODING_COMPLETE;
            }
            /* Yield if there is no more space */
            if( session_state & RMT_ENCODING_MEM_FULL )
            {
                state |= RMT_ENCODING_MEM_FULL;
                *ret_state = state;
                return encoded_symbols;
            }
    }

    *ret_state = state;
    return encoded_symbols;
}

/* Local encoder function */
static esp_err_t RGB_RMT_delete( rmt_encoder_t* encoder )
{
    RGB_LED_ENCODER* RGB_encoder = __containerof( encoder, RGB_LED_ENCODER, base );
    rmt_del_encoder( RGB_encoder->byte_encoder );
    rmt_del_encoder( RGB_encoder->copy_encoder );
    free(RGB_encoder);
    return ESP_OK;
}

/* Local encoder function */
static esp_err_t RGB_RMT_reset( rmt_encoder_t* encoder )
{
    RGB_LED_ENCODER* RGB_encoder = __containerof( encoder, RGB_LED_ENCODER, base );
    rmt_encoder_reset( RGB_encoder->byte_encoder );
    rmt_encoder_reset( RGB_encoder->copy_encoder );
    RGB_encoder->encoder_state = 0;
    return ESP_OK;
}

/* Local encoder function */
static esp_err_t RGB_RMT_setup_encoder( RGB_LED_TYPE_PARAMS* params, rmt_encoder_handle_t* ret_encoder )
{
    esp_err_t return_status = ESP_OK;

    RGB_LED_ENCODER* RGB_encoder = NULL;
    RGB_encoder = calloc( 1, sizeof(RGB_LED_ENCODER) );

    /* (1/4) Set up the base encoder */
    RGB_encoder->base.encode = RGB_RMT_encode;
    RGB_encoder->base.del = RGB_RMT_delete;
    RGB_encoder->base.reset = RGB_RMT_reset;

    /* (2/4) Set up the byte encoder */
    rmt_bytes_encoder_config_t byte_config = {
        .bit0 = {
            .level0 = 1,
            .duration0 = params->T0H * RGB_LED_HZ / 1000000,
            .level1 = 0,
            .duration1 = params->T0L * RGB_LED_HZ / 1000000,
        },
        .bit1 = {
            .level0 = 1,
            .duration0 = params->T1H * RGB_LED_HZ / 1000000,
            .level1 = 0,
            .duration1 = params->T1L * RGB_LED_HZ / 1000000,
        },
        .flags.msb_first = 1
    };
    ESP_ERROR_CHECK( rmt_new_bytes_encoder( &byte_config, &RGB_encoder->byte_encoder ) );

    /* (3/4) Set up the copy encoder */
    rmt_copy_encoder_config_t copy_config = {};
    ESP_ERROR_CHECK( rmt_new_copy_encoder( &copy_config, &RGB_encoder->copy_encoder ) );

    /* (4/4) Set up the reset code value */
    UINT32 reset_ticks = params->TRS / 2 * RGB_LED_HZ / 1000000;
    RGB_encoder->reset_code = (rmt_symbol_word_t) {
        .level0 = 0,
        .duration0 = reset_ticks,
        .level1 = 0,
        .duration1 = reset_ticks,
    };

    /* Return the encoder */
    *ret_encoder = &RGB_encoder->base;

    /* Return status */
    return return_status;
}

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * SUMMARY:
 *      RGB_LED_Init() - "Initialize the RGB leds"
 *
 * DESCRIPTION:
 *      Initializes the RGB leds
 *
 * INPUTS:
 *      none
 *
 * OUTPUTS:
 *      none
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
STATUS_E RGB_LED_Init()
{
    /* Set up the RGBLED params */
    RGB_LED_params_S = RGB_LED_all_params_S[ RGB_LED_TYPE ];

    /* 1 - Install RMT TX Channel */
    ESP_LOGI("RGB_LED_Init()", "RMT TX: install");
    rmt_tx_channel_config_t RGB_channel_config;
    RGB_channel_config.gpio_num = RGB_LED_PIN;
    RGB_channel_config.clk_src = RMT_CLK_SRC_DEFAULT;
    RGB_channel_config.resolution_hz = RGB_LED_HZ;
    RGB_channel_config.mem_block_symbols = 128;
    RGB_channel_config.trans_queue_depth = 4;
    ESP_ERROR_CHECK( rmt_new_tx_channel( &RGB_channel_config, &RGB_channel_handle ) );

    /* 2 - Enable RMT TX channel */
    ESP_LOGI("RGB_LED_Init()", "RMT tx: enable");
    ESP_ERROR_CHECK( rmt_enable( RGB_channel_handle ) );

    /* 3 - Install RGB RMT encoder */
    ESP_ERROR_CHECK( RGB_RMT_setup_encoder( &RGB_LED_params_S, &RGB_encoder_handle ) );

    /* Set the initial colors to off (0, 0, 0) */
    memset(pixel_tx_buffer_u8, 0, sizeof(pixel_tx_buffer_u8));
    RGB_LED_TransmitColors();

    return STATUS_OK;
}

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * SUMMARY:
 *      RGB_LED_SetPixelColor() - "Set the color of an RGB LED"
 *
 * DESCRIPTION:
 *      Sets a RGB_COLOR_24BIT corresponding to the LED
 *
 * INPUTS:
 *      (INT32) index of the LED
 *      (RGB_COLOR_PCT) RGB color proportions
 *      (UINT8) visible brightness of the color
 *
 * OUTPUTS:
 *      none
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
STATUS_E RGB_LED_SetPixelColor(INT32 index_i32, RGB_COLOR_PCT color_S, UINT8 brightness_u8 )
{
    /* If the passed index is greater than number of LEDs */
    if( index_i32 >= RGB_LED_COUNT ){
        ESP_LOGE("RGB_LED_SetPixelColor()", "Index out of bounds");
        return;
    }

    RGB_COLOR_24BIT rgb_color;
    UINT8 r_lut;
    UINT8 g_lut;
    UINT8 b_lut;

    /* Reduce brightness to max 100% */
    if( brightness_u8 > 100 )
    {
        brightness_u8 = 100;
    }

    /* Get the LUT indices */
    r_lut = (UINT8)(color_S.r * brightness_u8);
    g_lut = (UINT8)(color_S.g * brightness_u8);
    b_lut = (UINT8)(color_S.b * brightness_u8);

    /* Check for OOB LUT errors */
    if( r_lut > 100 || g_lut > 100 || b_lut > 100 )
    {
        ESP_LOGE("RGB_LED_SetPixelColor()", "LUT index out of bounds");
    }

    /* Get the actual 0-255 RGB values from the LUT */
    rgb_color.red_U8 = gamma_lut[r_lut];
    rgb_color.green_U8 = gamma_lut[g_lut];
    rgb_color.blue_U8 = gamma_lut[b_lut];

    /* Set the color */
    pixel_colors_pct_S[index_i32] = color_S;
    pixel_colors_24bit_S[index_i32] = rgb_color;

    return STATUS_OK;
}

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * SUMMARY:
 *      RGB_LED_ModifyPixelBrightness() - "Set the brightness of an RGB LED"
 *
 * DESCRIPTION:
 *      Sets a RGB_COLOR_24BIT corresponding to the LED, based on the newly
 *      provided brightness value.
 *
 * INPUTS:
 *      (INT32) index of the LED
 *      (UINT8) visible brightness of the color
 *
 * OUTPUTS:
 *      none
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
STATUS_E RGB_LED_ModifyPixelBrightness(INT32 index_i32, UINT8 brightness_u8)
{
    return RGB_LED_SetPixelColor(index_i32, pixel_colors_pct_S[index_i32], brightness_u8);
}

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * SUMMARY:
 *      RGB_LED_TransmitColors() - "Convert colors to pixel buffer and transmit"
 *
 * DESCRIPTION:
 *      Goes through the array of RGB_COLOR_24BIT variables, sets the pixel buffer,
 *      and then transmits them using the rmt peripheral.
 *
 * INPUTS:
 *      none
 *
 * OUTPUTS:
 *      none
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
STATUS_E RGB_LED_TransmitColors()
{
    /* Create a pixel buffer and populate it with 8-bit values */
    for( INT32 pixel = 0; pixel < RGB_LED_COUNT; pixel += 1 )
    {
        pixel_tx_buffer_u8[ ( pixel * 3 ) + 0 ] = pixel_colors_24bit_S[ pixel ].green_U8;
        pixel_tx_buffer_u8[ ( pixel * 3 ) + 1 ] = pixel_colors_24bit_S[ pixel ].red_U8;
        pixel_tx_buffer_u8[ ( pixel * 3 ) + 2 ] = pixel_colors_24bit_S[ pixel ].blue_U8;
    }

    /* Set up the transmission configuration */
    rmt_transmit_config_t tx_config = {
        .loop_count = 0,
    };

    /* Send out the values */
    ESP_ERROR_CHECK( rmt_transmit(
        RGB_channel_handle,
        RGB_encoder_handle,
        pixel_tx_buffer_u8,
        sizeof(pixel_tx_buffer_u8),
        &tx_config
    ) );

    return STATUS_OK;
}

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * SUMMARY:
 *      RGB_LED_Wipe() - "Wipe all colors and transmit"
 *
 * DESCRIPTION:
 *      Sets pixel buffer to all zeros and transmits.
 *
 * INPUTS:
 *      none
 *
 * OUTPUTS:
 *      none
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
STATUS_E RGB_LED_Wipe()
{
    /* Set the colors to off (0, 0, 0) */
    memset(pixel_colors_24bit_S, 0, sizeof(pixel_colors_24bit_S ) );
    memset(pixel_tx_buffer_u8, 0, sizeof(pixel_tx_buffer_u8 ) );
    return RGB_LED_TransmitColors();
}