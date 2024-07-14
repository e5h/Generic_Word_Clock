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
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"

/*=============================================================================*/
/*][ LOCAL : Constants and Types ][============================================*/
/*=============================================================================*/

#define LOG_TAG "rtc.c"     // Tag for optional ESP_LOGx calls

typedef struct{
    INT32 i2c_port_e;
    UINT8 address_u8;
} RTC_T;

/*=============================================================================*/
/*][ LOCAL : Variables ][======================================================*/
/*=============================================================================*/

volatile bool rtc_initialized_b = FALSE;
static RTC_T rtc_s;

static i2c_master_bus_handle_t i2c_bus_handle_s;
static i2c_master_dev_handle_t i2c_rtc_handle_s;

/*=============================================================================*/
/*][ LOCAL : Function Definitions ][===========================================*/
/*=============================================================================*/

/**===< local >================================================================
 * NAME:
 *      i2c_write() - write to one or more sequential registers on the RTC
 *
 * SUMMARY:
 *      using the i2c master configuration, this function will write to one
 *      or more sequential registers on the PCF85263A real time clock.
 *
 * INPUT REQUIREMENTS:
 *      - first byte in the buffer must be the register address
 *      - buffer must not be null
 *
 * OUTPUT GUARANTEES:
 *      - operation will not trigger a race condition
 *      - status of the operation will be returned (0 = fail, 1 = success)
 **===< local >================================================================*/
static STATUS_E i2c_write(UINT8* buf_u8, size_t len)
{
    STATUS_E status_e = STATUS_OK;

    if(ESP_OK != i2c_master_transmit(i2c_rtc_handle_s, buf_u8, len, -1))
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
 *      using the i2c master configuration, this function will read from one
 *      or more sequential registers on the PCF85263A real time clock.
 *
 * INPUT REQUIREMENTS:
 *      - buffer must not be null
 *
 * OUTPUT GUARANTEES:
 *      - operation will not trigger a race condition
 *      - status of the operation will be returned (0 = fail, 1 = success)
 **===< local >================================================================*/
static STATUS_E i2c_read(UINT8 reg_addr_u8, UINT8* buf_u8, size_t len)
{
    STATUS_E status_e = STATUS_OK;

    UINT8 write_buf[1] = {reg_addr_u8};

    if(ESP_OK != i2c_master_transmit_receive(i2c_rtc_handle_s, write_buf, 1, buf_u8, len, -1))
    {
        return STATUS_ERR;
    }

    return status_e;
}

/**===< local >================================================================
 * NAME:
 *      bcd_to_dec() - convert a bcd value to decimal
 *
 * SUMMARY:
 *      ---
 *
 * INPUT REQUIREMENTS:
 *      value must be <= 99
 *
 * OUTPUT GUARANTEES:
 *      will return a status enum (0 = fail, 1 = success)
 **===< local >================================================================*/
static STATUS_E bcd_to_dec(UINT8* bcd_u8, UINT8* dec_u8)
{
     STATUS_E status_e = STATUS_OK;

     // Bound check, must be a valid BCD value less than or equal to 99.
     if((*bcd_u8 & 0x0F) > 0x09 || (*bcd_u8 & 0xF0) > 0x90)
     {
         return STATUS_ERR;
     }

     *dec_u8 = (*bcd_u8 & 0x0F) + ((*bcd_u8 & 0xF0) >> 4) * 10;

     return status_e;
}

/**===< local >================================================================
 * NAME:
 *      dec_to_bcd() - convert a decimal value to bcd
 *
 * SUMMARY:
 *      ---
 *
 * INPUT REQUIREMENTS:
 *      value must be <= 99
 *
 * OUTPUT GUARANTEES:
 *      will return a status enum (0 = fail, 1 = success)
 **===< local >================================================================*/
static STATUS_E dec_to_bcd(UINT8* dec_u8, UINT8* bcd_u8)
{
    STATUS_E status_e = STATUS_OK;

    // Bound check, decimal value must be less than or equal to 99.
    if(*dec_u8 > 99)
    {
        return STATUS_ERR;
    }

    *bcd_u8 = (*dec_u8 % 10) | ((*dec_u8 / 10) << 4);

    return status_e;
}

/**===< local >================================================================
 * NAME:
 *      reg_set() - sets a single register in the RTC.
 *
 * SUMMARY:
 *      Helper function to be used during configuration. Simplifies the bitwise
 *      operations to do non-destructive writes to registers.
 *
 * INPUT REQUIREMENTS:
 *      ---
 *
 * OUTPUT GUARANTEES:
 *      Returns an enum status (0 - fail, 1 - success)
 **===< local >================================================================*/
static STATUS_E reg_set(UINT8 reg_addr_u8, UINT8 mask_u8, UINT8 bits_u8)
{
    STATUS_E status_e = STATUS_OK;

    UINT8 reg_read_value_u8[1] = {0};
    UINT8 reg_write_command_u8[2] = {reg_addr_u8, 0};

    // Read the original register value
    if(i2c_read(reg_addr_u8, reg_read_value_u8, 1) < STATUS_OK)
    {
        return STATUS_ERR;
    }

    // Keep the irrelevant bits, only write the masked bits to the value
    reg_write_command_u8[1] = reg_read_value_u8[0] & ((0xFF & ~mask_u8) | bits_u8);

    // Write the new register value
    status_e &= i2c_write(reg_write_command_u8, sizeof(reg_write_command_u8));

    return status_e;
}

/**===< global >===============================================================
 * NAME:
 *      RTC_init() - Initialize the real time clock
 *
 * SUMMARY:
 *      Sets the RTC address and the i2c port number
 *
 * INPUT REQUIREMENTS:
 *      Must be a valid address
 *
 * OUTPUT GUARANTEES:
 *      Local struct will be initialized
 *      Will return a status enum (0 = fail, 1 = success)
 **===< global >===============================================================*/
STATUS_E RTC_init(UINT8 rtc_addr_u8)
{
    STATUS_E status_e = STATUS_OK;

    // Set up the I2C port - currently only used by the RTC.
    i2c_master_bus_config_t i2c_bus_config_s = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = GPIO_NUM_1,
        .sda_io_num = GPIO_NUM_0,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = FALSE,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config_s, &i2c_bus_handle_s));

    i2c_device_config_t i2c_rtc_config_s = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = rtc_addr_u8,
            .scl_speed_hz = 400000,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle_s, &i2c_rtc_config_s, &i2c_rtc_handle_s));

    rtc_s.address_u8 = rtc_addr_u8;
    rtc_s.i2c_port_e = I2C_NUM_0;

    // TODO: Set important RTC values
    // - 24 hour mode (default)
    // - 12.5pF, 32.768kHz crystal
    status_e &= reg_set(CTRL_REG_ADDR_OSCILLATOR, CTRL_REG_OSC_M_CL, OSC_CL_12_5_PF);

    rtc_initialized_b = TRUE;

    return status_e;
}

