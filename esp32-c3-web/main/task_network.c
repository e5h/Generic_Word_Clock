/*==============================================================================
 *==============================================================================
 * NAME:
 *      task_network.c
 *
 * PURPOSE:
 *      One of the major tasks of the application.
 *
 *      The network task is responsible for managing the wifi connection, and
 *      for translating between application level "messages" and data received
 *      over the network interfaces.
 *
 * DEPENDENCIES:
 *      task_networking.h
 *      lib_messaging.h
 *      lib_connect.h
 *      networking.h
 *
 *==============================================================================
 * (C) Andrew Bright 2024, github.com/e5h
 *==============================================================================
 *=============================================================================*/

/*=============================================================================*/
/*][ Feature Switches ][=======================================================*/
/*=============================================================================*/

/*=============================================================================*/
/*][ Include Files ][==========================================================*/
/*=============================================================================*/

#include "task_network.h"
#include "lib_messaging.h"
#include "lib_connect.h"
#include "networking.h"

/*=============================================================================*/
/*][ LOCAL : Constants and Types ][============================================*/
/*=============================================================================*/

const static char* LOG_TAG = "task_network";

/*=============================================================================*/
/*][ LOCAL : Variables ][======================================================*/
/*=============================================================================*/

/* ESP event groups (flags) */
static EventGroupHandle_t h_network_flags;

/* FreeRTOS task handles */
static TaskHandle_t h_network_task;

/* Order and request queues */
static QueueHandle_t h_network_queue;

/*=============================================================================*/
/*][ LOCAL : Function Prototypes ][============================================*/
/*=============================================================================*/

void task_network( void* params );

/*=============================================================================*/
/*][ LOCAL : Function Definitions ][===========================================*/
/*=============================================================================*/

/**===< local >================================================================
 * NAME:
 *
 * SUMMARY:
 *
 * INPUT REQUIREMENTS:
 *
 * OUTPUT GUARANTEES:
 **===< local >================================================================*/

/**===< local >================================================================
 * NAME:
 *
 * SUMMARY:
 *
 * INPUT REQUIREMENTS:
 *
 * OUTPUT GUARANTEES:
 **===< local >================================================================*/
void task_network( void* params )
{
    BOOL wifi_connected_b = FALSE;
    BOOL ntp_connected_b = FALSE;
    CHAR ntp_time_str[50];
    memset(ntp_time_str, 0, sizeof(ntp_time_str));

    CONNECT_init();

    while(1)
    {
        if(wifi_connected_b == FALSE)
        {
            ESP_LOGW(LOG_TAG, "Connecting to wifi..." );
            if(ESP_OK == CONNECT_wifi_sta(CONFIG_ESP_WIFI_SSID, CONFIG_ESP_WIFI_PASSWORD, 10000) )
            {
                ESP_LOGW(LOG_TAG, "Connected to wifi!");
                wifi_connected_b = TRUE;
            }
        }
        else if(ntp_connected_b == FALSE)
        {
            ESP_LOGW(LOG_TAG, "Initializing NTP, server %s...", CONFIG_ESP_NTP_SERVER_IP );
            if(ESP_OK == NETWORKING_NTP_init())
            {
                ESP_LOGW(LOG_TAG, "Got NTP time!" );
                ntp_connected_b = TRUE;
            }
        }
        else
        {
            NETWORKING_NTP_get_time_string(ntp_time_str, sizeof(ntp_time_str));
            ESP_LOGI(LOG_TAG, "NTP Time: %s", ntp_time_str);
            vTaskDelay(pdMS_TO_TICKS(10000));
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

/**===< global >===============================================================
 * NAME:
 *
 * SUMMARY:
 *
 * INPUT REQUIREMENTS:
 *
 * OUTPUT GUARANTEES:
 **===< global >===============================================================*/

/**===< global >===============================================================
 * NAME:
 *      TASK_NETWORK_create() - create the network task
 *
 * SUMMARY:
 *      This is called from the initialization function of the application, and
 *      separates the task creation into a separate module.
 *
 * INPUT REQUIREMENTS:
 *      ---
 *
 * OUTPUT GUARANTEES:
 *      ---
 **===< global >===============================================================*/
void TASK_NETWORK_create(void)
{
    xTaskCreate(&task_network, "Network Task", 4096, NULL, 2, &h_network_task);
}









/* End */