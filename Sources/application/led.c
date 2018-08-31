/** @FILE NAME:    template.c
 *  @DESCRIPTION:  This file for ...
 *
 *  Copyright (c) 2018 EES Ltd.
 *  All Rights Reserved This program is the confidential and proprietary
 *  product of EES Ltd. Any Unauthorized use, reproduction or transfer
 *  of this program is strictly prohibited.
 *
 *  @Author: HaiHoang
 *  @NOTE:   No Note at the moment
 *  @BUG:    No known bugs.
 *
 *<pre>
 *  MODIFICATION HISTORY:
 *
 *  Ver   Who       Date                Changes
 *  ----- --------- ------------------  ----------------------------------------
 *  1.00  HaiHoang  August 1, 2018      First release
 *
 *
 *</pre>
******************************************************************************/
/***************************** Include Files *********************************/
#include <led.h>
#include <bsp.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/
SLed obj[LED_NUM_NODE];
/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */

void LED_InitAll() {
	LED_Init(&obj[0], PIN_DISP_RUN_IDX, 		TRUE,   4000, 	2000, BSP_SetGpioPin);
	LED_Init(&obj[1], PIN_DISP_BATT_LOW_IDX, 	FALSE, 	2000, 	1000, BSP_SetGpioPin);
	LED_Init(&obj[2], PIN_DISP_BATT_CHARG_IDX, 	FALSE, 	3000, 	1500, BSP_SetGpioPin);
	LED_Init(&obj[3], PIN_DISP_BATT_FULL_IDX, 	FALSE, 	4000, 	2000, BSP_SetGpioPin);
	LED_Init(&obj[4], PIN_DISP_BATT_EMPTY_IDX, 	FALSE, 	5000, 	2500, BSP_SetGpioPin);	
}
/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */

void LED_Init(SLed *led, uint8_t id, uint8_t blink, 
		uint32_t period, uint32_t high, void *func) {
	led->pinId = id;
	led->isBlink = blink;
	led->period = period;
	led->high = high;
	led->clb_func = func;
	led->counter = 0;
}
/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */

void LED_SetPeriod(uint8_t id, uint32_t period) {
	(&obj[id])->period = period;	
}
/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */

void LED_SetHighDuty(uint8_t id, uint32_t high) {
	(&obj[id])->high = high;
}
/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */

void LED_SetBlink(uint8_t id, uint8_t blink) {
	(&obj[id])->isBlink = blink;
	if(!blink) {
		(&obj[id])->clb_func((&obj[id])->pinId, 0);
	}
}
/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */



void LED_ActAll() {
	int i = 0;
	for(; i < LED_NUM_NODE; i++) {
		LED_Act(&obj[i]);
	}
}

/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */

void LED_OffAll() {
	int i = 0;
	for(; i < LED_NUM_NODE; i++) {
		LED_SetBlink(i, 0);
	}
}
/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */

void LED_OnAll() {
	int i = 0;
	for(; i < LED_NUM_NODE; i++) {
		LED_SetBlink(i, 1);
	}
}
