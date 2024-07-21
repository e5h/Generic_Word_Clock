/*==============================================================================
 *==============================================================================
 * NAME:
 *      lib_connect.c
 *
 * PURPOSE:
 *      Component for simple wifi connections.
 *
 * DEPENDENCIES:
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

#include "lib_connect.h"
#include <stdio.h>
#include <string.h>
#include <esp_mac.h>
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

/*=============================================================================*/
/*][ LOCAL : Constants and Types ][============================================*/
/*=============================================================================*/

/*=============================================================================*/
/*][ LOCAL : Variables ][======================================================*/
/*=============================================================================*/

const static char* LOG_TAG = "lib_connect";
static esp_netif_t* esp_netif;

static EventGroupHandle_t wifi_events;
static const EventBits_t FLAG_CONNECTED = BIT0;
static const EventBits_t FLAG_DISCONNECTED = BIT1;
static const EventBits_t FLAG_ASSIGNED_IP = BIT2;

static bool attempt_reconnect_b = true;
static int reconnect_attempts_i32 = 0;

/*=============================================================================*/
/*][ LOCAL : Function Definitions ][===========================================*/
/*=============================================================================*/

/**===< local >================================================================
 * NAME:
 *      return_error_string() - gets a string representing the wifi error
 *
 * SUMMARY:
 *      This uses a simple switch case with every defined enum for wifi
 *      error reasons (from esp_wifi_types.h) and returns a string corresponding
 *      to the error type. Used primarily for logging error reasons.
 *
 * INPUT REQUIREMENTS:
 *      Must be a valid type of wifi error.
 *
 * OUTPUT GUARANTEES:
 *      Returns a pointer to a null-terminated string.
 **===< local >================================================================*/
