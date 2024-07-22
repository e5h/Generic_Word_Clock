/*==============================================================================
 *==============================================================================
 * NAME:
 *      task_display.h
 *
 * PURPOSE:
 *      One of the major tasks of the application.
 *
 *      The display task is responsible for the RMT peripheral, and managing the
 *      entire context of the "clock" from an abstracted level. This involves
 *      translating system time to words, handling brightness and color changes,
 *      and handling animations on the display.
 *
 * DEPENDENCIES:
 *      ---
 *
 *==============================================================================
 * (C) Andrew Bright 2024, github.com/e5h
 *==============================================================================
 *=============================================================================*/

#ifndef WC_TASK_DISPLAY_H

/*=============================================================================*/
/*][ Include Files ][==========================================================*/
/*=============================================================================*/

#include "rgb_rmt.h"

/*=============================================================================*/
/*][ GLOBAL : Constants and Types ][===========================================*/
/*=============================================================================*/

/*=============================================================================*/
/*][ GLOBAL : Exportable Variables ][==========================================*/
/*=============================================================================*/

// TODO: message queue to main supervisor
// TODO: freertos variables to main supervisor

/*=============================================================================*/
/*][ GLOBAL : Exportable Function Prototypes ][================================*/
/*=============================================================================*/

extern void TASK_DISPLAY_create();

extern BOOL CLOCK_Init( void );
extern BOOL CLOCK_Tick( void );
extern BOOL CLOCK_UpdateTime( void );
extern BOOL CLOCK_TestWords( RGB_COLOR_PCT color );

/* End */
#define WC_TASK_DISPLAY_H
#endif