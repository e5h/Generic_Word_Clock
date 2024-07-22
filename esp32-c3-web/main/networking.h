/*==============================================================================
 *==============================================================================
 * NAME:
 *      networking.h
 *
 * PURPOSE:
 *      Higher level interfaces related to networking in the application. These
 *      include:
 *          -> NTP time retrieval
 *          -> REST api endpoints
 *          -> Static webpage hosting
 *
 * DEPENDENCIES:
 *      ---
 *
 *==============================================================================
 * (C) Andrew Bright 2024, github.com/e5h
 *==============================================================================
 *=============================================================================*/

#ifndef WC_NETWORKING_H

/*=============================================================================*/
/*][ Feature Switches ][=======================================================*/
/*=============================================================================*/

/*=============================================================================*/
/*][ Include Files ][==========================================================*/
/*=============================================================================*/

#include "lib_includes.h"

/*=============================================================================*/
/*][ GLOBAL : Constants and Types ][===========================================*/
/*=============================================================================*/

#ifdef CONFIG_ESP_TZ_STRING_CUT0GDT
#define BASE_TZ_STRING "CUT0GDT"
#elif defined(CONFIG_ESP_TZ_STRING_GMT0BST)
#define BASE_TZ_STRING "GMT0BST"
#elif defined(CONFIG_ESP_TZ_STRING_AZOREST1AZOREDT)
#define BASE_TZ_STRING "AZOREST1AZOREDT"
#elif defined(CONFIG_ESP_TZ_STRING_FALKST2FALKDT)
#define BASE_TZ_STRING "FALKST2FALKDT"
#elif defined(CONFIG_ESP_TZ_STRING_GRNLNDST3GRNLNDDT)
#define BASE_TZ_STRING "GRNLNDST3GRNLNDDT"
#elif defined(CONFIG_ESP_TZ_STRING_AST4ADT)
#define BASE_TZ_STRING "AST4ADT"
#elif defined(CONFIG_ESP_TZ_STRING_EST5EDT)
#define BASE_TZ_STRING "EST5EDT"
#elif defined(CONFIG_ESP_TZ_STRING_CST6CDT)
#define BASE_TZ_STRING "CST6CDT"
#elif defined(CONFIG_ESP_TZ_STRING_MST7MDT)
#define BASE_TZ_STRING "MST7MDT"
#elif defined(CONFIG_ESP_TZ_STRING_PST8PDT)
#define BASE_TZ_STRING "PST8PDT"
#elif defined(CONFIG_ESP_TZ_STRING_AST9ADT_ALASKA)
    #define BASE_TZ_STRING "AST9ADT"
#elif defined(CONFIG_ESP_TZ_STRING_HST10HDT)
    #define BASE_TZ_STRING "HST10HDT"
#elif defined(CONFIG_ESP_TZ_STRING_BST11BDT)
    #define BASE_TZ_STRING "BST11BDT"
#elif defined(CONFIG_ESP_TZ_STRING_NZST_12NZDT)
    #define BASE_TZ_STRING "NZST-12NZDT"
#elif defined(CONFIG_ESP_TZ_STRING_MET_11METDT)
    #define BASE_TZ_STRING "MET-11METDT"
#elif defined(CONFIG_ESP_TZ_STRING_EET_10EETDT)
    #define BASE_TZ_STRING "EET-10EETDT"
#elif defined(CONFIG_ESP_TZ_STRING_JST_9JSTDT)
    #define BASE_TZ_STRING "JST-9JSTDT"
#elif defined(CONFIG_ESP_TZ_STRING_KORST_9KORDT)
    #define BASE_TZ_STRING "KORST-9KORDT"
#elif defined(CONFIG_ESP_TZ_STRING_WAUST_8WAUDT)
    #define BASE_TZ_STRING "WAUST-8WAUDT"
#elif defined(CONFIG_ESP_TZ_STRING_TAIST_8TAIDT)
    #define BASE_TZ_STRING "TAIST-8TAIDT"
#elif defined(CONFIG_ESP_TZ_STRING_THAIST_7THAIDT)
    #define BASE_TZ_STRING "THAIST-7THAIDT"
#elif defined(CONFIG_ESP_TZ_STRING_TASHST_6TASHDT)
    #define BASE_TZ_STRING "TASHST-6TASHDT"
#elif defined(CONFIG_ESP_TZ_STRING_PAKST_5PAKDT)
    #define BASE_TZ_STRING "PAKST-5PAKDT"
#elif defined(CONFIG_ESP_TZ_STRING_WST_4WDT)
    #define BASE_TZ_STRING "WST-4WDT"
#elif defined(CONFIG_ESP_TZ_STRING_MEST_3MEDT)
    #define BASE_TZ_STRING "MEST-3MEDT"
#elif defined(CONFIG_ESP_TZ_STRING_SAUST_3SAUDT)
    #define BASE_TZ_STRING "SAUST-3SAUDT"
#elif defined(CONFIG_ESP_TZ_STRING_WET_2WET)
    #define BASE_TZ_STRING "WET-2WET"
#elif defined(CONFIG_ESP_TZ_STRING_USAST_2USADT)
    #define BASE_TZ_STRING "USAST-2USADT"
#elif defined(CONFIG_ESP_TZ_STRING_NFT_1DFT)
    #define BASE_TZ_STRING "NFT-1DFT"
#else
    #warning "No timezone!"
#endif

/*=============================================================================*/
/*][ GLOBAL : Exportable Variables ][==========================================*/
/*=============================================================================*/

/*=============================================================================*/
/*][ GLOBAL : Exportable Function Prototypes ][================================*/
/*=============================================================================*/

// NTP related functions
extern esp_err_t NETWORKING_NTP_init();
extern esp_err_t NETWORKING_NTP_get_time_string( CHAR* p_buffer_c, INT32 len_i32 );
extern esp_err_t NETWORKING_NTP_get_time_stamp( CHAR* p_buffer_c, INT32 len_i32 );
extern esp_err_t NETWORKING_NTP_get_time_info( struct tm** p_time_info_s );

// API / webpage related functions
extern esp_err_t NETWORKING_http_server_init();

extern esp_err_t NETWORKING_populate_clock_status_struct();

/* End */
#define WC_NETWORKING_H
#endif