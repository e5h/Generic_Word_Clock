/*==============================================================================
 *==============================================================================
 * NAME:
 *      task_device.c
 *
 * PURPOSE:
 *      One of the major tasks of the application.
 *
 *      The device task is responsible for handling the I2C bus and all connected
 *      devices including buttons, sensors, flash memory, and system peripherals.
 *
 * DEPENDENCIES:
 *      task_device.h
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

#include "task_device.h"
#include "lib_messaging.h"

/*=============================================================================*/
/*][ LOCAL : Constants and Types ][============================================*/
/*=============================================================================*/

const static char* LOG_TAG = "task_device";

/*=============================================================================*/
/*][ LOCAL : Variables ][======================================================*/
/*=============================================================================*/

/* ESP event groups (flags) */
static EventGroupHandle_t h_device_flags;

/* FreeRTOS task handles */
static TaskHandle_t h_device_task;

/* Order and request queues */
static QueueHandle_t h_device_queue;

/*=============================================================================*/
/*][ LOCAL : Function Prototypes ][============================================*/
/*=============================================================================*/

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

/**===< global >===============================================================
 * NAME:
 *
 * SUMMARY:
 *
 * INPUT REQUIREMENTS:
 *
 * OUTPUT GUARANTEES:
 **===< global >===============================================================*/