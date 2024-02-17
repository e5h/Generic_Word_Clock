/*==============================================================================
 *==============================================================================
 * NAME:
 *      button.c
 *
 * PURPOSE:
 *      This module encapsulates the buttons.
 * 
 *      Buttons are periodically polled, and the state machine will generate
 *      "button events" which are sent through a queue defined in the messaging
 *      library.
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

#include "button.h"
#include "lib_messaging.h"
#include "driver/gpio.h"

/*=============================================================================*/
/*][ LOCAL : Constants and Types ][============================================*/
/*=============================================================================*/

#define LOG_TAG "button.c" // Tag for optional ESP_LOGx calls

/*=============================================================================*/
/*][ LOCAL : Variables ][======================================================*/
/*=============================================================================*/

static BUTTON_STATUS_T p_button_statuses_s[ NUM_BUTTONS ];
static gpio_num_t p_button_gpio_pins_e[ NUM_BUTTONS ] =
{
    GPIO_NUM_4,     // Wifi button          (0) mapped to G4
    GPIO_NUM_6,     // Color button         (1) mapped to G6
    GPIO_NUM_5,     // Light button         (2) mapped to G5
    GPIO_NUM_3      // M5 builtin button    (3) mapped to G3
};

CHAR* p_button_names_c[] = { "Wifi", "Color", "Light", "M5" };
CHAR* p_state_names_c[] = { "idle", "debouncing", "pressed", "holding", "repeating" };

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
 *      button_read_status() - read the current status (on/off) of a button
 * 
 * SUMMARY:
 *      This is a local implementation, define the relevant HAL behaviour or
 *      otherwise to get a single button.
 * 
 * INPUT REQUIREMENTS:
 *      ---
 * 
 * OUTPUT GUARANTEES:
 *      Returns true (1) if the button is active, false (0) otherwise.
 **===< local >================================================================*/
