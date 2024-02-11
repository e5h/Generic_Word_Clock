/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * NAME:
 *      lib_types.h
 *
 * PURPOSE:
 *      Part of the general library for projects.
 *
 *      This file provides translations between system-specific data types and
 *      their portable definitions.
 *
 * DEPENDENCIES:
 *      ---
 *
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
 * (C) Andrew Bright 2023, github.com/e5h
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

#ifndef WC_LIB_TYPES_H

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ Feature Switches ][~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

#define LIB_TYPES_USING_CPP     (0) /* 1 = C++, 0 = C */

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ Include Files ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

#if LIB_TYPES_USING_CPP == 1
#include <cstdint>
#include <cinttypes>
#else
#include <stdint.h>
#include <inttypes.h>
#endif

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ GLOBAL : Constants and Types ][~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/* Append the bracketed types to the end of declared variable names */

typedef int8_t      BOOL;       /* ( _b ) */
typedef char        CHAR;       /* ( _c ) */
typedef int8_t      INT8;       /* ( _i8 ) */
typedef int16_t     INT16;      /* ( _i16 ) */
typedef int32_t     INT32;      /* ( _i32 ) */
typedef int64_t     INT64;      /* ( _i64 ) */

typedef char*       STRING;     /* ( _str ) */
typedef uint8_t     UINT8;      /* ( _u8 ) */
typedef uint8_t     UCHAR;      /* ( _u8 ) */
typedef uint16_t    UINT16;     /* ( _u16 ) */
typedef uint32_t    UINT32;     /* ( _u32 ) */
typedef uint64_t    UINT64;     /* ( _u64 ) */

typedef void        VOID;       /* ( _v ) */

typedef float       FLOAT32;    /* ( _f32 ) */
typedef double      FLOAT64;    /* ( _f64 ) */

#define TRUE        (1)
#define FALSE       (0)

/* Function return status enums */

typedef enum{
    /* Failure codes */
    STATUS_NO_CHANGE    = -1,   // No changes
    STATUS_ERR          = 0,    // Boolean compatible false

    /* Success codes */
    STATUS_OK           = 1,    // Boolean compatible true
    STATUS_NEW_EVENT    = 2,    // New event of some sort

} STATUS_E;

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ GLOBAL : Exportable Variables ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*][ GLOBAL : Exportable Function Prototypes ][*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

/* End */
#define WC_LIB_TYPES_H
#endif