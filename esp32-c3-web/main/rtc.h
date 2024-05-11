/*==============================================================================
 *==============================================================================
 * NAME:
 *      rtc.h
 *
 * PURPOSE:
 *      This module encapsulates the real time clock.
 *
 *      The real time clock used in this module is a PCF85263A manufactured
 *      by NXP.
 *
 * DEPENDENCIES:
 *      ---
 *
 *==============================================================================
 * (C) Andrew Bright 2024, github.com/e5h
 *==============================================================================
 *=============================================================================*/

#ifndef WC_RTC_H

/*=============================================================================*/
/*][ Include Files ][==========================================================*/
/*=============================================================================*/

#include "lib_includes.h"

/*=============================================================================*/
/*][ GLOBAL : Constants and Types ][===========================================*/
/*=============================================================================*/

/* PCF85263A VALUES */
#define PCF85263A_ADDR_7BIT             (0x51u) // Default address (1010_001x), MSB = b6
#define PCF85263A_ADDR_8BIT             (0xA2u) // Default address (1010_001x), MSB = b7

/* REGISTER DEFINITIONS */
/* RTC time and date */
#define RTC_REG_ADDR_100TH_SECONDS      (0x00u)
#define RTC_REG_100TH_SECONDS_M         (0xFFu) // Mask - 100th second (bcd 0-99) [b7:0]

#define RTC_REG_ADDR_SECONDS            (0x01u)
#define RTC_REG_SECONDS_M_SEC           (0x7Fu) // Mask - second (bcd 0-59) [b6:0]
#define RTC_REG_SECONDS_M_OS            (0x80u) // Mask - oscillator stop [b7]

#define RTC_REG_ADDR_MINUTES            (0x02u)
#define RTC_REG_MINUTES_M_MIN           (0x7Fu) // Mask - minute (bcd 0-59) [b6:0]
#define RTC_REG_MINUTES_M_EMON          (0x80u) // Mask - emon [b7]

#define RTC_REG_ADDR_HOURS              (0x03u)
#define RTC_REG_HOURS_M_HR12            (0x1Fu) // Mask - hour, 12h (bcd 1-12) [b4:0]
#define RTC_REG_HOURS_M_AMPM            (0x20u) // Mask - am/pm, 12h [b5]
#define RTC_REG_HOURS_M_HRS24           (0x3Fu) // Mask - hour, 24h (bcd 0-23) [b5:0]

#define RTC_REG_ADDR_DAYS               (0x04u)
#define RTC_REG_DAYS_M                  (0x3Fu) // Mask - day (bcd 1-31) [b5:0]

#define RTC_REG_ADDR_WEEKDAYS           (0x05u)
#define RTC_REG_WEEKDAYS_M              (0x07u) // Mask - weekday (bcd 0-6) [b2:0]

#define RTC_REG_ADDR_MONTHS             (0x06u)
#define RTC_REG_MONTHS_M                (0x1Fu) // Mask - month (bcd 1-12) [b4:0]

#define RTC_REG_ADDR_YEARS              (0x07u)
#define RTC_REG_YEARS_M                 (0xFFu) // Mask - year (bcd 0-99) [b7:0]

/* RTC alarm 1 */
#define RTC_REG_ADDR_SECOND_ALARM1      (0x08u)
#define RTC_REG_SECOND_ALARM1_M         (0x7Fu) // Mask - second (bcd 0-59) [b6:0]

#define RTC_REG_ADDR_MINUTE_ALARM1      (0x09u)
#define RTC_REG_MINUTE_ALARM1_M         (0x7Fu) // Mask - minute (bcd 0-59) [b6:0]

#define RTC_REG_ADDR_HOUR_ALARM1        (0x0Au)
#define RTC_REG_HOUR_ALARM1_M_HR12      (0x1Fu) // Mask - hour, 12h (bcd 1-12) [b4:0]
#define RTC_REG_HOUR_ALARM1_M_AMPM      (0x20u) // Mask - am/pm, 12h [b5]
#define RTC_REG_HOUR_ALARM1_M_HR24      (0x3Fu) // Mask - hour, 24h (bcd 0-23) [b5:0]

#define RTC_REG_ADDR_DAY_ALARM1         (0x0Bu)
#define RTC_REG_DAY_ALARM1_M            (0x3Fu) // Mask - day (bcd 1-31) [b5:0]

#define RTC_REG_ADDR_MONTH_ALARM1       (0x0Cu)
#define RTC_REG_MONTH_ALARM1_M          (0x1Fu) // Mask - month (bcd 1-12) [b4:0]

/* RTC alarm 2 */
#define RTC_REG_ADDR_MINUTE_ALARM2      (0x0Du)
#define RTC_REG_MINUTE_ALARM2_M         (0x7Fu) // Mask - minute (bcd 0-59) [b6:0]

