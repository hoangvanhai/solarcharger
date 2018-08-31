
/** @FILE NAME:    led.h
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
#ifndef LED_H_
#define LED_H_
/***************************** Include Files *********************************/
#include <typedefs.h>
/************************** Constant Definitions *****************************/
#define LED_NUM_NODE		5
/**************************** Type Definitions *******************************/
typedef void(*led_func) (uint32_t, uint8_t);

typedef struct SLed_ {
	uint8_t		id;
	uint8_t		pinId;
	uint8_t 	isBlink;
	uint32_t 	high;			// high in ms
	uint32_t 	period;		// period in ms
	uint32_t	counter;
	led_func	clb_func;
}SLed;

/***************** Macros (Inline Functions) Definitions *********************/
#define LED_Act(pled) { \
	if((pled)->isBlink) { \
		(pled)->counter++;	\
		if((pled)->counter == (pled)->high) { \
			(pled)->clb_func((pled)->pinId, 0); \
		} else if((pled)->counter == (pled)->period) { \
			(pled)->counter = 0; \
			(pled)->clb_func((pled)->pinId, 1); \
		} \
		if((pled)->counter > (pled)->period) { \
			(pled)->counter = 0; \
		} \
	} \
} 
/************************** Function Prototypes ******************************/
void LED_InitAll();
void LED_ActAll();
void LED_OffAll();
void LED_OnAll();
void LED_Init(SLed *led, uint8_t id, uint8_t blink, 
		uint32_t period, uint32_t high, void *func);
void LED_SetPeriod(uint8_t id, uint32_t period);
void LED_SetHighDuty(uint8_t id, uint32_t high);
void LED_SetBlink(uint8_t id, uint8_t blink);

/************************** Variable Definitions *****************************/

/*****************************************************************************/


#endif /* LED_H_ */
