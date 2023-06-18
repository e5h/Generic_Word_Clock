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
/*][ Feature Switches ][~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ Include Files ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

#include <stdio.h>
#include "lib_includes.h"

#include "ESP_RGB.h"

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

#define FLAG_1_MS                   (BIT0)  /* Flag set on 1ms timer callback */
#define FLAG_10_MS                  (BIT1)  /* Flag set on 10ms timer callback */
#define FLAG_100_MS                 (BIT2)  /* Flag set on 100ms timer callback */
#define FLAG_1_SEC                  (BIT3)  /* Flag set on 1s timer callback */

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
    TIMER_tickMsUpdate(); /* Updates the 1ms tick inside lib_timer */
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
    // Set event group bits
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
    flags_to_wait = FLAG_100_MS;

    UINT8 active_leds[10] = {0}; // Index of which LEDs are active
    COLOR_pct led_color = Cyan;

    while(1)
    {
        /* Wait for any of the flags to be set (wait all = false, auto clear = false) */
        xEventGroupWaitBits( rgbleds_flags, flags_to_wait, false, false, portMAX_DELAY );

        /* FLAG_100_MS - update */
        if( xEventGroupGetBits( rgbleds_flags ) & FLAG_100_MS )
        {
            /* Increment the first led */
            active_leds[ 0 ]++;
            if( active_leds[ 0 ] > ( RGB_LED_COUNT - 1 ) )
            {
                active_leds[ 0 ] -= 100;
            }

            /* Increment the rest of the leds */
            for( INT8 i = 1; i < 10; i++ )
            {
                active_leds[ i ] = active_leds[ i - 1 ] + 1;
                if( active_leds[ i ] > ( RGB_LED_COUNT - 1 ) )
                {
                    active_leds[ i ] -= 100;
                }
            }

            /* Set the LED colors */
            for( INT8 i = 9; i >= 0; i-- )
            {
                RGBLED_SetColor( active_leds[ i ], led_color, 10 * (i + 1) );
            }

            /* Write the LEDs to the screen */
            RGBLED_TransmitColors();

            /* Clear FLAG_100_MS */
            xEventGroupClearBits( rgbleds_flags, FLAG_100_MS );
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
    RGBLED_Init();

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
    xTaskCreate( &task_rgbleds, "RGB Led Task", 2048, NULL, 2, &h_task_rgbleds );
}
