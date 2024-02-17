/*==============================================================================
 *==============================================================================
 * NAME:
 *      button.h
 *
 * PURPOSE:
 *      This module encapsulates the buttons.
 *
 * DEPENDENCIES:
 *      ---
 *
 *==============================================================================
 * (C) Andrew Bright 2024, github.com/e5h
 *==============================================================================
 *=============================================================================*/

#ifndef WC_BUTTON_H

/*=============================================================================*/
/*][ Include Files ][==========================================================*/
/*=============================================================================*/

#include "lib_includes.h"

/*=============================================================================*/
/*][ GLOBAL : Constants and Types ][===========================================*/
/*=============================================================================*/

enum { BTN_DEBOUNCE_MS  = 0 };
enum { BTN_HOLD_MS      = 500 };
enum { BTN_REPEAT_MS    = 100 };

/* Buttons on the control board */
typedef enum{
    BTN_WIFI                    = 0,        // Button for all wifi related functions
    BTN_COLOR                   = 1,        // Button for changing colors
    BTN_LIGHT                   = 2,        // Button for cycling brightness (may be unused)
    BTN_M5STAMP                 = 3,        // M5Stamp built-in button with RGB led

    /* Number of buttons */
    NUM_BUTTONS,
} BUTTON_E;

/* States of the buttons on the control board */
typedef enum{
    BTN_STATE_IDLE              = 0,        // Default, idle
    BTN_STATE_PRESS_DEBOUNCING  = 1,        // Rising edge detected, debouncing
    BTN_STATE_PRESS_HOLDING     = 2,        // Debounced, waiting for falling edge
    BTN_STATE_BEING_HELD        = 3,        // Still waiting for falling edge, met 'held' time
    BTN_STATE_REPEATING         = 4,        // Still waiting for falling edge, met 'repeat' time

    /* Number of button states */
    NUM_BUTTON_STATES,
} BUTTON_STATE_E;

/* Edges between recorded button reads */
typedef enum{
    BTN_EDGE_NONE               = 0,        // Default, no edge
    BTN_EDGE_RISING             = 1,        // Rising edge (inactive -> active)
    BTN_EDGE_FALLING            = 2,        // Falling edge (active -> inactive)

    /* Number of button edge types */
    NUM_BUTTON_EDGES,
} BUTTON_EDGE_E;

/* Types of button events to monitor for */
typedef enum{
    BTN_NO_EVENT                = 0x00,     // Default, no button event
    BTN_PRESSED                 = 0x01,     // Button was debounced, has met 'press' threshold
    BTN_PRESS_RELEASED          = 0x02,     // Button was released after being pressed
    BTN_HELD                    = 0x04,     // Button has remained held after press
    BTN_HOLD_RELEASED           = 0x08,     // Button was released after being held
    BTN_REPEAT                  = 0x10,     // Button has remained held, now repeating

    /* Number of button event types */
    NUM_BUTTON_EVENTS,
} BUTTON_EVENT_E;

/* State machine information for each button */
typedef struct{
    UINT64              state_timer_u64;    // Used for debouncing, holds, repeats
    BOOL                prev_active_b;      // Checking 'last press' status for edges
    BUTTON_EDGE_E       edge_e;             // Current edge type, used in state machine
    BUTTON_STATE_E      last_state_e;       // Previous button state (debouncing, hold, etc.)
    BUTTON_STATE_E      state_e;            // New button state
} BUTTON_STATE_INFO_T;

/* Button event, used for sending in queues */
typedef struct{
    BUTTON_E            button_e;           // Enum for the button (wifi, color, etc.)
    BUTTON_EVENT_E      event_e;            // Enum for the event type (pressed, held, etc.)
} BUTTON_EVENT_T;

/* Status for each button */
typedef struct{
    BUTTON_STATE_INFO_T state_info_s;       // Struct for button state information
    BUTTON_EVENT_T      event_s;            // Struct for button event
} BUTTON_STATUS_T;

/*=============================================================================*/
/*][ GLOBAL : Exportable Variables ][==========================================*/
/*=============================================================================*/

// GPIO expander handle

/*=============================================================================*/
/*][ GLOBAL : Exportable Function Prototypes ][================================*/
/*=============================================================================*/

extern STATUS_E    BUTTON_init();
extern STATUS_E    BUTTON_poll();
extern STATUS_E    BUTTON_update_state_machine();
extern STATUS_E    BUTTON_send_events_to_queue();

extern BOOL        BUTTON_check_event( BUTTON_EVENT_T* p_button_event_s, BUTTON_E button_to_check_e, BUTTON_EVENT_E events_to_check_e );
extern INT32       BUTTON_event_to_string( BUTTON_EVENT_T* p_button_event_s, CHAR* p_string_c, INT32 len_i32 );

/* End */
#define WC_BUTTON_H
#endif