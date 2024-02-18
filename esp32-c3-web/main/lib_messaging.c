/*==============================================================================
 *==============================================================================
 * NAME:
 *      lib_messaging.c
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
 *      lib_messaging.h
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

#include "lib_messaging.h"

/*=============================================================================*/
/*][ LOCAL : Constants and Types ][============================================*/
/*=============================================================================*/

enum { MAX_MESSAGE_RECEIVERS = 10 };

/* Struct for router. A topic, and a pointer array for subscribed receivers */
typedef struct{
    MESSAGE_TOPIC_E     topic_e;
    MESSAGE_RECEIVER_T* p_receivers_s[ MAX_MESSAGE_RECEIVERS ];
    INT32               num_subscribers_i32;
} router_entry_t;

/*=============================================================================*/
/*][ GLOBAL : Variables ][=====================================================*/
/*=============================================================================*/

const EventBits_t MESSAGE_default_mail_flag_e = (1 << 24);  // Bit 24

/*=============================================================================*/
/*][ LOCAL : Variables ][======================================================*/
/*=============================================================================*/

/* The routing table used for the messaging library */
static router_entry_t message_routing_table[ NUM_MESSAGE_TOPICS ];

/*=============================================================================*/
/*][ LOCAL : Function Definitions ][===========================================*/
/*=============================================================================*/

/**===< local >================================================================
 * NAME:
 *      init_router() - initialize the routing table
 * 
 * SUMMARY:
 *      This is meant to be called by the other functions since no specific area
 *      of the program is responsible for initializing the messaging. If it
 *      has already been initialized, it simply returns STATUS_OK.
 * 
 * INPUT REQUIREMENTS:
 *      ---
 * 
 * OUTPUT GUARANTEES:
 *      Returns STATUS_OK if initialized
 **===< local >================================================================*/
STATUS_E init_router()
{
    static STATUS_E init_status_e = 0;

    if( init_status_e < TRUE )
    {
        for( INT32 message_topic = 0; message_topic < NUM_MESSAGE_TOPICS; message_topic++ )
        {
            message_routing_table[ message_topic ].topic_e = message_topic;
            message_routing_table[ message_topic ].num_subscribers_i32 = 0;
        }

        init_status_e = STATUS_OK;
    }

    return init_status_e;
}

/**===< global >===============================================================
 * NAME:
 *      MESSAGING_publish_to_topic() - publish a message from anywhere
 * 
 * SUMMARY:
 *      A message is sent to the message router from any part of the program.
 * 
 * INPUT REQUIREMENTS:
 *      - Valid topic
 *      - Valid pointer
 * 
 * OUTPUT GUARANTEES:
 **===< global >===============================================================*/
STATUS_E MESSAGING_publish_to_topic( MESSAGE_TOPIC_E topic_e, MESSAGE_CONTENT_T* p_msg_s )
{
    /* Check if the router is initialized */
    if( init_router() != STATUS_OK )
    {
        return STATUS_ERR;
    }

    /* Check for valid topic */
    if( topic_e == MSG_ERROR || topic_e >= NUM_MESSAGE_TOPICS )
    {
        return STATUS_ERR_PARAM;
    }

    /* Check for valid message */
    if( p_msg_s == NULL )
    {
        return STATUS_NULL_PTR;
    }

    STATUS_E            publish_status_e    = STATUS_OK;
    MESSAGE_RECEIVER_T* p_curr_receiver_s;
    router_entry_t*     p_router_entry_s    = &message_routing_table[ topic_e ];

    /* For each subscriber to this topic */
    for( INT32 topic_subscriber = 0;
         topic_subscriber < p_router_entry_s->num_subscribers_i32;
         topic_subscriber++ )
    {
        p_curr_receiver_s = p_router_entry_s->p_receivers_s[ topic_subscriber ];

        /* Check if the receiver has been initialized */
        if( p_curr_receiver_s->queue_s == NULL || p_curr_receiver_s->p_flags_s == NULL )
        {
            return STATUS_QUEUE_ERROR;
        }

        /* Check the send status */
        if( xQueueSend( p_curr_receiver_s->queue_s, p_msg_s, 0 ) == pdPASS )
        {
            xEventGroupSetBits( *(p_curr_receiver_s->p_flags_s), p_curr_receiver_s->new_mail_flag_e );
        }
        else
        {
            publish_status_e = STATUS_ERR;
        }
    }

    return publish_status_e;
}

/**===< global >===============================================================
 * NAME:
 *      MESSAGE_subscribe_to_topic() - subscribe a message receiver to a topic
 * 
 * SUMMARY:
 *      A message receiver struct is passed to the function, and registered
 *      internally. If the fields are not initialized, they will be initialized.
 * 
 * INPUT REQUIREMENTS:
 *      - Valid topic
 *      - Valid pointer
 * 
 * OUTPUT GUARANTEES:
 *      Returns error status
 **===< global >===============================================================*/
STATUS_E MESSAGING_subscribe_to_topic( MESSAGE_TOPIC_E topic_e, MESSAGE_RECEIVER_T* p_receiver_s )
{
    /* Check if the router is initialized */
    if( init_router() != STATUS_OK )
    {
        return STATUS_ERR;
    }

    /* Check for valid topic */
    if( topic_e == MSG_ERROR || topic_e >= NUM_MESSAGE_TOPICS )
    {
        return STATUS_ERR_PARAM;
    }

    /* Check for valid receiver */
    if( p_receiver_s == NULL )
    {
        return STATUS_NULL_PTR;
    }

    STATUS_E            subscribe_status_e  = STATUS_OK;
    router_entry_t*     p_router_entry_s    = &message_routing_table[ topic_e ];

    /* Check if maximum subscribers */
    if( p_router_entry_s->num_subscribers_i32 >= MAX_MESSAGE_RECEIVERS )
    {
        p_router_entry_s->num_subscribers_i32 = MAX_MESSAGE_RECEIVERS;
        return STATUS_ERR;
    }

    /* Initialize the receiver if necessary */
    /* Queue */
    if( p_receiver_s->queue_s == NULL )
    {
        p_receiver_s->queue_s = xQueueCreate( 10, sizeof( MESSAGE_CONTENT_T ) );
    }

    /* Event group */
    if( p_receiver_s->p_flags_s == NULL )
    {
        *(p_receiver_s->p_flags_s) = xEventGroupCreate();
    }

    /* New mail flag */
    if( p_receiver_s->new_mail_flag_e == 0 )
    {
        p_receiver_s->new_mail_flag_e = MESSAGE_default_mail_flag_e;
    }

    /* Assign the receiver to routing table */
    p_router_entry_s->p_receivers_s[ p_router_entry_s->num_subscribers_i32++ ] = p_receiver_s;

    return subscribe_status_e;
}