/**===< global >===============================================================
 * NAME:
 *      RTC_get_time() - Get the current time from the RTC
 *
 * SUMMARY:
 *
 * INPUT REQUIREMENTS:
 *
 * OUTPUT GUARANTEES:
 **===< global >===============================================================*/
STATUS_E RTC_get_time(struct tm* p_timestamp_s)
{
    if(rtc_initialized_b != TRUE)
    {
        return STATUS_ERR;
    }

    STATUS_E status_e = STATUS_OK;

    UINT8 buffer_u8[7];
    UINT8 time_values_u8[7];
    memset(buffer_u8, 0, sizeof(buffer_u8));
    memset(time_values_u8, 0, sizeof(time_values_u8));

    // Sequential RTC read
    if(i2c_read(RTC_REG_ADDR_SECONDS, buffer_u8, sizeof(buffer_u8)) < STATUS_OK)
    {
        return STATUS_ERR;
    }

    ESP_LOGI("RTC_get_time", "Read RTC time registers (bcd). [yr: %02x] [mo: %02x] [wd: %02x, d: %02x] [h: %02x, m: %02x, s: %02x]",
             buffer_u8[6],
             buffer_u8[5],
             buffer_u8[4],
             buffer_u8[3],
             buffer_u8[2],
             buffer_u8[1],
             buffer_u8[0]
    );

    // buffer_u8[0] : seconds
    buffer_u8[0] &= RTC_REG_SECONDS_M_SEC;
    status_e  &= bcd_to_dec(&buffer_u8[0], &time_values_u8[0]);
    p_timestamp_s->tm_sec = time_values_u8[0];

    // buffer_u8[1] : minutes
    buffer_u8[1] &= RTC_REG_MINUTES_M_MIN;
    status_e  &= bcd_to_dec(&buffer_u8[1], &time_values_u8[1]);
    p_timestamp_s->tm_min = time_values_u8[1];

    // buffer_u8[2] : hours
    buffer_u8[2] &= RTC_REG_HOURS_M_HRS24;
    status_e  &= bcd_to_dec(&buffer_u8[2], &time_values_u8[2]);
    p_timestamp_s->tm_hour = time_values_u8[2];

    // buffer_u8[3] : days
    buffer_u8[3] &= RTC_REG_DAYS_M;
    status_e  &= bcd_to_dec(&buffer_u8[3], &time_values_u8[3]);
    p_timestamp_s->tm_mday = time_values_u8[3];

    // buffer_u8[4] : weekdays
    buffer_u8[4] &= RTC_REG_WEEKDAYS_M;
    status_e  &= bcd_to_dec(&buffer_u8[4], &time_values_u8[4]);
    p_timestamp_s->tm_wday = time_values_u8[4];

    // buffer_u8[5] : months
    buffer_u8[5] &= RTC_REG_MONTHS_M;
    status_e  &= bcd_to_dec(&buffer_u8[5], &time_values_u8[5]);
    p_timestamp_s->tm_mon = time_values_u8[5];

    // buffer_u8[6] : years
    buffer_u8[6] &= RTC_REG_YEARS_M;
    status_e  &= bcd_to_dec(&buffer_u8[6], &time_values_u8[6]);
    p_timestamp_s->tm_year = time_values_u8[6];

    ESP_LOGI("RTC_get_time", "Read RTC time registers (dec). [yr: %u] [mo: %u] [wd: %u, d: %u] [h: %u, m: %u, s: %u]",
             time_values_u8[6],
             time_values_u8[5],
             time_values_u8[4],
             time_values_u8[3],
             time_values_u8[2],
             time_values_u8[1],
             time_values_u8[0]
    );

    // Any failed conversion will have set this to 0 (STATUS_ERR) if an error occurred.
    return status_e;
}

