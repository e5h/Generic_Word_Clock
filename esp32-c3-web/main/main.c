/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * NAME:
 *      main.c
 *
 * PURPOSE:
 *      This is the main file for running the generic word clock.
 *
 * DEPENDENCIES:
 *      lib_includes.h
 *
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
 * (C) Andrew Bright 2023, github.com/e5h
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ Include Files ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

#include <stdio.h>
#include "lib_includes.h"

#include "rgb_rmt.h"
#include "cfg_clock.h"

#include "task_display.h"
// task device
// task network

#include "esp_timer.h"

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ LOCAL : Constants and Types ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/* Different types of inter-thread messages */
typedef enum{
    MSG_DEFAULT,            /* Default (no type) */
} MSG_TYPE;

typedef struct{
    MSG_TYPE type_E;        /* Type of message */
    union                   /* Union for message data */
    {
        BOOL default_B;
    };
} MSG_DATA;

typedef enum{
    FLAG_1_MS           = 0x01, /* Flag set on 1ms timer callback */
    FLAG_10_MS          = 0x02, /* Flag set on 10ms timer callback */
    FLAG_100_MS         = 0x04, /* Flag set on 100ms timer callback */
    FLAG_1_SEC          = 0x08, /* Flag set on 1s timer callback */
} E_THREAD_FLAG;

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ LOCAL : Variables ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/* ESP event groups (flags) */
EventGroupHandle_t rgbleds_flags;

/* ESP timer handles */
esp_timer_handle_t timer_handle_1ms;
esp_timer_handle_t timer_handle_10ms;
esp_timer_handle_t timer_handle_100ms;
esp_timer_handle_t timer_handle_1sec;

/* FreeRTOS task handles */
TaskHandle_t h_task_rgbleds;

