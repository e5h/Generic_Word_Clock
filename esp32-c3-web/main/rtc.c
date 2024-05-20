/*==============================================================================
 *==============================================================================
 * NAME:
 *      rtc.c
 *
 * PURPOSE:
 *      This module encapsulates the real time clock.
 *
 *      The real time clock used in this module is a PCF85263A manufactured
 *      by NXP.
 *
 * DEPENDENCIES:
 *      rtc.h
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

#include "rtc.h"

/*=============================================================================*/
/*][ LOCAL : Constants and Types ][============================================*/
/*=============================================================================*/

#define LOG_TAG "rtc.c"     // Tag for optional ESP_LOGx calls

typedef struct{
    UINT8 address_u8;
} RTC_T;

/*=============================================================================*/
/*][ LOCAL : Variables ][======================================================*/
/*=============================================================================*/

volatile bool rtc_initialized_b = FALSE;
static RTC_T rtc_s;

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
static STATUS_E i2c_write(UINT8 dev_addr_u8, UINT8 reg_addr_u8, UINT8* buf_u8, INT32 len_i32)
{
    STATUS_E status_e = STATUS_OK;
    // TODO: Managed i2c write

    return status_e;
}


/**===< local >================================================================
 * NAME:
 *
 * SUMMARY:
 *
 * INPUT REQUIREMENTS:
 *
 * OUTPUT GUARANTEES:
 **===< local >================================================================*/
static STATUS_E i2c_read(UINT8 dev_addr_u8, UINT8 reg_addr_u8, UINT8* buf_u8, INT32 len_i32)
{
    STATUS_E status_e = STATUS_OK;
    // TODO: Managed i2c read

    return status_e;
}

/**===< global >===============================================================
 * NAME: RTC_init() - Initialize the real time clock
 *
 * SUMMARY:
 *
 * INPUT REQUIREMENTS:
 *
 * OUTPUT GUARANTEES:
 **===< global >===============================================================*/
STATUS_E RTC_init(UINT8 rtc_addr_u8)
{
    STATUS_E status_e = STATUS_OK;

    return status_e;
}

/**===< global >===============================================================
 * NAME: RTC_get_time() - Get the current time from the RTC
 *
 * SUMMARY:
 *
 * INPUT REQUIREMENTS:
 *
 * OUTPUT GUARANTEES:
 **===< global >===============================================================*/
STATUS_E RTC_get_time(struct tm* p_timestamp_s)
{
    STATUS_E status_e = STATUS_OK;

    // TODO: sequential read from RTC into a buffer
    // start
    // i2c address + write bit
    // register address (seconds)
    // stop

    // start
    // i2c address + read bit
    // read: seconds data
    // read: minutes data
    // read: hours data
    // read: days data
    // read: weekdays data
    // read: months data
    // read: years data
    // stop

    // TODO: convert the BCD values to decimal

    return status_e;
}

/**===< global >===============================================================
 * NAME: RTC_set_time() - Set the timestamp on the RTC
 *
 * SUMMARY:
 *
 * INPUT REQUIREMENTS:
 *
 * OUTPUT GUARANTEES:
 **===< global >===============================================================*/
STATUS_E RTC_set_time(struct tm* p_timestamp_s)
{
    STATUS_E status_e = STATUS_OK;
    UINT8 buffer[8];

    memset(buffer, 0, sizeof(buffer));

    // TODO: convert the timestamp struct to BCD values

    // TODO: sequential write to RTC
    // start
    // i2c address + write bit
    // register address (seconds)
    // write: seconds data
    // write: minutes data
    // write: hours data
    // write: days data
    // write: weekdays data
    // write: months data
    // write: years data
    // stop


    return status_e;
}

/* end */