#define RTC_REG_ADDR_HOUR_ALARM2        (0x0Eu)
#define RTC_REG_HOUR_ALARM2_M_HR12      (0x1Fu) // Mask - hour, 12h (bcd 1-12) [b4:0]
#define RTC_REG_HOUR_ALARM2_M_AMPM      (0x20u) // Mask - am/pm, 12h [b5]
#define RTC_REG_HOUR_ALARM2_M_HR24      (0x3Fu) // Mask - hour, 24h (bcd 0-23) [b5:0]

#define RTC_REG_ADDR_WEEKDAY_ALARM2     (0x0Fu)
#define RTC_REG_WEEKDAY_ALARM2_M        (0x07u) // Mask - weekday (bcd 0-6) [b2:0]

/* RTC alarm enables */
#define RTC_REG_ADDR_ALARM_ENABLES      (0x10u)
#define RTC_REG_ALARM_EN_M_SEC_A1E      (0x01u) // Mask - second alarm1 enable [b0]
#define RTC_REG_ALARM_EN_M_MIN_A1E      (0x02u) // Mask - minute alarm1 enable [b1]
#define RTC_REG_ALARM_EN_M_HR_A1E       (0x04u) // Mask - hour alarm1 enable [b2]
#define RTC_REG_ALARM_EN_M_DAY_A1E      (0x08u) // Mask - day alarm1 enable [b3]
#define RTC_REG_ALARM_EN_M_MON_A1E      (0x10u) // Mask - month alarm1 enable [b4]
#define RTC_REG_ALARM_EN_M_MIN_A2E      (0x20u) // Mask - minute alarm2 enable [b5]
#define RTC_REG_ALARM_EN_M_HR_A2E       (0x40u) // Mask - hour alarm2 enable [b6]
#define RTC_REG_ALARM_EN_M_WDAY_A2E     (0x80u) // Mask - weekday alarm2 enable [b7]

/* RTC timestamp 1 */
#define RTC_REG_ADDR_TSR1_SECONDS       (0x11u)
#define RTC_REG_TSR1_SECONDS_M_SEC      (0x7Fu) // Mask - second (bcd 0-59) [b6:0]

#define RTC_REG_ADDR_TSR1_MINUTES       (0x12u)
#define RTC_REG_TSR1_MINUTES_M_MIN      (0x7Fu) // Mask - minute (bcd 0-59) [b6:0]

#define RTC_REG_ADDR_TSR1_HOURS         (0x13u)
#define RTC_REG_TSR1_HOURS_M_HR12       (0x1Fu) // Mask - hour, 12h (bcd 1-12) [b4:0]
#define RTC_REG_TSR1_HOURS_M_AMPM       (0x20u) // Mask - am/pm, 12h [b5]
#define RTC_REG_TSR1_HOURS_M_HR24       (0x3Fu) // Mask - hour, 24h (bcd 0-23) [b5:0]

#define RTC_REG_ADDR_TSR1_DAYS          (0x14u)
#define RTC_REG_TSR1_DAYS_M             (0x3Fu) // Mask - day (bcd 1-31) [b5:0]

#define RTC_REG_ADDR_TSR1_MONTHS        (0x15u)
#define RTC_REG_TSR1_MONTHS_M           (0x1Fu) // Mask - month (bcd 1-12) [b4:0]

#define RTC_REG_ADDR_TSR1_YEARS         (0x16u)
#define RTC_REG_TSR1_YEARS_M            (0xFFu) // Mask - year (bcd 0-99) [b7:0]

/* RTC timestamp 2 */
#define RTC_REG_ADDR_TSR2_SECONDS       (0x17u)
#define RTC_REG_TSR2_SECONDS_M_SEC      (0x7Fu) // Mask - second (bcd 0-59) [b6:0]

#define RTC_REG_ADDR_TSR2_MINUTES       (0x18u)
#define RTC_REG_TSR2_MINUTES_M_MIN      (0x7Fu) // Mask - minute (bcd 0-59) [b6:0]

#define RTC_REG_ADDR_TSR2_HOURS         (0x19u)
#define RTC_REG_TSR2_HOURS_M_HR12       (0x1Fu) // Mask - hour, 12h (bcd 1-12) [b4:0]
#define RTC_REG_TSR2_HOURS_M_AMPM       (0x20u) // Mask - am/pm, 12h [b5]
#define RTC_REG_TSR2_HOURS_M_HR24       (0x3Fu) // Mask - hour, 24h (bcd 0-23) [b5:0]

#define RTC_REG_ADDR_TSR2_DAYS          (0x1Au)
#define RTC_REG_TSR2_DAYS_M             (0x3Fu) // Mask - day (bcd 1-31) [b5:0]

#define RTC_REG_ADDR_TSR2_MONTHS        (0x1Bu)
#define RTC_REG_TSR2_MONTHS_M           (0x1Fu) // Mask - month (bcd 1-12) [b4:0]

