/*==============================================================================
 *==============================================================================
 * NAME:
 *      lib_connect.h
 *
 * PURPOSE:
 *      Component for simple wifi connections.
 *
 * DEPENDENCIES:
 *      ---
 *
 *==============================================================================
 * (C) Andrew Bright 2024, github.com/e5h
 *==============================================================================
 *=============================================================================*/

#ifndef H_LIB_CONNECT

/*=============================================================================*/
/*][ Feature Switches ][=======================================================*/
/*=============================================================================*/

/*=============================================================================*/
/*][ Include Files ][==========================================================*/
/*=============================================================================*/

#include "esp_err.h"

/*=============================================================================*/
/*][ GLOBAL : Constants and Types ][===========================================*/
/*=============================================================================*/

/*=============================================================================*/
/*][ GLOBAL : Exportable Variables ][==========================================*/
/*=============================================================================*/

/*=============================================================================*/
/*][ GLOBAL : Exportable Function Prototypes ][================================*/
/*=============================================================================*/

void        CONNECT_init(void);
esp_err_t   CONNECT_wifi_sta(const char* ssid, const char* pass, int timeout_ms);
void        CONNECT_wifi_ap(const char* ssid, const char* pass);
void        CONNECT_wifi_disconnect(void);
extern void CONNECT_stop_interfaces(void);
extern void CONNECT_start_interfaces(void);

/* End */
#define H_LIB_CONNECT
#endif