/* Order and request queues */
QueueHandle_t q_order_rgbleds_task;

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ LOCAL : Function Prototypes ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ Function Definitions ][~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * Timer callback - 1 ms
 *
 * DESCRIPTION:
 *      This function is called every 1 milliseconds. The purpose is to wake
 *      certain tasks periodically by setting their task notification bits.
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
void timer_1ms_callback(void *param)
{
    TIMER_TickMsUpdate(); /* Updates the 1ms tick inside lib_timer */
    //xEventGroupSetBits( rgbleds_flags, FLAG_1_MS );
}

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * Timer callback - 10 ms
 *
 * DESCRIPTION:
 *      This function is called every 10 milliseconds. The purpose is to wake
 *      certain tasks periodically by setting their task notification bits.
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
void timer_10ms_callback(void *param)
{
    // Set event group bits
}

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * Timer callback - 100 ms
 *
 * DESCRIPTION:
 *      This function is called every 100 milliseconds. The purpose is to wake
 *      certain tasks periodically by setting their task notification bits.
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
void timer_100ms_callback(void *param)
{
    xEventGroupSetBits( rgbleds_flags, FLAG_100_MS );
}

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * Timer callback - 1 sec
 *
 * DESCRIPTION:
 *      This function is called every 1 second. The purpose is to wake
 *      certain tasks periodically by setting their task notification bits.
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
void timer_1sec_callback(void *param)
{
    xEventGroupSetBits( rgbleds_flags, FLAG_1_SEC );
}

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * TASK: task_rgbleds
 * PRIO: 2 - default priority (runs constantly)
 *
 * DESCRIPTION:
 *      This task handles writing to the RGB leds and updating them.
 * 
 * TASK NOTIFICATION BITS:
 *      bit 0:      100ms flag
 *      bit 1:      n/a
 *      bit 2:      n/a
 *      bit 3:      n/a
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
void task_rgbleds( void* params )
{
    char* LOG_TAG = "task_rgbleds";
    MSG_DATA rec_msg; /* Variable for receiving a message */

    EventBits_t flags_to_wait;
    flags_to_wait = FLAG_100_MS | FLAG_1_SEC;

    UINT8 active_leds[10] = {0}; // Index of which LEDs are active
    
    UINT8 colorIndex_UC = COLOR_Cyan;
    RGB_COLOR_PCT led_color = RGB_LED_default_colors_S[ colorIndex_UC ];

    while(1)
    {
        /* Wait for any of the flags to be set (wait all = false, auto clear = false) */
        xEventGroupWaitBits( rgbleds_flags, flags_to_wait, false, false, portMAX_DELAY );

        /* FLAG_100_MS - update */
        if( xEventGroupGetBits( rgbleds_flags ) & FLAG_100_MS )
        {
            // /* Increment the first led */
            // active_leds[ 0 ]++;
            // if( active_leds[ 0 ] > ( WC_RGB_LED_COUNT - 1 ) )
            // {
            //     active_leds[ 0 ] -= 100;
            //     if( ++colorIndex_UC >= NUM_DEFAULT_COLORS )
            //     {
            //         colorIndex_UC = 0;
            //     }
            //     led_color = RGB_LED_default_colors_S[ colorIndex_UC ];
            // }

            // /* Increment the rest of the leds */
            // for( INT8 i = 1; i < 10; i++ )
            // {
            //     active_leds[ i ] = active_leds[ i - 1 ] + 1;
            //     if( active_leds[ i ] > ( WC_RGB_LED_COUNT - 1 ) )
            //     {
            //         active_leds[ i ] -= 100;
            //     }
            // }

            // /* Set the LED colors */
            // for( INT8 i = 9; i >= 0; i-- )
            // {
            //     RGB_LED_SetPixelColor( active_leds[ i ], led_color, 10 * (i + 1) );
            // }

            // /* Write the LEDs to the screen */
            // RGB_LED_TransmitColors();

            // /* Log the leading LED */
            // ESP_LOGI( LOG_TAG, "LED index: %d", active_leds[9] );

            /* Clear FLAG_100_MS */
            xEventGroupClearBits( rgbleds_flags, FLAG_100_MS );
        }

        /* FLAG_1_SEC - update */
        if( xEventGroupGetBits( rgbleds_flags ) & FLAG_1_SEC )
        {
            led_color = RGB_LED_default_colors_S[ colorIndex_UC ];

            CLOCK_TestWords( led_color );

            if( ++colorIndex_UC >= NUM_DEFAULT_COLORS )
            {
                colorIndex_UC = 0;
                CLOCK_UpdateTime(); // test a phrase
            }

            /* Clear FLAG_1_SEC */
            xEventGroupClearBits( rgbleds_flags, FLAG_1_SEC );
        }
    }
}

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * app_main
 *
 * DESCRIPTION:
 *      This acts as the initial setup of the device when powered on. It sets up
 *      all hardware peripherals, and begins each RTOS task.
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
void app_main(void)
{
    /* Hardware delays (capacitors, etc) */

    /* Set up event groups */
    rgbleds_flags = xEventGroupCreate();

    /* Set up queues */
    q_order_rgbleds_task = xQueueCreate( 5, sizeof(MSG_DATA) );

    /* Set up other peripherals */
    RGB_LED_Init();

    /* Set up timers */
    /* 1 ms timer */
    const esp_timer_create_args_t timer_args_1ms = {
        .callback = &timer_1ms_callback,
        .name = "1 ms timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_args_1ms, &timer_handle_1ms));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle_1ms, 1000));

    /* 10ms timer */
    const esp_timer_create_args_t timer_args_10ms = {
        .callback = &timer_10ms_callback,
        .name = "10 ms timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_args_10ms, &timer_handle_10ms));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle_10ms, 10000));

    /* 100ms timer */
    const esp_timer_create_args_t timer_args_100ms = {
        .callback = &timer_100ms_callback,
        .name = "100 ms timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_args_100ms, &timer_handle_100ms));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle_100ms, 100000));

    /* 1sec timer */
    const esp_timer_create_args_t timer_args_1sec = {
        .callback = &timer_1sec_callback,
        .name = "1 sec timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_args_1sec, &timer_handle_1sec));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle_1sec, 1000000));

    /* Set up interrupts */

    /* Set up tasks */
    xTaskCreate( &task_rgbleds, "RGB Led Task", 4096, NULL, 2, &h_task_rgbleds );
}