#define RTC_REG_ADDR_TSR2_YEARS         (0x1Cu)
#define RTC_REG_TSR2_YEARS_M            (0xFFu) // Mask - year (bcd 0-99) [b7:0]

/* RTC timestamp 3 */
#define RTC_REG_ADDR_TSR3_SECONDS       (0x1Du)
#define RTC_REG_TSR3_SECONDS_M_SEC      (0x7Fu) // Mask - second (bcd 0-59) [b6:0]

#define RTC_REG_ADDR_TSR3_MINUTES       (0x1Eu)
#define RTC_REG_TSR3_MINUTES_M_MIN      (0x7Fu) // Mask - minute (bcd 0-59) [b6:0]

#define RTC_REG_ADDR_TSR3_HOURS         (0x1Fu)
#define RTC_REG_TSR3_HOURS_M_HR12       (0x1Fu) // Mask - hour, 12h (bcd 1-12) [b4:0]
#define RTC_REG_TSR3_HOURS_M_AMPM       (0x20u) // Mask - am/pm, 12h [b5]
#define RTC_REG_TSR3_HOURS_M_HR24       (0x3Fu) // Mask - hour, 24h (bcd 0-23) [b5:0]

#define RTC_REG_ADDR_TSR3_DAYS          (0x20u)
#define RTC_REG_TSR3_DAYS_M             (0x3Fu) // Mask - day (bcd 1-31) [b5:0]

#define RTC_REG_ADDR_TSR3_MONTHS        (0x21u)
#define RTC_REG_TSR3_MONTHS_M           (0x1Fu) // Mask - month (bcd 1-12) [b4:0]

#define RTC_REG_ADDR_TSR3_YEARS         (0x22u)
#define RTC_REG_TSR3_YEARS_M            (0xFFu) // Mask - year (bcd 0-99) [b7:0]

/* RTC timestamp mode control */
#define RTC_REG_ADDR_TSR_MODE           (0x23u)

#define TSR_MODE_M_TSR1                 (0x03u) // Mask - timestamp 1 [b1:0]
#define TSR1_SHIFT                      (0)     // TSR1 shift
#define TSR1_MODE_NT                    (0b00 << TSR1_SHIFT) // no timestamp
#define TSR1_MODE_FE                    (0b01 << TSR1_SHIFT) // record first TS pin event
#define TSR1_MODE_LE                    (0b10 << TSR1_SHIFT) // record last TS pin event

#define TSR_MODE_M_TSR2                 (0x1Cu) // Mask - timestamp 2 [b4:2]
#define TSR2_SHIFT                      (2)     // TSR2 shift
#define TSR2_MODE_NT                    (0b000 << TSR2_SHIFT) // no timestamp
#define TSR2_MODE_FB                    (0b001 << TSR2_SHIFT) // record first time switch to battery event
#define TSR2_MODE_LB                    (0b010 << TSR2_SHIFT) // record last time switch to battery event
#define TSR2_MODE_LV                    (0b011 << TSR2_SHIFT) // record last time switch to VDD event
#define TSR2_MODE_FE                    (0b100 << TSR2_SHIFT) // record first TS pin event
#define TSR2_MODE_LE                    (0b101 << TS02_SHIFT) // record last TS pin event

#define TSR_MODE_M_TSR3                 (0xC0u) // Mask - timestamp 3 [b7:6]
#define TSR3_SHIFT                      (6)     // TSR3 shift
#define TSR3_MODE_NT                    (0b00 << TSR3_SHIFT) // no timestamp
#define TSR3_MODE_FB                    (0b01 << TSR3_SHIFT) // record first time switch to battery event
#define TSR3_MODE_LB                    (0b10 << TSR3_SHIFT) // record last time switch to battery event
#define TSR3_MODE_LV                    (0b11 << TSR3_SHIFT) // record last time switch to VDD event

/* Stopwatch time registers */
#define SW_REG_ADDR_100TH_SECONDS       (0x00u)
#define SW_REG_100TH_SECONDS_M          (0xFFu) // Mask - 100th second (bcd 0-99) [b7:0]

#define SW_REG_ADDR_SECONDS             (0x01u)
#define SW_REG_SECONDS_M_SEC            (0x7Fu) // Mask - second (bcd 0-59) [b6:0]
#define SW_REG_SECONDS_M_OS             (0x80u) // Mask - oscillator stop [b7]

#define SW_REG_ADDR_MINUTES             (0x02u)
#define SW_REG_MINUTES_M_MIN            (0x7Fu) // Mask - minute (bcd 0-59) [b6:0]
#define SW_REG_MINUTES_M_EMON           (0x80u) // Mask - emon [b7]