static char* return_error_string(wifi_err_reason_t reason_e)
{
    switch (reason_e)
    {
        case WIFI_REASON_UNSPECIFIED:
            return "WIFI_REASON_UNSPECIFIED";
        case WIFI_REASON_AUTH_EXPIRE:
            return "WIFI_REASON_AUTH_EXPIRE";
        case WIFI_REASON_AUTH_LEAVE:
            return "WIFI_REASON_AUTH_LEAVE";
        case WIFI_REASON_ASSOC_EXPIRE:
            return "WIFI_REASON_ASSOC_EXPIRE";
        case WIFI_REASON_ASSOC_TOOMANY:
            return "WIFI_REASON_ASSOC_TOOMANY";
        case WIFI_REASON_NOT_AUTHED:
            return "WIFI_REASON_NOT_AUTHED";
        case WIFI_REASON_NOT_ASSOCED:
            return "WIFI_REASON_NOT_ASSOCED";
        case WIFI_REASON_ASSOC_LEAVE:
            return "WIFI_REASON_ASSOC_LEAVE";
        case WIFI_REASON_ASSOC_NOT_AUTHED:
            return "WIFI_REASON_ASSOC_NOT_AUTHED";
        case WIFI_REASON_DISASSOC_PWRCAP_BAD:
            return "WIFI_REASON_DISASSOC_PWRCAP_BAD";
        case WIFI_REASON_DISASSOC_SUPCHAN_BAD:
            return "WIFI_REASON_DISASSOC_SUPCHAN_BAD";
        case WIFI_REASON_BSS_TRANSITION_DISASSOC:
            return "WIFI_REASON_BSS_TRANSITION_DISASSOC";
        case WIFI_REASON_IE_INVALID:
            return "WIFI_REASON_IE_INVALID";
        case WIFI_REASON_MIC_FAILURE:
            return "WIFI_REASON_MIC_FAILURE";
        case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
            return "WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT";
        case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT:
            return "WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT";
        case WIFI_REASON_IE_IN_4WAY_DIFFERS:
            return "WIFI_REASON_IE_IN_4WAY_DIFFERS";
        case WIFI_REASON_GROUP_CIPHER_INVALID:
            return "WIFI_REASON_GROUP_CIPHER_INVALID";
        case WIFI_REASON_PAIRWISE_CIPHER_INVALID:
            return "WIFI_REASON_PAIRWISE_CIPHER_INVALID";
        case WIFI_REASON_AKMP_INVALID:
            return "WIFI_REASON_AKMP_INVALID";
        case WIFI_REASON_UNSUPP_RSN_IE_VERSION:
            return "WIFI_REASON_UNSUPP_RSN_IE_VERSION";
        case WIFI_REASON_INVALID_RSN_IE_CAP:
            return "WIFI_REASON_INVALID_RSN_IE_CAP";
        case WIFI_REASON_802_1X_AUTH_FAILED:
            return "WIFI_REASON_802_1X_AUTH_FAILED";
        case WIFI_REASON_CIPHER_SUITE_REJECTED:
            return "WIFI_REASON_CIPHER_SUITE_REJECTED";
        case WIFI_REASON_TDLS_PEER_UNREACHABLE:
            return "WIFI_REASON_TDLS_PEER_UNREACHABLE";
        case WIFI_REASON_TDLS_UNSPECIFIED:
            return "WIFI_REASON_TDLS_UNSPECIFIED";
        case WIFI_REASON_SSP_REQUESTED_DISASSOC:
            return "WIFI_REASON_SSP_REQUESTED_DISASSOC";
        case WIFI_REASON_NO_SSP_ROAMING_AGREEMENT:
            return "WIFI_REASON_NO_SSP_ROAMING_AGREEMENT";
        case WIFI_REASON_BAD_CIPHER_OR_AKM:
            return "WIFI_REASON_BAD_CIPHER_OR_AKM";
        case WIFI_REASON_NOT_AUTHORIZED_THIS_LOCATION:
            return "WIFI_REASON_NOT_AUTHORIZED_THIS_LOCATION";
        case WIFI_REASON_SERVICE_CHANGE_PERCLUDES_TS:
            return "WIFI_REASON_SERVICE_CHANGE_PERCLUDES_TS";
        case WIFI_REASON_UNSPECIFIED_QOS:
            return "WIFI_REASON_UNSPECIFIED_QOS";
        case WIFI_REASON_NOT_ENOUGH_BANDWIDTH:
            return "WIFI_REASON_NOT_ENOUGH_BANDWIDTH";
        case WIFI_REASON_MISSING_ACKS:
            return "WIFI_REASON_MISSING_ACKS";
        case WIFI_REASON_EXCEEDED_TXOP:
            return "WIFI_REASON_EXCEEDED_TXOP";
        case WIFI_REASON_STA_LEAVING:
            return "WIFI_REASON_STA_LEAVING";
        case WIFI_REASON_END_BA:
            return "WIFI_REASON_END_BA";
        case WIFI_REASON_UNKNOWN_BA:
            return "WIFI_REASON_UNKNOWN_BA";
        case WIFI_REASON_TIMEOUT:
            return "WIFI_REASON_TIMEOUT";
        case WIFI_REASON_PEER_INITIATED:
            return "WIFI_REASON_PEER_INITIATED";
        case WIFI_REASON_AP_INITIATED:
            return "WIFI_REASON_AP_INITIATED";
        case WIFI_REASON_INVALID_FT_ACTION_FRAME_COUNT:
            return "WIFI_REASON_INVALID_FT_ACTION_FRAME_COUNT";
        case WIFI_REASON_INVALID_PMKID:
            return "WIFI_REASON_INVALID_PMKID";
        case WIFI_REASON_INVALID_MDE:
            return "WIFI_REASON_INVALID_MDE";
        case WIFI_REASON_INVALID_FTE:
            return "WIFI_REASON_INVALID_FTE";
        case WIFI_REASON_TRANSMISSION_LINK_ESTABLISH_FAILED:
            return "WIFI_REASON_TRANSMISSION_LINK_ESTABLISH_FAILED";
        case WIFI_REASON_ALTERATIVE_CHANNEL_OCCUPIED:
            return "WIFI_REASON_ALTERATIVE_CHANNEL_OCCUPIED";
        case WIFI_REASON_BEACON_TIMEOUT:
            return "WIFI_REASON_BEACON_TIMEOUT";
        case WIFI_REASON_NO_AP_FOUND:
            return "WIFI_REASON_NO_AP_FOUND";
        case WIFI_REASON_AUTH_FAIL:
            return "WIFI_REASON_AUTH_FAIL";
        case WIFI_REASON_ASSOC_FAIL:
            return "WIFI_REASON_ASSOC_FAIL";
        case WIFI_REASON_HANDSHAKE_TIMEOUT:
            return "WIFI_REASON_HANDSHAKE_TIMEOUT";
        case WIFI_REASON_CONNECTION_FAIL:
            return "WIFI_REASON_CONNECTION_FAIL";
        case WIFI_REASON_AP_TSF_RESET:
            return "WIFI_REASON_AP_TSF_RESET";
        case WIFI_REASON_ROAMING:
            return "WIFI_REASON_ROAMING";
        case WIFI_REASON_ASSOC_COMEBACK_TIME_TOO_LONG:
            return "WIFI_REASON_ASSOC_COMEBACK_TIME_TOO_LONG";
        case WIFI_REASON_SA_QUERY_TIMEOUT:
            return "WIFI_REASON_SA_QUERY_TIMEOUT";
        case WIFI_REASON_NO_AP_FOUND_W_COMPATIBLE_SECURITY:
            return "WIFI_REASON_NO_AP_FOUND_W_COMPATIBLE_SECURITY";
        case WIFI_REASON_NO_AP_FOUND_IN_AUTHMODE_THRESHOLD:
            return "WIFI_REASON_NO_AP_FOUND_IN_AUTHMODE_THRESHOLD";
        case WIFI_REASON_NO_AP_FOUND_IN_RSSI_THRESHOLD:
            return "WIFI_REASON_NO_AP_FOUND_IN_RSSI_THRESHOLD";
        default:
            return "UNKNOWN_WIFI_REASON";
    }

    return "";
}

