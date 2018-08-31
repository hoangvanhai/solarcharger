/*
 * bsp.h
 *
 *  Created on: Aug 22, 2018
 *      Author: MSI
 */

#ifndef BSP_H_
#define BSP_H_

#include <stdint.h>
#include "common.h"
#include "adc.h"
#include "rtc.h"
#include "uart.h"
#include "gpio.h"
#include "pit.h"
#include "ftm.h"
#include "sysinit.h"
#include "sim.h"
#include "MKE06Z4.h"
#include "console.h"
#include "hardware_profile.h"


typedef enum GPIO_PinIdx_ {
	PIN_DISP_RUN_IDX = 0,
	PIN_DISP_BATT_LOW_IDX,
	PIN_DISP_BATT_CHARG_IDX,
	PIN_DISP_BATT_FULL_IDX,
	PIN_DISP_BATT_EMPTY_IDX,
	PIN_CTRL_BUCK_DRV_IDX,
	PIN_CTRL_VUSB_IDX,
	PIN_CTRL_LOAD_IDX,
	PIN_CTRL_U1_16_IDX,
	PIN_DISP_U1_48_IDX,
	PIN_MAX_IDX
}GPIO_PinIdx;


void BSP_Init();
void BSP_InitPIT0(uint32_t freq);
void BSP_InitPIT1(uint32_t freq);
void BSP_InitUart(uint32_t bitrate);
void BSP_InitPwm();
void BSP_InitAdc();
void BSP_InitGpio();
void BSP_InitGpioPin(uint32_t pin);
void BSP_SetHighGpioPin(uint32_t pin);
void BSP_SetLowGpioPin(uint32_t pin);
void BSP_ToggleGpioPin(uint32_t pin);
void BSP_SetGpioPin(uint32_t pin, uint8_t lev);

void UART_HandleInt(UART_Type *pUART);
void PIT0_HandleInt(void);
void PIT1_HandleInt(void);
void FTM0_HandleInt(void);
void FTM2_HandleInt(void);
void ADC_HandleInt(void);




#define GPIO_TOGGLE_RUN()							GPIO_PinToggle(PIN_DISP_RUNNING)
#define GPIO_SET_LOW_RUN()							GPIO_PinClear(PIN_DISP_RUNNING)
#define GPIO_SET_HIGH_RUN()							GPIO_PinSet(PIN_DISP_RUNNING)

#define GPIO_TOGGLE_DISP_BATT_LOW()					GPIO_PinToggle(PIN_DISP_BATT_LOW)
#define GPIO_SET_LOW_DISP_BATT_LOW()				GPIO_PinClear(PIN_DISP_BATT_LOW)
#define GPIO_SET_HIGH_DISP_BATT_LOW()				GPIO_PinSet(PIN_DISP_BATT_LOW)

#define GPIO_TOGGLE_DISP_BATT_CHARG()				GPIO_PinToggle(PIN_DISP_BATT_CHARG)
#define GPIO_SET_LOW_DISP_BATT_CHARG()				GPIO_PinClear(PIN_DISP_BATT_CHARG)
#define GPIO_SET_HIGH_DISP_BATT_CHARG()				GPIO_PinSet(PIN_DISP_BATT_CHARG)

#define GPIO_TOGGLE_DISP_BATT_FULL()				GPIO_PinToggle(PIN_DISP_BATT_FULL)
#define GPIO_SET_LOW_DISP_BATT_FULL()				GPIO_PinClear(PIN_DISP_BATT_FULL)
#define GPIO_SET_HIGH_DISP_BATT_FULL()				GPIO_PinSet(PIN_DISP_BATT_FULL)

#define GPIO_TOGGLE_DISP_BATT_EMPTY()				GPIO_PinToggle(PIN_DISP_BATT_EMPTY)
#define GPIO_SET_LOW_DISP_BATT_EMPTY()				GPIO_PinClear(PIN_DISP_BATT_EMPTY)
#define GPIO_SET_HIGH_DISP_BATT_EMPTY()				GPIO_PinSet(PIN_DISP_BATT_EMPTY)

#define GPIO_TOGGLE_CTRL_BUCK_DRV()					GPIO_PinToggle(PIN_CTRL_BUCK_DRV)
#define GPIO_SET_LOW_CTRL_BUCK_DRV()				GPIO_PinClear(PIN_CTRL_BUCK_DRV)
#define GPIO_SET_HIGH_CTRL_BUCK_DRV()				GPIO_PinSet(PIN_CTRL_BUCK_DRV)

#define GPIO_TOGGLE_CTRL_VUSB_EN()					GPIO_PinToggle(PIN_CTRL_VUSB_EN)
#define GPIO_SET_LOW_CTRL_VUSB_EN()					GPIO_PinClear(PIN_CTRL_VUSB_EN)
#define GPIO_SET_HIGH_CTRL_VUSB_EN()				GPIO_PinSet(PIN_CTRL_VUSB_EN)

#define GPIO_TOGGLE_CTRL_LOAD_EN()					GPIO_PinToggle(PIN_CTRL_LOAD_EN)
#define GPIO_SET_LOW_CTRL_LOAD_EN()					GPIO_PinClear(PIN_CTRL_LOAD_EN)
#define GPIO_SET_HIGH_CTRL_LOAD_EN()				GPIO_PinSet(PIN_CTRL_LOAD_EN)

#define GPIO_TOGGLE_CTRL_U1_16()					GPIO_PinToggle(PIN_CTRL_U1_16)
#define GPIO_SET_LOW_CTRL_U1_16()					GPIO_PinClear(PIN_CTRL_U1_16)
#define GPIO_SET_HIGH_CTRL_U1_16()					GPIO_PinSet(PIN_CTRL_U1_16)

#define GPIO_TOGGLE_CTRL_U1_48()					GPIO_PinToggle(PIN_CTRL_U1_48)
#define GPIO_SET_LOW_CTRL_U1_48()					GPIO_PinClear(PIN_CTRL_U1_48)
#define GPIO_SET_HIGH_CTRL_U1_48()					GPIO_PinSet(PIN_CTRL_U1_48)

extern uint32_t		GPIO_Pin[];

#endif /* BSP_H_ */