#define SW_REG_ADDR_HOURS_XX_XX_00      (0x03u)
#define SW_REG_HOURS_XX_XX_00_M_HRS     (0xFFu) // Mask - tens hours (bcd 0-99) [b7:0]

#define SW_REG_ADDR_HOURS_XX_00_XX      (0x04u)
#define SW_REG_HOURS_XX_00_XX_M_HRS     (0xFFu) // Mask - thousands hours (bcd 0-99) [b7:0]

#define SW_REG_ADDR_HOURS_00_XX_XX      (0x05u)
#define SW_REG_HOURS_00_XX_XX_M_HRS     (0xFFu) // Mask - 100s thousands hours (bcd 0-99) [b7:0]

/* Stopwatch alarm 1 */
#define SW_REG_ADDR_SECOND_ALM1         (0x08u)
#define SW_REG_SECOND_ALM1_M            (0x7Fu) // Mask - second (bcd 0-59) [b6:0]

#define SW_REG_ADDR_MINUTE_ALM1         (0x09u)
#define SW_REG_MINUTE_ALM1_M            (0x7Fu) // Mask - minute (bcd 0-59) [b6:0]

#define SW_REG_ADDR_HR_XX_XX_00_ALM1    (0x0Au)
#define SW_REG_HR_XX_XX_00_ALM1_M       (0xFFu) // Mask - tens hours (bcd 0-99) [b7:0]

#define SW_REG_ADDR_HR_XX_00_XX_ALM1    (0x0Bu)
#define SW_REG_HR_XX_00_XX_ALM1_M       (0xFFu) // Mask - thousands hours (bcd 0-99) [b7:0]

#define SW_REG_ADDR_HR_00_XX_XX_ALM1    (0x0Cu)
#define SW_REG_HR_00_XX_XX_ALM1_M       (0xFFu) // Mask - 100s thousands hours (bcd 0-99) [b7:0]

/* Stopwatch alarm 2 */
#define SW_REG_ADDR_MINUTE_ALM2         (0x0Du)
#define SW_REG_MINUTE_ALM2_M            (0x7Fu) // Mask - minute (bcd 0-59) [b6:0]

#define SW_REG_ADDR_HR_XX_00_ALM2       (0x0Eu)
#define SW_REG_HR_XX_00_ALM2_M          (0xFFu) // Mask - tens hours (bcd 0-99) [b7:0]

#define SW_REG_ADDR_HR_00_XX_ALM2       (0x0Fu)
#define SW_REG_HR_00_XX_ALM2_M          (0xFFu) // Mask - thousands hours (bcd 0-99) [b7:0]

/* Stopwatch alarm enables */
#define SW_REG_ADDR_ALARM_ENABLES       (0x10u)
#define RTC_REG_ALARM_EN_M_SEC_A1E      (0x01u) // Mask - second alarm1 enable [b0]
#define RTC_REG_ALARM_EN_M_MIN_A1E      (0x02u) // Mask - minute alarm1 enable [b1]
#define RTC_REG_ALARM_EN_M_H_XX0_A1E    (0x04u) // Mask - tens hours alarm1 enable [b2]
#define RTC_REG_ALARM_EN_M_H_X0X_A1E    (0x08u) // Mask - thousands hours alarm1 enable [b3]
#define RTC_REG_ALARM_EN_M_H_0XX_A1E    (0x10u) // Mask - 100 thousands hours alarm1 enable [b4]
#define RTC_REG_ALARM_EN_M_MIN_A2E      (0x20u) // Mask - minute alarm2 enable [b5]
#define RTC_REG_ALARM_EN_M_H_X0_A2E     (0x40u) // Mask - tens hours alarm2 enable [b6]
#define RTC_REG_ALARM_EN_M_H_0X_A2E     (0x80u) // Mask - thousands hours alarm2 enable [b7]

/* Stopwatch timestamp 1 */
#define SW_REG_ADDR_TSR1_SECONDS        (0x11u)
#define SW_REG_TSR1_SECONDS_M           (0x7Fu) // Mask - second (bcd 0-59) [b6:0]

#define SW_REG_ADDR_TSR1_MINUTES        (0x12u)
#define SW_REG_TSR1_MINUTES_M           (0x7Fu) // Mask - minute (bcd 0-59) [b6:0]

#define SW_REG_ADDR_TSR1_HR_XX_XX_00    (0x13u)
#define SW_REG_TSR1_HR_XX_XX_00_M       (0xFFu) // Mask - tens hours (bcd 0-99) [b7:0]

#define SW_REG_ADDR_TSR1_HR_XX_00_XX    (0x14u)
#define SW_REG_TSR1_HR_XX_00_XX_M       (0xFFu) // Mask - thousands hours (bcd 0-99) [b7:0]

#define SW_REG_ADDR_TSR1_HR_00_XX_XX    (0x15u)
#define SW_REG_TSR1_HR_00_XX_XX_M       (0xFFu) // Mask - 100s thousands hours (bcd 0-99) [b7:0]

