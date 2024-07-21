/*==============================================================================
 *==============================================================================
 * NAME:
 *      networking.c
 *
 * PURPOSE:
 *      Higher level interfaces related to networking in the application. These
 *      include:
 *          -> NTP time retrieval
 *          -> REST api endpoints
 *          -> Static webpage hosting
 *
 * DEPENDENCIES:
 *      networking.h
 *      lib_connect.h
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

#include "networking.h"
#include "lib_connect.h"
#include "lib_messaging.h"

#include "esp_sntp.h"
#include "esp_http_server.h"
#include "cJSON.h"

/*=============================================================================*/
/*][ LOCAL : Constants and Types ][============================================*/
/*=============================================================================*/

const static char* LOG_TAG = "networking";

/*=============================================================================*/
/*][ LOCAL : Variables ][======================================================*/
/*=============================================================================*/

static SemaphoreHandle_t ntp_semaphore_s;
// TODO: last clock status

static httpd_handle_t httpd_server_s;

// TODO: stupid definition for webpage - add to program flash

/*=============================================================================*/
/*][ LOCAL : Function Prototypes ][============================================*/
/*=============================================================================*/

static void ntp_retrieve_handler( struct timeval* tv );

// TODO: API handlers
// static esp_err_t api_something_handler( hhtpd_req_t* req );

/*=============================================================================*/
/*][ LOCAL : Function Definitions ][===========================================*/
/*=============================================================================*/

/**===< EXTERNAL / global >====================================================
 * NAME:
 *      CONNECT_start_interfaces() - start the various networking components
 *
 * SUMMARY:
 *      This is called inside the event handler of the high level "lib_connect"
 *      component when the wifi is successfully connected.
 *
 * INPUT REQUIREMENTS:
 *      ---
 *
 * OUTPUT GUARANTEES:
 *      ---
 **===< EXTERNAL / global >====================================================*/
void CONNECT_start_interfaces()
{
    sntp_restart();
}

/**===< EXTERNAL / global >====================================================
 * NAME:
 *      CONNECT_stop_interfaces() - stop the various networking components
 *
 * SUMMARY:
 *      This is called inside the event handler of the high level "lib_connect"
 *      component when the wifi is disconnected.
 *
 * INPUT REQUIREMENTS:
 *      ---
 *
 * OUTPUT GUARANTEES:
 *      ---
 **===< EXTERNAL / global >====================================================*/
void CONNECT_stop_interfaces()
{
    esp_sntp_stop();
}

/**===< local >================================================================
 * NAME:
 *
 * SUMMARY:
 *
 * INPUT REQUIREMENTS:
 *
 * OUTPUT GUARANTEES:
 **===< local >================================================================*/
void ntp_retrieve_handler( struct timeval* tv )
{
    ESP_LOGD(LOG_TAG, "NTP handler: %lld sec", tv->tv_sec);

    CHAR time_buffer_str[20];
    esp_err_t time_retrieved = FALSE;

    time_retrieved = NETWORKING_NTP_get_time_string( time_buffer_str, sizeof(time_buffer_str) );

    if( time_retrieved == ESP_OK )
    {
        ESP_LOGI(LOG_TAG, "NTP handler: '%s'", time_buffer_str);
    }
    else
    {
        ESP_LOGE(LOG_TAG, "Failed to get converted time.");
    }

    xSemaphoreGive( ntp_semaphore_s );
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
esp_err_t NETWORKING_NTP_init()
{
    ntp_semaphore_s = xSemaphoreCreateBinary();

    CHAR tz_string_str[50];
    memset( tz_string_str, 0, sizeof(tz_string_str) );

    snprintf( tz_string_str, sizeof(tz_string_str), "%s,%s,%s", BASE_TZ_STRING, CONFIG_DST_START, CONFIG_DST_END );

    setenv( "TZ", tz_string_str, 1 );
    tzset();

    esp_sntp_init();
    sntp_set_sync_mode( SNTP_SYNC_MODE_IMMED );
    esp_sntp_setservername( 0, CONFIG_ESP_NTP_SERVER_IP );

    sntp_set_time_sync_notification_cb( ntp_retrieve_handler );
    sntp_set_sync_interval( CONFIG_ESP_NTP_REFRESH_INTERVAL );

    // Wait for 10 seconds to get the NTP time, otherwise fail.
    if( xSemaphoreTake( ntp_semaphore_s, pdMS_TO_TICKS( 10000 ) ) == pdTRUE )
    {
        return ESP_OK;
    }

    esp_sntp_stop();
    return ESP_FAIL;
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
esp_err_t NETWORKING_NTP_get_time_string( CHAR* p_buffer_c, INT32 len_i32 )
{
    struct tm* time_info_s;
    NETWORKING_NTP_get_time_info( &time_info_s );

    CHAR time_buffer_str[20];

    size_t printed = strftime(time_buffer_str, sizeof(time_buffer_str), "%I:%M %p, %b %d", time_info_s );
    if( printed < len_i32 )
    {
        snprintf(p_buffer_c, printed + 1, "%s", time_buffer_str );
        return ESP_OK;
    }

    return ESP_FAIL;
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
extern esp_err_t NETWORKING_NTP_get_time_stamp( CHAR* p_buffer_c, INT32 len_i32 )
{
    struct tm* time_info_s;
    NETWORKING_NTP_get_time_info( &time_info_s );

    CHAR time_buffer_str[20];

    size_t printed = strftime(time_buffer_str, sizeof(time_buffer_str), "%F,%T", time_info_s );
    if( printed < len_i32 )
    {
        snprintf(p_buffer_c, printed + 1, "%s", time_buffer_str );
        return ESP_OK;
    }

    return ESP_FAIL;
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
extern esp_err_t NETWORKING_NTP_get_time_info( struct tm** p_time_info_s )
{
    time_t now = 0;
    time( &now );

    *p_time_info_s = localtime( &now );

    return ESP_OK;
}