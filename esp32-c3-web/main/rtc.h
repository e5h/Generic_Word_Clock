/*==============================================================================
 *==============================================================================
 * NAME:
 *      rtc.h
 *
 * PURPOSE:
 *      This module encapsulates the real time clock.
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

/*=============================================================================*/
/*][ GLOBAL : Constants and Types ][===========================================*/
/*=============================================================================*/

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

#define CTRL_REG_ADDR_BATTERY_SWITCH    (0x26u)
// TODO

#define CTRL_REG_ADDR_PIN_IO            (0x27u)
// TODO

#define CTRL_REG_ADDR_FUNCTION          (0x28u)
// TODO

#define CTRL_REG_ADDR_INTA_ENABLE       (0x29u)
// TODO

#define CTRL_REG_ADDR_INTB_ENABLE       (0x2Au)
// TODO

#define CTRL_REG_ADDR_FLAGS             (0x2Bu)
// TODO

/* RAM byte */
#define CTRL_REG_ADDR_RAM_BYTE          (0x2Cu)

/* WDT registers */
#define CTRL_REG_ADDR_WATCHDOG          (0x2Du)
// TODO

/* Stop */
#define CTRL_REG_ADDR_STOP_ENABLE       (0x2Eu)

/* Reset */
#define CTRL_REG_ADDR_RESETS            (0x2Fu)
// TODO

/*=============================================================================*/
/*][ GLOBAL : Exportable Variables ][==========================================*/
/*=============================================================================*/

/*=============================================================================*/
/*][ GLOBAL : Exportable Function Prototypes ][================================*/
/*=============================================================================*/

/* End */
#define WC_RTC_H
#endif