/* Stopwatch timestamp 2 */
#define SW_REG_ADDR_TSR2_SECONDS        (0x17u)
#define SW_REG_TSR2_SECONDS_M           (0x7Fu) // Mask - second (bcd 0-59) [b6:0]

#define SW_REG_ADDR_TSR2_MINUTES        (0x18u)
#define SW_REG_TSR2_MINUTES_M           (0x7Fu) // Mask - minute (bcd 0-59) [b6:0]

#define SW_REG_ADDR_TSR2_HR_XX_XX_00    (0x19u)
#define SW_REG_TSR2_HR_XX_XX_00_M       (0xFFu) // Mask - tens hours (bcd 0-99) [b7:0]

#define SW_REG_ADDR_TSR2_HR_XX_00_XX    (0x1Au)
#define SW_REG_TSR2_HR_XX_00_XX_M       (0xFFu) // Mask - thousands hours (bcd 0-99) [b7:0]

#define SW_REG_ADDR_TSR2_HR_00_XX_XX    (0x1Bu)
#define SW_REG_TSR2_HR_00_XX_XX_M       (0xFFu) // Mask - 100s thousands hours (bcd 0-99) [b7:0]

/* Stopwatch timestamp 3 */
#define SW_REG_ADDR_TSR3_SECONDS        (0x1Du)
#define SW_REG_TSR3_SECONDS_M           (0x7Fu) // Mask - second (bcd 0-59) [b6:0]

#define SW_REG_ADDR_TSR3_MINUTES        (0x1Eu)
#define SW_REG_TSR3_MINUTES_M           (0x7Fu) // Mask - minute (bcd 0-59) [b6:0]

#define SW_REG_ADDR_TSR3_HR_XX_XX_00    (0x1Fu)
#define SW_REG_TSR3_HR_XX_XX_00_M       (0xFFu) // Mask - tens hours (bcd 0-99) [b7:0]

#define SW_REG_ADDR_TSR3_HR_XX_00_XX    (0x20u)
#define SW_REG_TSR3_HR_XX_00_XX_M       (0xFFu) // Mask - thousands hours (bcd 0-99) [b7:0]

#define SW_REG_ADDR_TSR3_HR_00_XX_XX    (0x21u)
#define SW_REG_TSR3_HR_00_XX_XX_M       (0xFFu) // Mask - 100s thousands hours (bcd 0-99) [b7:0]

/* Stopwatch timestamp mode control */
#define SW_REG_ADDR_TSR_MODE            (0x23u) // Reuses the timestamp controls from RTC mode

/* Offset */
#define CTRL_REG_ADDR_OFFSET            (0x24u)
#define CTRL_REG_OFFSET_M               (0xFFu) // Mask - offset value [b7:0]

/* Control registers */
/*===========================================*/
#define CTRL_REG_ADDR_OSCILLATOR        (0x25u)

#define CTRL_REG_OSC_M_CL               (0x03u) // Mask - quartz oscillator capacitance [b1:0]
#define OSC_CL_SHIFT                    (0)     // Shift - quartz oscillator capacitance
#define OSC_CL_7_0_PF                   (0b00 << OSC_CL_SHIFT) // 7.5 pF crystal
#define OSC_CL_6_0_PF                   (0b01 << OSC_CL_SHIFT) // 6.0 pF crystal
#define OSC_CL_12_5_PF                  (0b10 << OSC_CL_SHIFT) // 12.5 pF crystal
#define OSC_CL_12_5_PF                  (0b11 << OSC_CL_SHIFT) // 12.5 pF crystal

#define CTRL_REG_OSC_M_OSCD             (0x0Cu) // Mask - oscillator control [b3:2]
#define OSC_OSCD_SHIFT                  (2)     // Shift - oscillator control
#define OSC_OSCD_DRIVE_N                (0b00 << OSC_OSCD_SHIFT) // normal drive, Rs(max) = 100 kR
#define OSC_OSCD_DRIVE_L                (0b01 << OSC_OSCD_SHIFT) // low drive, Rs(max) = 60 kR and IDD-
#define OSC_OSCD_DRIVE_H                (0b10 << OSC_OSCD_SHIFT) // high drive, Rs(max) = 500 kR and IDD+

#define CTRL_REG_OSC_M_LOWJ             (0x10u) // Mask - low jitter mode [b4] (0 - normal, 1 - reduced CLK jitter and IDD+)
#define CTRL_REG_OSC_M_12_24            (0x20u) // Mask - time format [b5] (0 - 24hr, 1 - 12hr)
#define CTRL_REG_OSC_M_OFFM             (0x40u) // Mask - offset mode [b6] (0 - every 4 hr, 1 - every 8 min)
#define CTRL_REG_OSC_M_CLKIV            (0x80u) // Mask - clock inversion [b7] (0 - non inverted, 1 - inverted)

