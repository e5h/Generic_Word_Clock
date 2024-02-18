/*==============================================================================
 *==============================================================================
 * NAME:
 *      lib_messaging.h
 *
 * PURPOSE:
 *      Part of the general library for projects.
 *
 *      This file provides context for all participants in the publish-subscribe
 *      messaging structure of the program. This file allows the routing of
 *      messages and definition of logic without underlying details of every
 *      module in the program.
 *
 * DEPENDENCIES:
 *      ---
 *
 *==============================================================================
 * (C) Andrew Bright 2024, github.com/e5h
 *==============================================================================
 *=============================================================================*/

#ifndef WC_LIB_MESSAGING_H

/*=============================================================================*/
/*][ Include Files ][==========================================================*/
/*=============================================================================*/

#include "lib_includes.h"
#include "button.h"

/*=============================================================================*/
/*][ GLOBAL : Constants and Types ][===========================================*/
/*=============================================================================*/

/* Topics for messaging */
typedef enum{
    MSG_ERROR = 0,
    MSG_BUTTONS,
    MSG_NETWORK,

    /* Number of message topics */
    NUM_MESSAGE_TOPICS,
} MESSAGE_TOPIC_E;

/* Struct to hold message content */
typedef struct{
    MESSAGE_TOPIC_E topic_e;
    union{
        BUTTON_EVENT_T btn_event_s;
    };
} MESSAGE_CONTENT_T;

/* Struct for message receivers */
typedef struct{
    EventGroupHandle_t* p_flags_s;          // Location of the flags
    EventBits_t         new_mail_flag_e;    // Which flag to set on new message
    QueueHandle_t       queue_s;            // Message queue to use
} MESSAGE_RECEIVER_T;

/*=============================================================================*/
/*][ GLOBAL : Exportable Variables ][==========================================*/
/*=============================================================================*/

extern const EventBits_t MESSAGE_default_mail_flag_e;

/*=============================================================================*/
/*][ GLOBAL : Exportable Function Prototypes ][================================*/
/*=============================================================================*/

extern STATUS_E MESSAGING_publish_to_topic( MESSAGE_TOPIC_E topic_e, MESSAGE_CONTENT_T* p_msg_s );
extern STATUS_E MESSAGING_subscribe_to_topic( MESSAGE_TOPIC_E topic_e, MESSAGE_RECEIVER_T* p_receiver_s );

/* End */
#define WC_LIB_MESSAGING_H
#endif