/**===< global >===============================================================
 * NAME:
 *      RTC_set_time() - Set the timestamp on the RTC
 *
 * SUMMARY:
 *
 * INPUT REQUIREMENTS:
 *
 * OUTPUT GUARANTEES:
 **===< global >===============================================================*/
STATUS_E RTC_set_time(struct tm* p_timestamp_s)
{
    if(rtc_initialized_b != TRUE)
    {
        return STATUS_ERR;
    }

    STATUS_E status_e = STATUS_OK;

    UINT8 buffer_u8[8];
    UINT8 time_values_u8[7];
    memset(buffer_u8, 0, sizeof(buffer_u8));
    memset(time_values_u8, 0, sizeof(time_values_u8));

    // buffer_u8[0] : register address

    // buffer_u8[1] : seconds
    time_values_u8[0] = (UINT8)p_timestamp_s->tm_sec;

    // buffer_u8[2] : minutes
    time_values_u8[1] = (UINT8)p_timestamp_s->tm_min;

    // buffer_u8[3] : hours
    time_values_u8[2] = (UINT8)p_timestamp_s->tm_hour;

    // buffer_u8[4] : days
    time_values_u8[3] = (UINT8)p_timestamp_s->tm_mday;

    // buffer_u8[5] : weekdays
    time_values_u8[4] = (UINT8)p_timestamp_s->tm_wday;

    // buffer_u8[6] : months
    time_values_u8[5] = (UINT8)p_timestamp_s->tm_mon;

    // buffer_u8[7] : years
    time_values_u8[6] = (UINT8)p_timestamp_s->tm_year;

    ESP_LOGI("RTC_set_time", "Setting RTC time registers (dec). [yr: %u] [mo: %u] [wd: %u, d: %u] [h: %u, m: %u, s: %u]",
             time_values_u8[6],
             time_values_u8[5],
             time_values_u8[4],
             time_values_u8[3],
             time_values_u8[2],
             time_values_u8[1],
             time_values_u8[0]
             );

    // Copy everything to the actual write buffer. Offset by 1 due to inclusion of register address.
    buffer_u8[0] = RTC_REG_ADDR_SECONDS;
    for(INT32 i = 1; i < sizeof(buffer_u8); i++)
    {
        status_e &= dec_to_bcd(&time_values_u8[i - 1], &buffer_u8[i]);
    }

    if(status_e >= STATUS_OK)
    {
        status_e = i2c_write(buffer_u8, sizeof(buffer_u8));
    }

    ESP_LOGI("RTC_set_time", "Setting RTC time registers (bcd). [yr: %02x] [mo: %02x] [wd: %02x, d: %02x] [h: %02x, m: %02x, s: %02x]",
             buffer_u8[7],
             buffer_u8[6],
             buffer_u8[5],
             buffer_u8[4],
             buffer_u8[3],
             buffer_u8[2],
             buffer_u8[1]
    );

    return status_e;
}

/* end */