/*===[control registers]=====================*/
#define CTRL_REG_ADDR_BATTERY_SWITCH    (0x26u)

#define CTRL_REG_BATT_M_BSM             (0x06u) // Mask - battery switch mode [b2:1]
#define BATT_BSM_SHIFT                  (1)     // Shift - battery switch mode
#define BATT_BSM_SW_VTH                 (0b00 << BATT_BSM_SHIFT) // Use Vdd when > Vth, Vbat otherwise
#define BATT_BSM_SW_VBAT                (0b01 << BATT_BSM_SHIFT) // Use Vdd when > Vbat, Vbat otherwise
#define BATT_BSM_SW_HI_EITHER           (0b10 << BATT_BSM_SHIFT) // Use Vdd when > max(Vth, Vbat), Vbat otherwise
#define BATT_BSM_SW_LO_EITHER           (0b11 << BATT_BSM_SHIFT) // Use Vdd when > min(Vth, Vbat), Vbat otherwise

#define CTRL_REG_BATT_M_BSTH            (0x01u) // Mask - threshold voltage control [b0] (0 - Vth=1.5v, 1 - Vth=2.8v)
#define CTRL_REG_BATT_M_BSRR            (0x08u) // Mask - battery switch internal refresh rate [b3] (0 - low, 1 - high)
#define CTRL_REG_BATT_H_BSOFF           (0x10u) // Mask - battery switch on/off [b4] (0 - enable, 1 - disable)

/*===[control registers]=====================*/
#define CTRL_REG_ADDR_PIN_IO            (0x27u)

#define CTRL_REG_PIN_IO_M_INTAPM        (0x03u) // Mask - ~INTA pin mode control [b1:0]
#define PIN_IO_INTAPM_SHIFT             (0)     // Shift - ~INTA pin mode control
#define PIN_IO_INTAPM_CLK_OUT           (0b00 << PIN_IO_INTAPM_SHIFT) // CLK output mode
#define PIN_IO_INTAPM_BATT_IND          (0b01 << PIN_IO_INTAPM_SHIFT) // Battery mode indication
#define PIN_IO_INTAPM_N_INTA_OUT        (0b10 << PIN_IO_INTAPM_SHIFT) // ~INTA output
#define PIN_IO_INTAPM_HIZ               (0b11 << PIN_IO_INTAPM_SHIFT) // Hi-Z

#define CTRL_REG_PIN_IO_M_TSPM          (0x0Cu) // Mask - TS pin IO control [b3:2]
#define PIN_IO_TSPM_SHIFT               (2)     // Shift - TS pin IO control
#define PIN_IO_TSPM_DISABLED            (0b00 << PIN_IO_TSPM_SHIFT) // Disabled, can leave floating
#define PIN_IO_TSPM_N_INTB_PP           (0b01 << PIN_IO_TSPM_SHIFT) // ~INTB output, push-pull (only on Vdd, Vbat = HiZ)
#define PIN_IO_TSPM_CLK_PP              (0b10 << PIN_IO_TSPM_SHIFT) // CLK output, push-pull (only on Vdd, Vbat = HiZ)
#define PIN_IO_TSPM_INPUT               (0b11 << PIN_IO_TSPM_SHIFT) // Input mode

#define CTRL_REG_PIN_IO_M_TSIM          (0x10u) // Mask - TS pin input type [b4] (0 - CMOS [refs Vdd, disabled on Vbat], 1 - mechanical [active pullup sampled 16Hz, operates on both])
#define CTRL_REG_PIN_IO_M_TSL           (0x20u) // Mask - TS pin level sense [b5] (0 - active HI, 1 - active LO)
#define CTRL_REG_PIN_IO_M_TSPULL        (0x40u) // Mask - TS pin pull-up R value [b6] (0 - 80kR, 1 - 40kR)
#define CTRL_REG_PIN_IO_M_CLKPM         (0x80u) // Mask - clock pin mode control [b7] (0 - enable, 1 - disable)

/*===[control registers]=====================*/
#define CTRL_REG_ADDR_FUNCTION          (0x28u)