/**===< local >================================================================
 * NAME:
 *      return_auth_mode_string() - gets a string representing the wifi
 *      authentication mode.
 *
 * SUMMARY:
 *      This uses a simple switch case with every defined enum for wifi
 *      authentication modes (from esp_wifi_types.h) and returns a string
 *      corresponding to the auth mode. Used primarily for logging.
 *
 * INPUT REQUIREMENTS:
 *      Must be a valid type of wifi authentication mode.
 *
 * OUTPUT GUARANTEES:
 *      Returns a pointer to a null-terminated string.
 **===< local >================================================================*/
static char* return_auth_mode_string(wifi_auth_mode_t auth_mode_e)
{
    switch (auth_mode_e)
    {
        case WIFI_AUTH_OPEN:
            return "WIFI_AUTH_OPEN";
        case WIFI_AUTH_WEP:
            return "WIFI_AUTH_WEP";
        case WIFI_AUTH_WPA_PSK:
            return "WIFI_AUTH_WPA_PSK";
        case WIFI_AUTH_WPA2_PSK:
            return "WIFI_AUTH_WPA2_PSK";
        case WIFI_AUTH_WPA_WPA2_PSK:
            return "WIFI_AUTH_WPA_WPA2_PSK";
        case WIFI_AUTH_ENTERPRISE:
            return "WIFI_AUTH_ENTERPRISE";
        case WIFI_AUTH_WPA3_PSK:
            return "WIFI_AUTH_WPA3_PSK";
        case WIFI_AUTH_WPA2_WPA3_PSK:
            return "WIFI_AUTH_WPA2_WPA3_PSK";
        case WIFI_AUTH_WAPI_PSK:
            return "WIFI_AUTH_WAPI_PSK";
        case WIFI_AUTH_OWE:
            return "WIFI_AUTH_OWE";
        case WIFI_AUTH_WPA3_ENT_192:
            return "WIFI_AUTH_WPA3_ENT_192";
        case WIFI_AUTH_WPA3_EXT_PSK:
            return "WIFI_AUTH_WPA3_EXT_PSK";
        case WIFI_AUTH_WPA3_EXT_PSK_MIXED_MODE:
            return "WIFI_AUTH_WPA3_EXT_PSK_MIXED_MODE";
        case WIFI_AUTH_MAX:
            return "WIFI_AUTH_MAX";
        default:
            return "UNKNOWN_WIFI_AUTH_MODE";
    }
}

/**===< local >================================================================
 * NAME:
 *      wifi_event_handler() - defines the behaviour for each wifi event
 *
 * SUMMARY:
 *      This is the primary callback when an event is received through the network
 *      interface. This defines logic for things like reconnecting when a signal
 *      is lost, and setting up connections when a mode (STA / AP) is chosen.
 *
 * INPUT REQUIREMENTS:
 *      Must be passed valid event parameters.
 *
 * OUTPUT GUARANTEES:
 *      ---
 **===< local >================================================================*/
