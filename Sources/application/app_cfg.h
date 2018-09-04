/*
 * app_cfg.h
 *
 *  Created on: Aug 22, 2018
 *      Author: MSI
 */

#ifndef APP_CFG_H_
#define APP_CFG_H_

#define 	APP_FIRMWARE_VER				("1.0.0")
#define		APP_SETTING_PASSWD				("load")
#define		APP_DEVICE_ID					(1)

#define 	DEBUG
#define 	CONSOLE

#define		MILLI_SEC_ON_HOUR				(7200000)
#define 	MILLI_SEC_ON_MINUTE				(120000)

#define		APP_CHARGE_CONST_VOLT_TIME		(120 * MILLI_SEC_ON_MINUTE)	// const volt on 120 minute

#define 	APP_TIMER_FREQ					(1000)	//Hz
#define 	ADC_READ_FREQ					(1000)	//Hz

#define 	BATT_VOLT_FULL_VALUE			(14200)	// mVolt
#define 	BATT_VOLT_FLOAT_VALUE			(13800) // mVolt
#define 	BATT_VOLT_LOW_VALUE				(13000)	// mVolt
#define 	BATT_VOLT_EMPTY_VALUE			(9000)	// mVolt

#define		BATT_MAX_CURRENT_VALUE			(1000) 	// Ampe
#define 	BATT_MIN_CURRENT_VALUE			(500)


#define 	PANEL_VOLT_HIGH_PROTECT			(60000)	// volt
#define 	PANEL_VOLT_LOW_PROTECT			(15000)	// milli volt
#define		PANEL_CURR_HIGH_VALUE			(5000)	// Ampe

	
#define 	ADC_PANEL_VOLT_COEFF			(11.5064695009242) 	// convert to mV
#define		ADC_BATT_VOLT_COEFF				(6.3958333333333)	// convert to mV
#define 	ADC_PANEL_CURR_COEFF			(12.25925925925926)	//(15.85106382978725)	//(31.7021276595745)	// convert to mA

#define		APP_MAX_DUTY_PERCEN				(0.95)
#define 	APP_MIN_DUTY_PERCEN				(0)
#define 	APP_STEP_DUTY_PERCEN			(0.001);

#define 	MPPT_POWER_DELTA_MIN			(50)
#define 	MPPT_VOLT_STEP_SIZE				(10)
#define 	MPPT_VOLT_OUT_MAX				(20000)
#define		MPPT_VOLT_OUT_MIN				(13000)

#define 	PID_UPDATE_FREQ					(2000)
#define		BUCKER_Kp						(0.01)
#define 	BUCKER_Ki						(0)
#define		BUCKER_Kd						(0)

#define		KP_A_COEFF						(1)
#define		KP_B_COEFF						(1)
#define		POWER_FACTOR					(0.95)

#define		CONTROL_PID						0
#define		CONTROL_THRESHOLD				1
#define 	BUCKER_CONTROL_METHOD			CONTROL_THRESHOLD



#endif /* APP_CFG_H_ */