#define CTRL_REG_FUNC_M_COF             (0x07u) // Mask - clock output frequency [b2:0]
#define FUNC_COF_SHIFT                  (0)     // Shift - clock output frequency
#define FUNC_COF_32KHZ                  (0b000 << FUNC_COF_SHIFT) // CLK, TS, ~INTA = 32,768 Hz
#define FUNC_COF_16KHZ                  (0b001 << FUNC_COF_SHIFT) // CLK, TS, ~INTA = 16,384 Hz
#define FUNC_COF_8KHZ                   (0b010 << FUNC_COF_SHIFT) // CLK, TS, ~INTA = 8,192 Hz
#define FUNC_COF_4KHZ                   (0b011 << FUNC_COF_SHIFT) // CLK, TS, ~INTA = 4,096 Hz
#define FUNC_COF_2KHZ                   (0b100 << FUNC_COF_SHIFT) // CLK, TS, ~INTA = 2,048 Hz
#define FUNC_COF_1KHZ                   (0b101 << FUNC_COF_SHIFT) // CLK, TS, ~INTA = 1,024 Hz
#define FUNC_COF_1HZ                    (0b110 << FUNC_COF_SHIFT) // CLK, TS, ~INTA = 1 Hz
#define FUNC_COF_DISABLE                (0b111 << FUNC_COF_SHIFT) // CLK, TS = LOW, ~INTA = Hi-Z

#define CTRL_REG_FUNC_M_STOPM           (0x80u) // Mask - RTC STOP mode control [b3] (0 - STOP bit only, 1 - STOP bit or TS pin)
#define CTRL_REG_FUNC_M_RTCM            (0x10u) // Mask - RTC mode [b4] (0 - real time mode, 1 - stopwatch mode)

#define CTRL_REG_FUNC_M_PI              (0x60u) // Mask - periodic interrupt [b6:5]
#define FUNC_PI_SHIFT                   (5)     // Shift - periodic interrupt
#define FUNC_PI_NONE                    (0b00 << FUNC_PI_SHIFT) // No periodic interrupt
#define FUNC_PI_SECOND                  (0b01 << FUNC_PI_SHIFT) // Onc interrupt pulse per second
#define FUNC_PI_MINUTE                  (0b10 << FUNC_PI_SHIFT) // One interrupt pulse per minute
#define FUNC_PI_HOUR                    (0b11 << FUNC_PI_SHIFT) // One interrupt pulse per hour

#define CTRL_REG_FUNC_M_1OOTH           (0x80u) // Mask - 100th seconds mode [b7] (0 - disabled, 1 - enabled)

/*===[control registers]=====================*/
#define CTRL_REG_ADDR_INTA_ENABLE       (0x29u)

#define CTRL_REG_INTA_ENABLE_M_WDIEA    (0x01u) // Mask - watchdog interrupt enable [b0] (0 - disabled, 1 - enabled)
#define CTRL_REG_INTA_ENABLE_M_BSIEA    (0x02u) // Mask - battery switch interrupt enable [b1] (0 - disabled, 1 - enabled)
#define CTRL_REG_INTA_ENABLE_M_TSRIEA   (0x04u) // Mask - timestamp register interrupt enable [b2] (0 - disabled, 1 - enabled)
#define CTRL_REG_INTA_ENABLE_M_A2IEA    (0x08u) // Mask - alarm2 interrupt enable [b3] (0 - disabled, 1 - enabled)
#define CTRL_REG_INTA_ENABLE_M_A1IEA    (0x10u) // Mask - alarm1 interrupt enable [b4] (0 - disabled, 1 - enabled)
#define CTRL_REG_INTA_ENABLE_M_OIEA     (0x20u) // Mask - offset correction interrupt enable [b5] (0 - disabled, 1 - enabled)
#define CTRL_REG_INTA_ENABLE_M_PIEA     (0x40u) // Mask - periodic interrupt enable [b6] (0 - disabled, 1 - enabled)
#define CTRL_REG_INTA_ENABLE_M_ILPA     (0x80u) // Mask - level or pulse mode [b7] (0 - pulse, 1 - follow flags [permanent level])

/*===[control registers]=====================*/
#define CTRL_REG_ADDR_INTB_ENABLE       (0x2Au)

#define CTRL_REG_INTA_ENABLE_M_WDIEB    (0x01u) // Mask - watchdog interrupt enable [b0] (0 - disabled, 1 - enabled)
#define CTRL_REG_INTA_ENABLE_M_BSIEB    (0x02u) // Mask - battery switch interrupt enable [b1] (0 - disabled, 1 - enabled)
#define CTRL_REG_INTA_ENABLE_M_TSRIEB   (0x04u) // Mask - timestamp register interrupt enable [b2] (0 - disabled, 1 - enabled)
#define CTRL_REG_INTA_ENABLE_M_A2IEB    (0x08u) // Mask - alarm2 interrupt enable [b3] (0 - disabled, 1 - enabled)
#define CTRL_REG_INTA_ENABLE_M_A1IEB    (0x10u) // Mask - alarm1 interrupt enable [b4] (0 - disabled, 1 - enabled)
#define CTRL_REG_INTA_ENABLE_M_OIEB     (0x20u) // Mask - offset correction interrupt enable [b5] (0 - disabled, 1 - enabled)
#define CTRL_REG_INTA_ENABLE_M_PIEB     (0x40u) // Mask - periodic interrupt enable [b6] (0 - disabled, 1 - enabled)
#define CTRL_REG_INTA_ENABLE_M_ILPB     (0x80u) // Mask - level or pulse mode [b7] (0 - pulse, 1 - follow flags [permanent level])

