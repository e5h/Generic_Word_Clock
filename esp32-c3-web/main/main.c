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
#include "lib_messaging.h"

#include "rgb_rmt.h"
#include "cfg_clock.h"
#include "rtc.h"

#include "task_display.h"
// task device
// task network

#include "esp_timer.h"
#include "nvs_flash.h"

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ LOCAL : Constants and Types ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

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
EventGroupHandle_t heartbeat_flags;

/* ESP timer handles */
esp_timer_handle_t timer_handle_1ms;
esp_timer_handle_t timer_handle_10ms;
esp_timer_handle_t timer_handle_100ms;
esp_timer_handle_t timer_handle_1sec;

/* FreeRTOS task handles */
TaskHandle_t h_task_heartbeat;

/* Order and request queues */
QueueHandle_t q_heartbeat_task;

// Clock configuration
static CLOCK_CONFIG clock_config_s;

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ LOCAL : Function Prototypes ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ Function Definitions ][~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/**===< local >================================================================
 * NAME:
 *      nvs_save_clock_config() - saves the current clock configuration
 *
 * SUMMARY:
 *      Takes all of the clock configuration values and saves them to non
 *      volatile storage.
 *
 * INPUT REQUIREMENTS:
 *      ---
 *
 * OUTPUT GUARANTEES:
 *      Returns an enum status (fail <= 0, success > 0)
 **===< local >================================================================*/
static STATUS_E nvs_save_clock_config( CLOCK_CONFIG* new_config_s )
{
    nvs_handle_t nvs_handle;
    size_t config_size = sizeof( CLOCK_CONFIG );

    esp_err_t err;
    STATUS_E status_e = STATUS_OK;

    // Check for differences to reduce memory wear
    if( memcmp( new_config_s, &clock_config_s, config_size ) == 0 )
    {
        return STATUS_NO_CHANGE; // No change
    }

    // TODO: Sanity check the values

    err = nvs_open( "clock", NVS_READWRITE, &nvs_handle );
    if( err != ESP_OK )
    {
        ESP_LOGE("nvs_save_clock_config", "Could not open NVS! (%s)", esp_err_to_name(err));
        return STATUS_ERR;
    }

    err = nvs_set_blob( nvs_handle, "clockconfig", new_config_s, config_size );
    if( err != ESP_OK )
    {
        ESP_LOGE("nvs_save_clock_config", "Could not write to NVS! (%s)", esp_err_to_name(err));
        return STATUS_ERR;
    }

    err = nvs_commit( nvs_handle );
    if( err != ESP_OK )
    {
        ESP_LOGE("nvs_save_clock_config", "Could not save NVS! (%s)", esp_err_to_name(err));
        return STATUS_ERR;
    }

    nvs_close( nvs_handle );
    ESP_LOGW("nvs_save_clock_config", "Saved clock config to NVS.");
    ESP_LOGI("nvs_save_clock_config", "Saved time: ");
    // TODO: Convert the struct tm to a readable timestamp.

    clock_config_s = *new_config_s;

    return status_e;
}

/**===< local >================================================================
 * NAME:
 *      nvs_load_clock_config() - loads the current clock configuration
 *
 * SUMMARY:
 *      Loads all of the clock configuration values from the on-board non
 *      volatile storage.
 *
 * INPUT REQUIREMENTS:
 *      ---
 *
 * OUTPUT GUARANTEES:
 *      Returns an enum status (fail <= 0, success > 0)
 **===< local >================================================================*/
static STATUS_E nvs_load_clock_config( CLOCK_CONFIG* new_config_s )
{
    nvs_handle_t nvs_handle;
    size_t config_size = sizeof( CLOCK_CONFIG );

    esp_err_t err;
    STATUS_E status_e = STATUS_OK;

    err = nvs_open( "clock", NVS_READWRITE, &nvs_handle );
    if( err != ESP_OK )
    {
        ESP_LOGE("nvs_load_clock_config", "Could not open NVS! (%s)", esp_err_to_name(err));
        return STATUS_ERR;
    }

    err = nvs_get_blob( nvs_handle, "clockconfig", new_config_s, &config_size );
    if( err != ESP_OK )
    {
        ESP_LOGE("nvs_load_clock_config", "Could not read from NVS! (%s)", esp_err_to_name(err));
        return STATUS_ERR;
    }

    err = nvs_commit( nvs_handle );
    if( err != ESP_OK )
    {
        ESP_LOGE("nvs_load_clock_config", "Could not save NVS! (%s)", esp_err_to_name(err));
        return STATUS_ERR;
    }

    nvs_close( nvs_handle );
    ESP_LOGW("nvs_load_clock_config", "Loaded clock config from NVS.");
    ESP_LOGI("nvs_load_clock_config", "Loaded time: ");
    // TODO: Convert the struct tm to a readable timestamp.

    clock_config_s = *new_config_s;

    return status_e;
}

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * Timer callback - 1 ms
 *
 * DESCRIPTION:
 *      This function is called every 1 millisecond. The purpose is to wake
 *      certain tasks periodically by setting their task notification bits.
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
void timer_1ms_callback(void *param)
{
    TIMER_TickMsUpdate(); /* Updates the 1ms tick inside lib_timer */
    //xEventGroupSetBits( heartbeat_flags, FLAG_1_MS );
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
    xEventGroupSetBits(heartbeat_flags, FLAG_100_MS );
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
    xEventGroupSetBits(heartbeat_flags, FLAG_1_SEC );
}

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * TASK: task_heartbeat
 * PRIO: 2 - default priority (runs constantly)
 *
 * DESCRIPTION:
 *      This task handles periodic behaviour, primarily for testing.
 *
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
void task_heartbeat(void* params )
{
    char* LOG_TAG = "task_heartbeat";

    EventBits_t new_msg_flag_s = 0x80u;
    MESSAGE_CONTENT_T rec_msg;
    MESSAGE_RECEIVER_T inbox = {
            &heartbeat_flags,
            new_msg_flag_s,
            q_heartbeat_task
    };

    EventBits_t flags_to_wait;
    flags_to_wait = FLAG_100_MS | FLAG_1_SEC | new_msg_flag_s;

    UINT8 active_leds[10] = {0}; // Index of which LEDs are active
    
    UINT8 colorIndex_UC = COLOR_Cyan;
    RGB_COLOR_PCT led_color = RGB_LED_default_colors_S[ colorIndex_UC ];

    MESSAGING_subscribe_to_topic(MSG_BUTTONS, &inbox);

    // Temporarily here for testing
    BUTTON_init();

    while(1)
    {
        /* Wait for any of the flags to be set (wait all = false, auto clear = false) */
        xEventGroupWaitBits(heartbeat_flags, flags_to_wait, false, false, portMAX_DELAY );

        /* New message */
        if(xEventGroupGetBits(heartbeat_flags) & new_msg_flag_s)
        {
            xQueueReceive(q_heartbeat_task, &rec_msg, 0);

            if(rec_msg.topic_e == MSG_BUTTONS)
            {
                CHAR p_button_str_c[40];
                memset(p_button_str_c, 0, sizeof(p_button_str_c));
                BUTTON_event_to_string(&rec_msg.btn_event_s, p_button_str_c, sizeof(p_button_str_c)-1);

                ESP_LOGI(LOG_TAG, "Button message: %s", p_button_str_c);

                // If the "light" button was just pressed, toggle the "status" LED
                if(BUTTON_check_event(&rec_msg.btn_event_s, BTN_LIGHT, BTN_PRESS_RELEASED))
                {
                    ESP_LOGI(LOG_TAG, "Toggling the 'STATUS' LED.");
                    gpio_set_level(GPIO_NUM_10, gpio_get_level(GPIO_NUM_10) ? 0 : 1);
                }

                // If the "light" button was just held, toggle the "wifi" LED
                if(BUTTON_check_event(&rec_msg.btn_event_s, BTN_LIGHT, BTN_HOLD_RELEASED))
                {
                    ESP_LOGI(LOG_TAG, "Toggling the 'INTERNET' LED.");
                    gpio_set_level(GPIO_NUM_8, gpio_get_level(GPIO_NUM_8) ? 0 : 1);
                }
            }

            /* Clear the message flag if there are none left */
            if(uxQueueMessagesWaiting(q_heartbeat_task) == 0)
            {
                xEventGroupClearBits(heartbeat_flags, new_msg_flag_s);
            }
        }

        /* FLAG_100_MS - update */
        if(xEventGroupGetBits(heartbeat_flags ) & FLAG_100_MS )
        {
            // The button handler can be done via polling, or after detecting interrupts.
            // This can be done anywhere, currently here for testing.
            BUTTON_poll();
            if(BUTTON_update_state_machine() == STATUS_NEW_EVENT)
            {
                if(BUTTON_send_events_to_queue() >= STATUS_OK)
                {
                    ESP_LOGI(LOG_TAG, "Sent new button events to queue.");
                }
                else
                {
                    ESP_LOGE(LOG_TAG, "Failed to send new button events to queue!");
                }
            }

            /* Clear FLAG_100_MS */
            xEventGroupClearBits(heartbeat_flags, FLAG_100_MS );
        }

        /* FLAG_1_SEC - update */
        if(xEventGroupGetBits(heartbeat_flags ) & FLAG_1_SEC )
        {
            static UINT32 sec_count = 0;

            led_color = RGB_LED_default_colors_S[ colorIndex_UC ];

            CLOCK_TestWords( led_color );

            if( ++colorIndex_UC >= NUM_DEFAULT_COLORS )
            {
                colorIndex_UC = 0;
                CLOCK_UpdateTime(); // test a phrase
            }

            sec_count++;
            if(sec_count % 10 == 0)
            {
                struct tm rtc_time_s;
                if(RTC_get_time(&rtc_time_s) >= STATUS_OK)
                {
                    ESP_LOGI(LOG_TAG, "Retrieved RTC time: %s", asctime(&rtc_time_s));
                }
                else
                {
                    ESP_LOGE(LOG_TAG, "Failed to get time from RTC.");
                }
            }

            /* Clear FLAG_1_SEC */
            xEventGroupClearBits(heartbeat_flags, FLAG_1_SEC );
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
    heartbeat_flags = xEventGroupCreate();

    /* Set up queues */
    q_heartbeat_task = xQueueCreate( 10, sizeof(MESSAGE_CONTENT_T) );

    /* Set up other peripherals */
    RGB_LED_Init();
    RTC_init(PCF85263A_ADDR_7BIT);

    /* Initialize I2C driver */


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
    xTaskCreate(&task_heartbeat, "RGB Led Task", 4096, NULL, 2, &h_task_heartbeat );

    /* GPIO settings */
    gpio_set_direction(GPIO_NUM_8, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(GPIO_NUM_10, GPIO_MODE_INPUT_OUTPUT);
}