void wifi_event_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    wifi_event_sta_connected_t  *sta_connection_event_s;
    wifi_event_sta_disconnected_t *sta_disconnection_event_s;

    wifi_event_ap_staconnected_t *ap_connection_event_s;
    wifi_event_ap_stadisconnected_t *ap_disconnection_event_s;

    switch (event_id)
    {
        /* Wifi started in station mode */
        case WIFI_EVENT_STA_START:
            ESP_LOGI(LOG_TAG, "Connecting...");
            esp_wifi_connect();
            break;

        /* Wifi connected in station mode */
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(LOG_TAG, "Connected!");
            reconnect_attempts_i32 = 0;

            CONNECT_start_interfaces();

            xEventGroupSetBits(wifi_events, FLAG_CONNECTED);
            break;

        /* Wifi disconnected in station mode */
        case WIFI_EVENT_STA_DISCONNECTED:
            sta_disconnection_event_s = event_data;
            ESP_LOGW(LOG_TAG, "Disconnected code %d: %s \n", sta_disconnection_event_s->reason,
                     return_error_string(sta_disconnection_event_s->reason));

            CONNECT_stop_interfaces();

            if(attempt_reconnect_b)
            {
                if(sta_disconnection_event_s->reason == WIFI_REASON_NO_AP_FOUND ||
                   sta_disconnection_event_s->reason == WIFI_REASON_ASSOC_LEAVE ||
                   sta_disconnection_event_s->reason == WIFI_REASON_AUTH_EXPIRE)
                {
                    if(reconnect_attempts_i32++ < 5)
                    {
                        vTaskDelay(pdMS_TO_TICKS(5000));
                        esp_wifi_connect();
                        break;
                    }
                    else
                    {
                        ESP_LOGE(LOG_TAG, "Exceeded maximum WIFI reconnection attempts.");
                    }
                }
            }

            xEventGroupSetBits(wifi_events, FLAG_DISCONNECTED);
            break;

        /* Got IP in station mode */
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(LOG_TAG, "Got IP!");
            xEventGroupSetBits(wifi_events, FLAG_ASSIGNED_IP);
            break;

        case WIFI_EVENT_AP_STACONNECTED:
            ap_connection_event_s = event_data;
            ESP_LOGI(LOG_TAG, "Station " MACSTR " joined, AID=%d", MAC2STR(ap_connection_event_s->mac), ap_connection_event_s->aid);
            break;

        case WIFI_EVENT_AP_STADISCONNECTED:
            ap_disconnection_event_s = event_data;
            ESP_LOGI(LOG_TAG, "Station " MACSTR " left, AID=%d", MAC2STR(ap_disconnection_event_s->mac), ap_disconnection_event_s->aid);
            break;

        case WIFI_EVENT_AP_START:
            ESP_LOGI(LOG_TAG, "AP Starting...\n");
            break;

        case WIFI_EVENT_AP_STOP:
            ESP_LOGI(LOG_TAG, "AP Stopping...\n");
            esp_netif_destroy(esp_netif);
            break;

        default:
            break;
    }
}

/**===< global >===============================================================
 * NAME:
 *      CONNECT_init() - initialize required interfaces for both STA and AP mode
 *
 * SUMMARY:
 *      Initializes the wifi configuration and assigns the event handler to
 *      relevant wifi events. Assigns the wifi storage to be RAM.
 *
 * INPUT REQUIREMENTS:
 *      ---
 *
 * OUTPUT GUARANTEES:
 *      ---
 **===< global >===============================================================*/
void CONNECT_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
}

/**===< global >===============================================================
 * NAME:
 *      CONNECT_wifi_sta() - connects to ssid as a station
 *
 * SUMMARY:
 *      This connects the device to the provided ssid as a station.
 *
 * INPUT REQUIREMENTS:
 *      ssid - valid ssid
 *      pass - correct password for ssid
 *      timeout - amount of time to wait for connection.
 *
 * OUTPUT GUARANTEES:
 *      Returns ESP_OK if the device connected and received an IP.
 *      Returns ESP_FAIL if the device was unable to connect.
 **===< global >===============================================================*/
esp_err_t CONNECT_wifi_sta(const char* ssid, const char* pass, int timeout)
{
    wifi_events = xEventGroupCreate();

    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password) - 1);

    esp_netif = esp_netif_create_default_wifi_sta();
    ESP_ERROR_CHECK(esp_netif_set_hostname(esp_netif, CONFIG_ESP_DEV_HOSTNAME));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t result = xEventGroupWaitBits(wifi_events, FLAG_ASSIGNED_IP | FLAG_DISCONNECTED, pdTRUE, pdFALSE, pdMS_TO_TICKS(timeout));

    if(result == FLAG_ASSIGNED_IP)
    {
        return ESP_OK;
    }
    return ESP_FAIL;
}

/**===< global >===============================================================
 * NAME:
 *      CONNECT_wifi_ap - sets the device up as an access point.
 *
 * SUMMARY:
 *      Sets the device up as an access point with the provided ssid and pass.
 *
 * INPUT REQUIREMENTS:
 *      ---
 *
 * OUTPUT GUARANTEES:
 *      ---
 **===< global >===============================================================*/
void CONNECT_wifi_ap(const char* ssid, const char* pass)
{
    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    strncpy((char *)wifi_config.ap.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *)wifi_config.ap.password, pass, sizeof(wifi_config.sta.password) - 1);
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    wifi_config.ap.max_connection = 4;
    wifi_config.ap.beacon_interval = 100;

    esp_netif = esp_netif_create_default_wifi_ap();
    ESP_ERROR_CHECK(esp_netif_set_hostname(esp_netif, CONFIG_ESP_DEV_HOSTNAME));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

/**===< global >===============================================================
 * NAME:
 *      CONNECT_wifi_disconnect - disconnects all wifi interfaces.
 *
 * SUMMARY:
 *      Sets the automatic reconnection to false, disconnects from wifi, and
 *      stops the wifi interface.
 *
 * INPUT REQUIREMENTS:
 *      ---
 *
 * OUTPUT GUARANTEES:
 *      ---
 **===< global >===============================================================*/
void CONNECT_wifi_disconnect(void)
{
    attempt_reconnect_b = false;
    esp_wifi_disconnect();
    esp_wifi_stop();
}