/*===[control registers]=====================*/
#define CTRL_REG_ADDR_FLAGS             (0x2Bu)

#define CTRL_REG_FLAGS_M_TSR1F          (0x01u) // Mask - timestamp register 1 event flag [b0] (0 - [R: flag is inactive, W: clear flag], 1 - [R: flag is active, W: no change])
#define CTRL_REG_FLAGS_M_TSR2F          (0x02u) // Mask - timestamp register 2 event flag [b1] (0 - [R: flag is inactive, W: clear flag], 1 - [R: flag is active, W: no change])
#define CTRL_REG_FLAGS_M_TSR3F          (0x04u) // Mask - timestamp register 3 event flag [b2] (0 - [R: flag is inactive, W: clear flag], 1 - [R: flag is active, W: no change])
#define CTRL_REG_FLAGS_M_BSF            (0x08u) // Mask - battery switch flag [b3] (0 - [R: flag is inactive, W: clear flag], 1 - [R: flag is active, W: no change])
#define CTRL_REG_FLAGS_M_WDF            (0x10u) // Mask - watchdog flag [b4] (0 - [R: flag is inactive, W: clear flag], 1 - [R: flag is active, W: no change])
#define CTRL_REG_FLAGS_M_A1F            (0x20u) // Mask - alarm1 flag [b5] (0 - [R: flag is inactive, W: clear flag], 1 - [R: flag is active, W: no change])
#define CTRL_REG_FLAGS_M_A2F            (0x40u) // Mask - alarm2 flag [b6] (0 - [R: flag is inactive, W: clear flag], 1 - [R: flag is active, W: no change])
#define CTRL_REG_FLAGS_M_PIF            (0x80u) // Mask - periodic interrupt flag [b7] (0 - [R: flag is inactive, W: clear flag], 1 - [R: flag is active, W: no change])

/*===[control registers]=====================*/
#define CTRL_REG_ADDR_RAM_BYTE          (0x2Cu) // Any purpose, 8-bit storage for user application

/*===[control registers]=====================*/
#define CTRL_REG_ADDR_WATCHDOG          (0x2Du)

#define CTRL_REG_WDG_M_WDS              (0x03u) // Mask - watchdog step size [b1:0]
#define WDG_WDS_SHIFT                   (0)     // Shift - watchdog step size
#define WDG_WDS_4_SEC                   (0b00 << WDG_WDS_SHIFT) // 4 second step size (0.25Hz source)
#define WDG_WDS_1_SEC                   (0b01 << WDG_WDS_SHIFT) // 1 second step size (1Hz source)
#define WDG_WDS_1_4_SEC                 (0b10 << WDG_WDS_SHIFT) // 1/4 second step size (4Hz source)
#define WDG_WDS_1_16_SEC                (0b11 << WDG_WDS_SHIFT) // 1/16 second step size (16Hz source)

#define CTRL_REG_WDG_M_WDR              (0x7Cu) // Mask - watchdog register bits [b6:2] (R: current counter value, W: watchdog counter load value)
#define CTRL_REG_WDG_M_WDM              (0x80u) // Mask - watchdog mode [b7] (0 - single shot, 1 - repeat mode)

/*===[control registers]=====================*/
#define CTRL_REG_ADDR_STOP_ENABLE       (0x2Eu)

#define CTRL_REG_STOP_ENABLE_M_STOP     (0x01u) // Mask - STOP bit [b0] (0 - RTC runs, 1 - RTC is stopped)
#define CTRL_REG_STOP_ENABLE_M_UNUSED   (0xF7u) // Unused

/*===[control registers]=====================*/
#define CTRL_REG_ADDR_RESETS            (0x2Fu)

#define CTRL_REG_RESET_BASE             (0x24u) // Base value - AND bits for different reset functions
#define REG_RESET_BIT_SR                (0x04u) // Software reset, also triggers CTS and CPR
#define REG_RESET_BIT_CPR               (0x80u) // Clear prescaler, can be combined with CTS
#define REG_RESET_BIT_CTS               (0x01u) // Clear timestamp, can be combined with CPR

/* ===========================================*/

/*=============================================================================*/
/*][ GLOBAL : Exportable Variables ][==========================================*/
/*=============================================================================*/

/*=============================================================================*/
/*][ GLOBAL : Exportable Function Prototypes ][================================*/
/*=============================================================================*/

extern STATUS_E     RTC_init(UINT8 rtc_addr_u8);
extern STATUS_E     RTC_get_time(struct tm* p_timestamp_s);
extern STATUS_E     RTC_set_time(struct tm* p_timestamp_s);

/* End */
#define WC_RTC_H
#endif