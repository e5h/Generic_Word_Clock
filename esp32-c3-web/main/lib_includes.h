/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * NAME:
 *      lib_includes.h
 *
 * PURPOSE:
 *      Part of the general library for projects.
 *
 *      This file includes the common and typical header files, designed to save
 *      space and reduce complexity.
 *
 * DEPENDENCIES:
 *      none
 *
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
 * (C) Andrew Bright 2023, github.com/e5h
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

#ifndef WC_LIB_INCLUDES_H

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ Feature Switches ][~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

#define LIB_INCLUDES_FREERTOS       (1) /* 1 = FreeRTOS used, 0 = unused */
#define LIB_INCLUDES_USING_CPP      (0) /* 1 = C++, 0 = C */

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ Include Files ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/* HAL and C standard */
#if LIB_INCLUDES_USING_CPP ==  1
#include <cctype>
#include <cstdio>
#include <cstring>
#else
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#endif

/* FreeRTOS dependent */
#if LIB_INCLUDES_FREERTOS == 1
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#endif

/* User defined */
#include "lib_types.h"
#include "lib_timer.h"
#include "esp_log.h"

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ GLOBAL : Constants and Types ][~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/* End */
#define WC_LIB_INCLUDES_H
#endif

/* Function templates */

/**===< local >================================================================
 * NAME:
 * 
 * SUMMARY:
 * 
 * INPUT REQUIREMENTS:
 * 
 * OUTPUT GUARANTEES:
 **===< local >================================================================*/

/**===< global >===============================================================
 * NAME:
 * 
 * SUMMARY:
 * 
 * INPUT REQUIREMENTS:
 * 
 * OUTPUT GUARANTEES:
 **===< global >===============================================================*/