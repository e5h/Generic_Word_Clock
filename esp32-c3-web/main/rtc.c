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
#include "i2c_manager.h"

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
 *      i2c_write() - write to one or more sequential registers on the RTC
 *
 * SUMMARY:
 *      using the i2c_manager component, this function will write to one
 *      or more sequential registers on the PCF85263A real time clock.
 *
 * INPUT REQUIREMENTS:
 *      - buffer must not be null
 *
 * OUTPUT GUARANTEES:
 *      - operation will not trigger a race condition
 *      - status of the operation will be returned
 **===< local >================================================================*/
static STATUS_E i2c_write(UINT8 reg_addr_u8, UINT8* buf_u8, UINT16 len_u16)
{
    STATUS_E status_e = STATUS_OK;

    esp_err_t err = i2c_manager_write(I2C_NUM_0, PCF85263A_ADDR_7BIT, reg_addr_u8, buf_u8, len_u16);

    if(err != ESP_OK)
    {
        status_e = STATUS_ERR;
    }

    return status_e;
}


/**===< local >================================================================
 * NAME:
 *      i2c_write() - read from one or more sequential registers on the RTC
 *
 * SUMMARY:
 *      using the i2c_manager component, this function will read from one
 *      or more sequential registers on the PCF85263A real time clock.
 *
 * INPUT REQUIREMENTS:
 *      - buffer must not be null
 *
 * OUTPUT GUARANTEES:
 *      - operation will not trigger a race condition
 *      - status of the operation will be returned
 **===< local >================================================================*/
static STATUS_E i2c_read(UINT8 reg_addr_u8, UINT8* buf_u8, INT16 len_u16)
{
    STATUS_E status_e = STATUS_OK;

    esp_err_t err = i2c_manager_read(I2C_NUM_0, PCF85263A_ADDR_7BIT, reg_addr_u8, buf_u8, len_u16);

    if(err != ESP_OK)
    {
        status_e = STATUS_ERR;
    }

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