static BOOL button_read_status( BUTTON_E button_e )
{
    BOOL button_status;

    /* Buttons are all active low */
    button_status = gpio_get_level( p_button_gpio_pins_e[ button_e ] ) ? FALSE : TRUE;

    return button_status;
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
 *      BUTTON_init() - initialize all relevant button states / information
 * 
 * SUMMARY:
 *      Sets up all the button statuses in the local static variable of type
 *      BUTTON_STATUS_T. Calls other required external functions as needed.
 * 
 * INPUT REQUIREMENTS:
 *      Button hardware inputs have already been initialized (GPIO, etc.)
 * 
 * OUTPUT GUARANTEES:
 *      Returns an accurate status enum, properly reports any errors.
 **===< global >===============================================================*/
STATUS_E BUTTON_init()
{
    ESP_LOGI( LOG_TAG, "Initializing buttons." );

    for( INT32 button_index_i32 = 0; button_index_i32 < NUM_BUTTONS; button_index_i32++ )
    {
        p_button_statuses_s[ button_index_i32 ].state_info_s.state_timer_u64 = 0;
        p_button_statuses_s[ button_index_i32 ].state_info_s.prev_active_b = FALSE;
        p_button_statuses_s[ button_index_i32 ].state_info_s.edge_e = BTN_EDGE_NONE;
        p_button_statuses_s[ button_index_i32 ].state_info_s.last_state_e = BTN_STATE_IDLE;
        p_button_statuses_s[ button_index_i32 ].state_info_s.state_e = BTN_STATE_IDLE;
        p_button_statuses_s[ button_index_i32 ].event_s.button_e = button_index_i32;
        p_button_statuses_s[ button_index_i32 ].event_s.event_e = BTN_NO_EVENT;
    }

    return STATUS_OK;
}

/**===< global >===============================================================
 * NAME:
 *      BUTTON_poll() - poll every defined button
 * 
 * SUMMARY:
 *      Calls the abstracted function "button_read_status()" for each button
 *      enum, and uses these values to update the button state machine.
 * 
 * INPUT REQUIREMENTS:
 *      ---
 * 
 * OUTPUT GUARANTEES:
 *      All button edges will be caught and recorded, regardless of polling rate.
 **===< global >===============================================================*/
STATUS_E BUTTON_poll()
{
    BOOL current_button_active_b;

    for( INT32 button_i32 = 0; button_i32 < NUM_BUTTONS; button_i32++ )
    {
        current_button_active_b = button_read_status( button_i32 );

        /* If the new 'active' status differs from the previous status */
        if( current_button_active_b != p_button_statuses_s[ button_i32 ].state_info_s.prev_active_b )
        {
            switch( current_button_active_b )
            {
                /* Newly active -> rising edge */
                case( TRUE ):
                {
                    p_button_statuses_s[ button_i32 ].state_info_s.edge_e = BTN_EDGE_RISING;
                    break;
                }

                /* Newly inactive -> falling edge */
                case( FALSE ):
                {
                    p_button_statuses_s[ button_i32 ].state_info_s.edge_e = BTN_EDGE_FALLING;
                    break;
                }

                /* Error case */
                default:
                {
                    p_button_statuses_s[ button_i32 ].state_info_s.edge_e = BTN_EDGE_NONE;
                    break;
                }
            }
        }
        /* If the new status has not changed */
        else
        {
            p_button_statuses_s[ button_i32 ].state_info_s.edge_e = BTN_EDGE_NONE;
        }

        /* Update the previous status */
        p_button_statuses_s[ button_i32 ].state_info_s.prev_active_b = current_button_active_b;
    }

    return STATUS_OK;
}

/**===< global >===============================================================
 * NAME:
 *      BUTTON_update_state_machine() - updates button states, and generates
 *      button events.
 * 
 * SUMMARY:
 *      Goes through each entry in the BUTTON_STATUS_T array and evaluates the
 *      edges generated in the polling function. Depending on the edge and the
 *      current state, a new button state may be entered.
 * 
 *      Some state changes will result in the generation of a button event which
 *      is stored in the same BUTTON_STATUS_T array entry.
 * 
 *      This function should be called periodically, or after a button poll
 *      occurs.
 * 
 * INPUT REQUIREMENTS:
 *      ---
 * 
 * OUTPUT GUARANTEES:
 *      All button state transitions and button events will be caught and
 *      recorded successfully, regardless of the update rate.
 * 
 *      If any new event is generated, it will be returned as a status.
 **===< global >===============================================================*/
STATUS_E BUTTON_update_state_machine()
{
    STATUS_E update_status_e = STATUS_NO_CHANGE;

    BUTTON_EDGE_E       edge_e;
    BUTTON_STATE_E      curr_state_e;
    BUTTON_STATE_E      last_state_e;

    for( INT32 button_i32 = 0; button_i32 < NUM_BUTTONS; button_i32++ )
    {
        edge_e          = p_button_statuses_s[ button_i32 ].state_info_s.edge_e;
        curr_state_e    = p_button_statuses_s[ button_i32 ].state_info_s.state_e;
        last_state_e    = p_button_statuses_s[ button_i32 ].state_info_s.last_state_e;

        switch( curr_state_e )
        {
            case( BTN_STATE_IDLE ):
            {
                /* Newly active from idle, begin debouncing a potential press */
                if( edge_e == BTN_EDGE_RISING )
                {
                    p_button_statuses_s[ button_i32 ].state_info_s.state_e = BTN_STATE_PRESS_DEBOUNCING;
                }
                break;
            }

            case( BTN_STATE_PRESS_DEBOUNCING ):
            {
                /* If debouncing just began, start a timer with the debounce timer duration */
                if( curr_state_e != last_state_e )
                {
                    p_button_statuses_s[ button_i32 ].state_info_s.state_timer_u64 = TIMER_TimerStartMs( BTN_DEBOUNCE_MS );
                }

                /* If the button became inactive, cancel debouncing */
                if( edge_e == BTN_EDGE_FALLING )
                {
                    p_button_statuses_s[ button_i32 ].state_info_s.state_e = BTN_STATE_IDLE;
                }

                /* Otherwise, check the timer to see if the debounce delay has elapsed */
                else if( TIMER_TimerHasExpiredMs( p_button_statuses_s[ button_i32 ].state_info_s.state_timer_u64 ) )
                {
                    /* Button has met the 'press' requirement, queue state and set event */
                    p_button_statuses_s[ button_i32 ].state_info_s.state_e = BTN_STATE_PRESS_HOLDING;
                    p_button_statuses_s[ button_i32 ].event_s.event_e = BTN_PRESSED;
                    update_status_e = STATUS_NEW_EVENT;
                }
                break;
            }

            case( BTN_STATE_PRESS_HOLDING ):
            {
                /* If button was just pressed, start a timer with the hold timer duration */
                if( curr_state_e != last_state_e )
                {
                    p_button_statuses_s[ button_i32 ].state_info_s.state_timer_u64 = TIMER_TimerStartMs( BTN_HOLD_MS );
                }

                /* If the button became inactive, cancel holding */
                if( edge_e == BTN_EDGE_FALLING )
                {
                    p_button_statuses_s[ button_i32 ].state_info_s.state_e = BTN_STATE_IDLE;
                    p_button_statuses_s[ button_i32 ].event_s.event_e = BTN_PRESS_RELEASED;
                    update_status_e = STATUS_NEW_EVENT;
                }

                /* Otherwise, check the timer to see if the hold duration has elapsed */
                else if( TIMER_TimerHasExpiredMs( p_button_statuses_s[ button_i32 ].state_info_s.state_timer_u64 ) )
                {
                    /* Button has met the 'hold' requirement, queue state and set event */
                    p_button_statuses_s[ button_i32 ].state_info_s.state_e = BTN_STATE_BEING_HELD;
                    p_button_statuses_s[ button_i32 ].event_s.event_e = BTN_HELD;
                    update_status_e = STATUS_NEW_EVENT;
                }
                break;
            }

            case( BTN_STATE_BEING_HELD ):
            {
                /* If button was just held, start a timer with the hold timer duration before repeating */
                if( curr_state_e != last_state_e )
                {
                    p_button_statuses_s[ button_i32 ].state_info_s.state_timer_u64 = TIMER_TimerStartMs( BTN_HOLD_MS );
                }

                /* If the button became inactive, cancel hold-for-repeat */
                if( edge_e == BTN_EDGE_FALLING )
                {
                    p_button_statuses_s[ button_i32 ].state_info_s.state_e = BTN_STATE_IDLE;
                    p_button_statuses_s[ button_i32 ].event_s.event_e = BTN_HOLD_RELEASED;
                    update_status_e = STATUS_NEW_EVENT;
                }

                /* Otherwise, check the timer to see if the hold duration has elapsed */
                else if( TIMER_TimerHasExpiredMs( p_button_statuses_s[ button_i32 ].state_info_s.state_timer_u64 ) )
                {
                    /* Button has met the 'repeat' requirement, queue state and set event */
                    p_button_statuses_s[ button_i32 ].state_info_s.state_e = BTN_STATE_REPEATING;
                    p_button_statuses_s[ button_i32 ].event_s.event_e = BTN_REPEAT;
                    update_status_e = STATUS_NEW_EVENT;
                }
                break;
            }

            case( BTN_STATE_REPEATING ):
            {
                /* If repeating just began, begin a timer with the repeat delay */
                if( curr_state_e != last_state_e )
                {
                    p_button_statuses_s[ button_i32 ].state_info_s.state_timer_u64 = TIMER_TimerStartMs( BTN_REPEAT_MS );
                }

                /* If the button became inactive, return to idle */
                if( edge_e == BTN_EDGE_FALLING )
                {
                    p_button_statuses_s[ button_i32 ].state_info_s.state_e = BTN_STATE_IDLE;
                    p_button_statuses_s[ button_i32 ].event_s.event_e = BTN_HOLD_RELEASED;
                    update_status_e = STATUS_NEW_EVENT;
                }

                /* Otherwise, check the timer to see if the repeat delay elapsed */
                else if( TIMER_TimerHasExpiredMs( p_button_statuses_s[ button_i32 ].state_info_s.state_timer_u64 ) )
                {
                    /* Button has met the 'repeat' requirement, queue state and set event */
                    p_button_statuses_s[ button_i32 ].event_s.event_e = BTN_REPEAT;
                    update_status_e = STATUS_NEW_EVENT;

                    /* Restart the timer for another repeat */
                    p_button_statuses_s[ button_i32 ].state_info_s.state_timer_u64 = TIMER_TimerStartMs( BTN_REPEAT_MS );
                }
                break;
            }

            default:
            {
                break;
            }
        }/*end-switch*/

        /* Update the last state */
        p_button_statuses_s[ button_i32 ].state_info_s.last_state_e = curr_state_e;

    }/*end-for*/

    return update_status_e;
}

/**===< global >===============================================================
 * NAME:
 *      BUTTON_send_events_to_queue() - check for new events, send them to other
 *      areas in the program via the messaging library.
 * 
 * SUMMARY:
 *      Goes through each element in the BUTTON_STATUS_T array and checks if
 *      anything other than "BTN_NO_EVENT" is present. If an actual event is
 *      present, it sends it through a queue defined in lib_messaging.h. At
 *      the end, clears all pending events.
 * 
 * INPUT REQUIREMENTS:
 *      ---
 * 
 * OUTPUT GUARANTEES:
 *      Returns an "OK" status if all events were sent successfully.
 **===< global >===============================================================*/
STATUS_E BUTTON_send_events_to_queue()
{
    STATUS_E send_status_e = STATUS_OK;

    // TODO: Message data

    for( INT32 button_i32 = 0; button_i32 < NUM_BUTTONS; button_i32++ )
    {
        // TODO: Populate message data struct

        // TODO: Check if != BTN_NO_EVENT
        if( 0 )
        {
            // TODO: Conditional check for message broadcast success
            if( 0 )
            {
                // TODO: Success status
            }
            else
            {
                // TODO: Fail status
            }

            p_button_statuses_s[ button_i32 ].event_s.event_e = BTN_NO_EVENT;
        }
    }

    return send_status_e;
}

/**===< global >===============================================================
 * NAME:
 *      BUTTON_check_event() - checks the provided button event against a specific
 *      button and one of several event types
 * 
 * SUMMARY:
 *      Takes a BUTTON_EVENT_T and checks if it matches with the provided BUTTON_E
 *      and BUTTON_EVENT_E. Used to verify if a specific button was used in a
 *      specific way.
 * 
 *      Since BUTTON_EVENT_E enums are individual bits, multiple options can be
 *      passed in a single call if different press types are valid.
 * 
 *      (ie.) Editing a number
 *          - All press types are valid to increment
 * 
 *          BUTTON_check_event( *, *, BTN_PRESSED | BTN_HELD | BTN_REPEAT );
 * 
 * INPUT REQUIREMENTS:
 *      BUTTON_EVENT_T* - must be a valid pointer
 * 
 * OUTPUT GUARANTEES:
 *      Will return true (1) if there is a match, false (0) otherwise.
 **===< global >===============================================================*/
BOOL BUTTON_check_event( BUTTON_EVENT_T* p_button_event_s, BUTTON_E button_to_check_e, BUTTON_EVENT_E events_to_check_e )
{
    BUTTON_E        received_button_e   = p_button_event_s->button_e;   // Received buttton
    BUTTON_EVENT_E  received_event_e    = p_button_event_s->event_e;    // Received event type
    BOOL            event_match_b       = FALSE;                        // Is it a match?

    /* Compare buttons */
    if( received_button_e == button_to_check_e )
    {
        /* Compare event bits */
        if( ( received_event_e & events_to_check_e ) == received_event_e )
        {
            event_match_b = TRUE;
        }
    }

    return event_match_b;
}

/**===< global >===============================================================
 * NAME:
 *      BUTTON_event_to_string() - turns a button event to a string.
 * 
 * SUMMARY:
 *      Takes a button event, a pointer to a character buffer, and a maximum
 *      length to print. Will print a readable string into the passed buffer.
 * 
 * INPUT REQUIREMENTS:
 *      CHAR* - must be a valid pointer to a buffer
 *      INT32 - must be >0
 * 
 * OUTPUT GUARANTEES:
 *      Will return the number of characters printed into the buffer.
 *          - Returns -1 if an error occurs
 *      Will not print more than the INT32 limit into the buffer.
 **===< global >===============================================================*/
INT32 BUTTON_event_to_string( BUTTON_EVENT_T* p_button_event_s, CHAR* p_string_c, INT32 len_i32 )
{
    if( p_string_c == NULL || len_i32 <= 0 ){ return -1; }

    BUTTON_E    button_e                = p_button_event_s->button_e;
    INT32       num_chars_printed_i32   = 0;
    CHAR        p_event_string_c[ 20 ];

    switch( p_button_event_s->event_e )
    {
        case( BTN_NO_EVENT ):
        {
            snprintf( p_event_string_c, 20 - 1, "no event." );
            break;
        }

        case( BTN_PRESSED ):
        {
            snprintf( p_event_string_c, 20 - 1, "being pressed..." );
            break;
        }

        case( BTN_PRESS_RELEASED ):
        {
            snprintf( p_event_string_c, 20 - 1, "was pressed." );
            break;
        }

        case( BTN_HELD ):
        {
            snprintf( p_event_string_c, 20 - 1, "being held..." );
            break;
        }

        case( BTN_HOLD_RELEASE ):
        {
            snprintf( p_event_string_c, 20 - 1, "was held." );
            break;
        }

        case( BTN_REPEAT ):
        {
            snprintf( p_event_string_c, 20 - 1, "repeated!" );
            break;
        }

        default:
        {
            snprintf( p_event_string_c, 20 - 1, "ERROR" );
            break;
        }
    }

    num_chars_printed_i32 = snprintf( p_string_c, len_i32 - 1, "%s %s", p_button_names_c[ button_e ], p_event_string_c );
    return num_chars_printed_i32;
}

/* End */