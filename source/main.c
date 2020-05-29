/******************************************************************************
* File Name: main.c
*
* Description: This code example demonstrates OTA update with PSoC 6 MCU and
* CYW43xxx connectivity devices. The device establishes a connection with the
* designated MQTT Broker (Mosquitto is used in this example) and subscribes to
* a topic. When an OTA image is published to that topic, the device automatically
* pulls the OTA image over MQTT and saves it to the secondary slot in the flash
* memory. On the next reboot, MCUBoot will copy the new image over to the
* primary slot and run the application.
*
* Related Document: See Readme.md
*
*******************************************************************************
* (c) 2020, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*******************************************************************************/

/* Header file includes */
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "ota_task.h"
#include "led_task.h"

/* FreeRTOS header file */
#include <FreeRTOS.h>
#include <task.h>

/*******************************************************************************
* Macros
********************************************************************************/
/* OTA task configurations */
#define OTA_TASK_STACK_SIZE                 (1024 * 6)
#define OTA_TASK_PRIORITY                   (configMAX_PRIORITIES - 3)

/* OTA task configurations */
#define LED_TASK_STACK_SIZE                 (configMINIMAL_STACK_SIZE)
#define LED_TASK_PRIORITY                   (configMAX_PRIORITIES - 3)

/*******************************************************************************
* Global Variables
********************************************************************************/
/* OTA task handle */
TaskHandle_t ota_task_handle;

/* LED task handle */
TaskHandle_t led_task_handle;

/* This enables RTOS aware debugging. */
volatile int uxTopUsedPriority;

/*******************************************************************************
 * Function Name: main
 ********************************************************************************
 * Summary:
 *  System entrance point. This function sets up OTA task and starts
 *  the RTOS scheduler.
 *
 * Parameters:
 *  void
 *
 * Return:
 *  int
 *
 *******************************************************************************/
int main(void)
{
    cy_rslt_t result ;

    /* This enables RTOS aware debugging in OpenOCD */
    uxTopUsedPriority = configMAX_PRIORITIES - 1 ;

    /* Initialize the board support package */
    result = cybsp_init() ;
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                                 CY_RETARGET_IO_BAUDRATE);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* To avoid compiler warning */
    (void)result;
    
    /* Enable global interrupts. */
    __enable_irq();

    printf("===============================================================\n");
    printf("TEST Application: OTA Update version: %d.%d.%d\n",
            APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_BUILD);
    printf("===============================================================\n\n");

    /* Create the tasks */
    xTaskCreate(ota_task, "OTA TASK", OTA_TASK_STACK_SIZE, NULL,
                OTA_TASK_PRIORITY, &ota_task_handle);
    xTaskCreate(led_task, "LED TASK", LED_TASK_STACK_SIZE, NULL,
                LED_TASK_PRIORITY, &led_task_handle);

    /* Start the FreeRTOS scheduler. */
    vTaskStartScheduler();

    /* Should never get here. */
    CY_ASSERT(0);
}

/* [] END OF FILE */
