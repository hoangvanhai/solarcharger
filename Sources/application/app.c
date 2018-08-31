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

#include <app.h>
#include <bsp.h>
#include <led.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

void Clb_TimerControl(uint32_t tick, void *param);
void Clb_TimerUpdate(uint32_t tick, void *param);
void Clb_TimerGui(uint32_t tick, void *param);

void clear_screen(int32_t argc, char**argv);
void clear_screen(int32_t argc, char**argv);
void help_cmd(int32_t argc, char **argv);
void restart(int32_t argc, char**argv);
void led_cmd(int32_t argc, char **argv);
void set_duty_cmd(int32_t argc, char **argv);

/* user command */
void set_vusb(int32_t argc, char **argv);
void set_charge_onoff(int32_t argc, char **argv);
void set_board_id(int32_t argc, char **argv);
void set_float_volt(int32_t argc, char **argv);
void set_boost_volt(int32_t argc, char **argv);
void set_boost_curr(int32_t argc, char **argv);
void set_const_volt_time(int32_t argc, char **argv);
void set_passwd(int32_t argc, char **argv);
void get_info(int32_t argc, char **argv);
void set_log(int32_t argc, char **argv);
void get_setting(int32_t argc, char **argv);


/************************** Variable Definitions *****************************/
SApp sApp;

//const shell_command_t cmd_table[] =
//{
//	{"help", 	0u, 0u, help_cmd, 		"Display this help message", ""},
//	{"led", 	2u, 2u, led_cmd, 		"Led control", "<on|off>"},	
//	{"clear", 	0u, 0u, clear_screen, 	"clear screen", ""},
//	{"restart", 0u, 0u, restart, 		"restart mcu", ""},
//	{"duty", 	1u, 1u, set_duty_cmd, 	"set duty cycle", ""},	
//	{"vusb", 	1u, 1u, set_vusb, 		"control 5VDC usb", "<on|off>"},
//	{"charg", 	1u, 1u, set_charge_onoff, 		"charge control", "<on|off>"},
//	{"bid", 	1u, 1u, set_board_id, 		"set board id", "<id>"},
//	{"fvolt", 	1u, 1u, set_float_volt, "set float volt (mV)", "<volt>"},
//	{"bvolt", 	1u, 1u, set_boost_volt, "set boost volt (mV)", "<volt>"},
//	{"bcurr", 	1u, 1u, set_boost_curr, "set boost current max (mA)", "<ampe"},
//	{"btime", 	1u, 1u, set_const_volt_time, "set boost time (hour)", "<time>"},
//	{"pass", 	1u, 1u, set_passwd, 	"issue password confirm action", "<passwd>"},
//	{"show", 	0u, 0u, get_info, 		"show board info", ""},
//	{"log", 	1u, 1u, set_log, 		"continuous log board info", "<on|off>"},
//	{0, 0u, 0u, 0, 0, 0}
//};

const shell_command_t cmd_table[] =
{
	{"help", 	0u, 0u, help_cmd, 		"Display this help message", ""},
	{"reset", 	0u, 0u, restart, 		"reset system", ""},
	{"clear", 	0u, 0u, clear_screen, 	"clear screen", ""},
	{"vusb", 	1u, 1u, set_vusb, 		"control 5v usb port, ex: \"vusb on\" for turn on 5v usb port", "<on|off>"},
	{"charg", 	1u, 1u, set_charge_onoff, "control charge, ex: \"charg on\" for enable charger", "<on|off>"},
	{"bid", 	1u, 1u, set_board_id, 	"set board id, ex: \"bid 10\" for set board id = 10", "<id>"},
	{"fvolt", 	1u, 1u, set_float_volt, "set float volt (mV), ex: \"fvolt: 13800\" for set float volt = 13800 mV", "<volt>"},
	{"bvolt", 	1u, 1u, set_boost_volt, "set boost volt (mV), ex: \"bvolt: 14200\" for set boost volt = 14200 mV", "<volt>"},
	{"bcurr", 	1u, 1u, set_boost_curr, "set boost current max (mA), ex: \"bcurr 3000\" for set boost current = 3000 mA", "<ampe"},
	{"btime", 	1u, 1u, set_const_volt_time, "set boost time (minute), ex: \"btime 120\" for set boost time = 120 minute", "<time>"},
	{"pass", 	1u, 1u, set_passwd, 	"issue password confirm action", "<passwd>"},
	{"sinfo", 	0u, 0u, get_info, 		"show board info", ""},
	{"log", 	1u, 1u, set_log, 		"continuous log board status on/off", "<on|off>"},
	{"scfg", 	0u, 0u, get_setting, 	"show board setting", ""},
	{0, 0u, 0u, 0, 0, 0}
};

const char *status[]  = {
	"DEVICE_IDLE",
	"DEVICE_STARTING",
	"DEVICE CONST CURRENT",
	"DEVICE MPPT VOLT",
	"DEVICE CONST VOLT",
	"DEVICE STOP",
};

/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */
void App_Init(SApp *pApp) {	
	Timer_Init();
	
	pApp->hTimerUpdate = Timer_Create(Clb_TimerUpdate, NULL);
	Timer_SetRate(pApp->hTimerUpdate, 100);
	Timer_Start(pApp->hTimerUpdate);
	
	pApp->hTimerGui = Timer_Create(Clb_TimerGui, NULL);
	Timer_SetRate(pApp->hTimerGui, 2000);
	//Timer_Start(pApp->hTimerGui);
	
	pApp->hTimerControl = Timer_Create(Clb_TimerControl, NULL);
	Timer_SetRate(pApp->hTimerControl, APP_CHARGE_CONST_VOLT_TIME);
	Timer_Start(pApp->hTimerControl);
	
	pApp->eDevState = 0;
	pApp->eDevState = DS_DEV_RUN | DS_BATT_VOLT_LOW | DS_PANEL_VOLT_LOW;
	
	pApp->eBuckerSM = BSM_BUCKER_STOP;		
	pApp->currDutyPer = 0;
	pApp->id = APP_DEVICE_ID;
	
	MPPT_PNO_F_init(&pApp->sMppt);
	
    PID_Init(&pApp->sPid, 
    		BUCKER_Kp, 
    		BUCKER_Ki, 
    		BUCKER_Kd, 
    		PID_UPDATE_FREQ,
            KP_A_COEFF, KP_B_COEFF);
	
	// init input caclculating
	Adc_InitValue(pApp->battVolt,                   /* adc node */
				  0,                                /* type adc node 1 avg 0 filter  */
				  ADC_BATT_VOLT_COEFF,       		/* coefficient for calculate realvalue */
				  1000,                             /* average counter */
				  0,                                /* offset value */
				  2);          						/* cuttoff freq */
	
	Adc_InitValue(pApp->panelCurr,
				  1,
				  ADC_PANEL_CURR_COEFF,
				  1000,
				  1976,	//1939,	//1963,
				  2);
	
	Adc_InitValue(pApp->panelVolt,
				  0,
				  ADC_PANEL_VOLT_COEFF,
				  2048,
				  0,
				  2);
	
	pApp->panelLastVolt = 0;
	pApp->panelLastCurr = 0;
	pApp->battLastVolt = 0;	
	pApp->battCurr = 0;
	pApp->battLastCurr = 0;
	pApp->panelPower = 0;
	
	// init control value 
	pApp->chargConstVoltTime 	= APP_CHARGE_CONST_VOLT_TIME;
	pApp->chargMaxCurrent 		= BATT_MAX_CURRENT_VALUE;
	pApp->chargMaxVolt 			= BATT_VOLT_FULL_VALUE;
	pApp->chargFloatVolt		= BATT_VOLT_FLOAT_VALUE;
	pApp->vUsb					= TRUE;
}
/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */
void App_Control(SApp *pApp) {
	
	static float peak = 0;
	/* *
	 * Check panel voltage
	 */
	if(pApp->panelVolt.realValue <= PANEL_VOLT_LOW_PROTECT) {		
		if(pApp->eBuckerSM != BSM_BUCKER_STARTING && pApp->eBuckerSM != BSM_BUCKER_CHARG_MPPT_VOLT_CTRL) {
			App_SetDevState(pApp, DS_PANEL_VOLT_LOW);	
		}
	} else if(pApp->panelVolt.realValue >= PANEL_VOLT_HIGH_PROTECT) {
		App_SetDevState(pApp, DS_PANEL_VOLT_HIGH);
	} else {
		if(pApp->eDevState & DS_PANEL_VOLT_HIGH) {
			App_ClearDevState(pApp, DS_PANEL_VOLT_HIGH);
		}
		if(pApp->eDevState & DS_PANEL_VOLT_LOW) {
			App_ClearDevState(pApp, DS_PANEL_VOLT_LOW);
		}
	}
	
	/* *
	 * Check panel current
	 */
	if(pApp->panelCurr.realValue > PANEL_CURR_HIGH_VALUE) {
		App_ClearDevState(pApp, DS_PANEL_CURR_HIGH);
	} else {
		if(pApp->eDevState & DS_PANEL_CURR_HIGH) {
			App_ClearDevState(pApp, DS_PANEL_CURR_HIGH);
		}
	}
	
	/**
	 * check battery voltage
	 */
	if(pApp->battVolt.realValue < BATT_VOLT_EMPTY_VALUE) {
		
		//if(!(pApp->eDevState & DS_BATT_VOLT_EMPTY)) 
		{
			App_SetDevState(pApp, DS_BATT_VOLT_EMPTY);
			GPIO_SET_HIGH_DISP_BATT_EMPTY();				
			GPIO_SET_LOW_DISP_BATT_LOW();	
		}
		
		if(pApp->eDevState & DS_BATT_VOLT_FULL) {
			App_ClearDevState(pApp, DS_BATT_VOLT_FULL);
			GPIO_SET_LOW_DISP_BATT_FULL();
		}	
	} else if (pApp->battVolt.realValue >= BATT_VOLT_EMPTY_VALUE && 
			pApp->battVolt.realValue < BATT_VOLT_FULL_VALUE) {
		
		
		if(pApp->eDevState & DS_BATT_VOLT_EMPTY) {
			App_ClearDevState(pApp, DS_BATT_VOLT_EMPTY);
			GPIO_SET_LOW_DISP_BATT_EMPTY();
		}
				
		if(pApp->eDevState & DS_BATT_VOLT_LOW) {
			App_ClearDevState(pApp, DS_BATT_VOLT_LOW);
			//GPIO_SET_LOW_DISP_BATT_LOW();
		}		
	} 
	
	
	/**
	 * LOGIC CONTROL DEVICE BEHAVIOUS
	 */
	switch(pApp->eBuckerSM) {
	
	case BSM_BUCKER_STOP:
		if(pApp->eDevState == DS_DEV_RUN) {
			App_StartBucker(pApp);
			pApp->eBuckerSM = BSM_BUCKER_STARTING;			
			LREP("\r\nSTART BUCKER\r\n\n");
		}
		
		break;
	case BSM_BUCKER_STARTING:
		if(pApp->eDevState == DS_DEV_RUN) {
			// if max current is reached
			if(sApp.battCurr >= pApp->chargMaxCurrent) {
				peak = sApp.battCurr;
				// change to const current phase
				pApp->eBuckerSM = BSM_BUCKER_CHARG_CONST_CURR;
				LREP("\r\nSTARTING CURRENT MAX %d -> CHANGE TO CONST CURRENT\r\n\n", (int)peak);				
			}

			if(pApp->battVolt.realValue >= pApp->chargMaxVolt) {
				pApp->eBuckerSM = BSM_BUCKER_CHARG_VOLT_MAX;
				Timer_Stop(pApp->hTimerControl);
				Timer_StartAt(pApp->hTimerControl, pApp->chargConstVoltTime);
				LREP("\r\nSTARTING -> VOLT MAX -> START TRACK CONST VOLT TIME\r\n\n");
			}
			
			// if duty is not reached max 
			if(pApp->currDutyPer < APP_MAX_DUTY_PERCEN) {
				// increment duty
				pApp->currDutyPer += 0.01;
				App_SetDutyPercen(pApp->currDutyPer);
			} else { // is max duty
				App_StartBucker(pApp);
				// change to mppt to get const voltage
				pApp->eBuckerSM = BSM_BUCKER_CHARG_MPPT_VOLT_CTRL;				
				LREP("\r\nSTARTING DUTY MAX -> VOLT OPTMZ\r\n\n");
			}			
		} else {			
			App_StopBucker(pApp);
			pApp->eBuckerSM = BSM_BUCKER_STOP;
		}		
		
		break;
	
	case BSM_BUCKER_CHARG_CONST_CURR:
		if(pApp->eDevState == DS_DEV_RUN) {

			#if BUCKER_CONTROL_METHOD == CONTROL_THRESHOLD
			float value = (pApp->battCurr - pApp->battLastCurr);
			if(value < 0) value = -value; 
			if(value > 50.0) {
				// if max current is reached
				if(pApp->battCurr > pApp->chargMaxCurrent) {
					pApp->currDutyPer -= APP_STEP_DUTY_PERCEN;
					if(pApp->currDutyPer < 0) pApp->currDutyPer = 0;
					App_SetDutyPercen(pApp->currDutyPer);
				} else if(sApp.battCurr < BATT_MAX_CURRENT_VALUE){
					pApp->currDutyPer += APP_STEP_DUTY_PERCEN;
					if(pApp->currDutyPer > APP_MAX_DUTY_PERCEN) pApp->currDutyPer = APP_MAX_DUTY_PERCEN;
					App_SetDutyPercen(pApp->currDutyPer);
				}
				pApp->battLastCurr = pApp->battCurr;
			}						
			#endif
			
			if(pApp->battVolt.realValue >= pApp->chargMaxVolt) {
				pApp->eBuckerSM = BSM_BUCKER_CHARG_VOLT_MAX;
				Timer_StartAt(pApp->hTimerControl, pApp->chargConstVoltTime);
				LREP("\r\nCONST CURRENT -> VOLT MAX -> START TRACK CONST VOLT TIME\r\n\n");
			}

			// if duty is reached max 
			if(pApp->currDutyPer >= APP_MAX_DUTY_PERCEN) {
				//pApp->eBuckerSM = BSM_BUCKER_CHARG_MPPT_VOLT_CTRL;
				LREP("\r\nCONST CURRENT DUTY MAX -> VOLT OPTMZ\r\n\n");
			}
		} else {			
			App_StopBucker(pApp);
			pApp->eBuckerSM = BSM_BUCKER_STOP;
		}
		break;
		
	case BSM_BUCKER_CHARG_MPPT_VOLT_CTRL:
		if(pApp->eDevState == DS_DEV_RUN) {			
			pApp->sMppt.Ipv = pApp->panelCurr.realValue;
			pApp->sMppt.Vpv = pApp->panelVolt.realValue;				
			MPPT_PNO_F_MACRO(pApp->sMppt);
			
			#if BUCKER_CONTROL_METHOD == CONTROL_THRESHOLD
			float value = (pApp->panelVolt.realValue - pApp->panelLastVolt);
			if(value < 0) value = -value; 
			if(value > 10.0) {
				// if max current is reached
				if(sApp.panelVolt.realValue > pApp->sMppt.VmppOut) {
					pApp->currDutyPer += APP_STEP_DUTY_PERCEN;
					if(pApp->currDutyPer > APP_MAX_DUTY_PERCEN) pApp->currDutyPer = APP_MAX_DUTY_PERCEN;
					App_SetDutyPercen(pApp->currDutyPer);
					//out_char('>');
				} else if(sApp.panelVolt.realValue < pApp->sMppt.VmppOut){
					pApp->currDutyPer -= APP_STEP_DUTY_PERCEN;
					if(pApp->currDutyPer < APP_MIN_DUTY_PERCEN) pApp->currDutyPer = APP_MIN_DUTY_PERCEN;
					App_SetDutyPercen(pApp->currDutyPer);
					//out_char('<');
				}
				pApp->panelLastVolt = pApp->panelVolt.realValue;
			}			
			#endif
			
			if(pApp->battVolt.realValue >= pApp->chargMaxVolt) {
				pApp->eBuckerSM = BSM_BUCKER_CHARG_VOLT_MAX;
				Timer_Stop(pApp->hTimerControl);
				Timer_StartAt(pApp->hTimerControl, pApp->chargConstVoltTime);
				LREP("\r\nMPPT VOLT CTRL -> VOLT MAX\r\n\n");
			}
			
		} else {			
			App_StopBucker(pApp);
			pApp->eBuckerSM = BSM_BUCKER_STOP;
		}
		
		break;		
		
	case BSM_BUCKER_CHARG_VOLT_MAX:
		if(pApp->eDevState == DS_DEV_RUN) {		
		#if BUCKER_CONTROL_METHOD == CONTROL_THRESHOLD
			float value = (pApp->battVolt.realValue - pApp->battLastVolt);
			if(value < 0) value = -value; 
			if(value > 10.0) {
				//out_char('.');
				// if max current is reached
				if(sApp.battVolt.realValue > BATT_VOLT_FULL_VALUE) {
					pApp->currDutyPer -= APP_STEP_DUTY_PERCEN;
					if(pApp->currDutyPer < 0) pApp->currDutyPer = 0;
					App_SetDutyPercen(pApp->currDutyPer);
				} else if(pApp->battVolt.realValue < pApp->chargMaxVolt){
					pApp->currDutyPer += APP_STEP_DUTY_PERCEN;
					if(pApp->currDutyPer > APP_MAX_DUTY_PERCEN) pApp->currDutyPer = APP_MAX_DUTY_PERCEN;
					App_SetDutyPercen(pApp->currDutyPer);
				}
				pApp->battLastVolt = pApp->battVolt.realValue;
			}
			
			if(sApp.battCurr >= pApp->chargMaxCurrent) {
				// change to const current phase
				pApp->eBuckerSM = BSM_BUCKER_CHARG_CONST_CURR;
				LREP("\r\nCONST VOLT -> CURRENT MAX -> CHANGE TO CONST CURRENT\r\n\n");				
			}
		#endif
			
		} else {			
			App_StopBucker(pApp);
			pApp->eBuckerSM = BSM_BUCKER_STOP;
		}
		break;
		
	case BSM_BUCKER_IDLE:
		break;
		
	default:
		break;
	
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
void led_cmd(int32_t argc, char **argv)
{
	if (argc == 3)
	{
		if (strcmp(argv[1], "all") == 0) {
			if (strcmp(argv[2], "on") == 0) {
				LED_OnAll();
				LREP("turn blink on all led\r\nn");
			} else if (strcmp(argv[2], "off") == 0) {
				LED_OffAll();
				LREP("turn blink off all led\r\nn");
			}
		} else {
			int value = atoi(argv[1]);		
			if(value >= 0 && value < LED_NUM_NODE) {
				if (strcmp(argv[2], "on") == 0)
				{
					LED_SetBlink(value, 1);
					LREP("set led %d blink on \r\n\n", value);
				}
				else if (strcmp(argv[2], "off") == 0)
				{
					LED_SetBlink(value, 0);
					LREP("set led %d blink off \r\n\n", value);
				}
				else
				{
					LREP("Argument not supported\r\n");
				}
			} else {
				LREP("invalid led id: %d\r\n\n", value);
			}
		}
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
void set_vusb(int32_t argc, char **argv)
{
	if (argc == 2)
	{		
		if (strcmp(argv[1], "on") == 0) {
			sApp.eDevPendCmd = CMD_ON_OFF_VUSB;
			sApp.cmdParam = 1;
			LREP("password: \r\n\n");
		} else if (strcmp(argv[1], "off") == 0) {
			sApp.eDevPendCmd = CMD_ON_OFF_VUSB;
			sApp.cmdParam = 0;
			LREP("password: \r\n\n");
		} else {
			LREP("argument not supported\r\n");
		}
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
void restart(int32_t argc, char**argv) {
	//NVIC_SystemReset();
	sApp.eDevPendCmd = CMD_RESTART;
	LREP("password: \r\n\n");
}

/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */
void help_cmd(int32_t argc, char **argv)
{
	(void)argc;
	(void)argv;

	shell_help();
}


#define SHELL_CFG_TERMINAL_HIGH         90
/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */
void clear_screen(int32_t argc, char**argv) {
    int i;

    for(i =0 ; i< SHELL_CFG_TERMINAL_HIGH; i++) {
        LREP("\r\n\n");
    }

    for(i =0 ; i< SHELL_CFG_TERMINAL_HIGH; i++) {
        LREP("\033[F");
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
void my_shell_init(void)
{
	LREP("shell init done\r\n");
}

/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */
void set_duty_cmd(int32_t argc, char **argv) {
	if (argc == 2)
	{
//		float value = atof(argv[1]);
//		if(value > 0) {
//			GPIO_SET_HIGH_CTRL_BUCK_DRV();
//		} else {
//			GPIO_SET_LOW_CTRL_BUCK_DRV();
//		}
//		
//		LREP("FTM0->MOD = %d\r\n", (int)FTM0->MOD);
//				
//		sApp.currDutyPer = value / 100.0;
//		
//		FTM_SetChannelValue(FTM0, FTM_CHANNEL_CHANNEL0, (int)((float)FTM0->MOD*sApp.currDutyPer)); 
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
void get_info(int32_t argc, char **argv) {	

	LREP("show info:\r\n ID: %d\r\n PV: %d mV\r\n PI: %d mA\r\n PP: %d mW\r\n BV: %d mV\r\n BI: %d mA\r\n\n",
			(int)sApp.id,
			(int)sApp.panelVolt.realValue,
			(int)sApp.panelCurr.realValue,
			(int)sApp.panelPower,
			(int)sApp.battVolt.realValue,
			(int)sApp.battCurr);
	
//	LREP("\r\nID: %d PV: %d mV PI: %d mA PP: %d mW BV: %d mV BI: %d mA STT: %s\r\n\n",
//			(int)sApp.id,
//			(int)sApp.panelVolt.realValue,
//			(int)sApp.panelCurr.realValue,
//			(int)sApp.panelPower,
//			(int)sApp.battVolt.realValue,
//			(int)sApp.battCurr,
//			status[sApp.eBuckerSM]);
}

/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */
void set_charge_onoff(int32_t argc, char **argv) {
	if (argc == 2)
	{		
		if (strcmp(argv[1], "on") == 0) {
			sApp.eDevPendCmd = CMD_ON_OFF_CHARGE;
			sApp.cmdParam = 1;
			LREP("password: \r\n\n");
		} else if (strcmp(argv[1], "off") == 0) {
			sApp.eDevPendCmd = CMD_ON_OFF_CHARGE;
			sApp.cmdParam = 0;
			LREP("password: \r\n\n");
		} else {
			LREP("argument not supported\r\n\n");
		}
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
void set_board_id(int32_t argc, char **argv) {
	if (argc == 2)
	{		
		float cont = atof(argv[1]);		
		sApp.eDevPendCmd = CMD_SET_BOARD_ID;
		sApp.cmdParam = cont;
		LREP("password: \r\n\n");
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
void set_float_volt(int32_t argc, char **argv) {
	if (argc == 2)
	{		
		int cont = atof(argv[1]);		
		sApp.eDevPendCmd = CMD_SET_FLOAT_VOLT;
		sApp.cmdParam = cont;
		LREP("password: \r\n\n");
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
void set_boost_volt(int32_t argc, char **argv) {
	if (argc == 2)
	{		
		float cont = atof(argv[1]);		
		sApp.eDevPendCmd = CMD_SET_BOOST_VOLT;
		sApp.cmdParam = cont;
		LREP("password: \r\n\n");
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
void set_boost_curr(int32_t argc, char **argv) {
	if (argc == 2)
	{		
		float cont = atof(argv[1]);		
		sApp.eDevPendCmd = CMD_SET_CURR_MAX;
		sApp.cmdParam = cont;
		LREP("password: \r\n\n");
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
void set_const_volt_time(int32_t argc, char **argv) {
	if (argc == 2)
	{		
		float cont = atof(argv[1]);		
		sApp.eDevPendCmd = CMD_SET_BOOST_TIME;
		sApp.cmdParam = cont;
		LREP("password: \r\n\n");
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
void set_log(int32_t argc, char **argv) {
	if (argc == 2)
	{		
		if (strcmp(argv[1], "on") == 0) {
			sApp.eDevPendCmd = CMD_SET_LOG_STT;
			sApp.cmdParam = 1;
			LREP("password: \r\n\n");
		} else if (strcmp(argv[1], "off") == 0) {
			sApp.eDevPendCmd = CMD_SET_LOG_STT;
			sApp.cmdParam = 0;
			LREP("password: \r\n\n");
		} else {
			LREP("argument not supported\r\n\n");
		}
	}
}

void get_setting(int32_t argc, char **argv) {
	
	char *stt = (sApp.eBuckerSM & DS_USER_DISABLE) == 0 ? "on" : "off";
	char *usb = sApp.vUsb == 0 ? "off" : "on";
	
	LREP("show config:\r\n bid: %d\r\n charg: %s\r\n bvolt: %d mV\r\n btime: %d min\r\n fvolt: %d mV\r\n bcurr: %d mA\r\n vusb: %s\r\n\n",			
			(int)sApp.id,
			stt,
			(int)sApp.chargMaxVolt,
			(int)(sApp.chargConstVoltTime / MILLI_SEC_ON_MINUTE),
			(int)sApp.chargFloatVolt,
			(int)sApp.chargMaxCurrent,
			usb);
			
}
/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */
void set_passwd(int32_t argc, char **argv) {
	if (argc == 2)
	{
		LREP("\r\n");
		if (strcmp(argv[1], APP_SETTING_PASSWD) == 0) {
			
			switch(sApp.eDevPendCmd) {
			case CMD_ON_OFF_CHARGE:
				if(sApp.cmdParam == 0) {
					App_SetDevState(&sApp, DS_USER_DISABLE);
					LREP("user disabled charger\r\n\n");
				} else {
					App_ClearDevState(&sApp, DS_USER_DISABLE);
					LREP("user enabled charger\r\n\n");
				}
				break;
			case CMD_ON_OFF_VUSB:
				if(sApp.cmdParam == 1) {
					GPIO_SET_HIGH_CTRL_VUSB_EN();
					LREP("user control on vusb\r\n\n");
				} else {
					GPIO_SET_LOW_CTRL_VUSB_EN();
					LREP("user control off vusb\r\n\n");
				}
				break;
			case CMD_SET_BOARD_ID:
				sApp.id = (int)sApp.cmdParam;
				LREP("user set board id = %d\r\n\n", sApp.id);
				break;
			case CMD_SET_FLOAT_VOLT:
				sApp.floatBattVolt = sApp.cmdParam;
				LREP("user set float battery voltage: %d\r\n\n", (int)sApp.floatBattVolt);
				break;
			case CMD_SET_BOOST_VOLT:
				sApp.chargMaxVolt = sApp.cmdParam;
				LREP("user set boost battery voltage: %d\r\n\n", (int)sApp.chargMaxVolt);
				break;
			case CMD_SET_CURR_MAX:
				sApp.chargMaxCurrent = sApp.cmdParam;
				LREP("user set boost battery current: %d\r\n\n", (int)sApp.chargMaxCurrent);
				break;
			case CMD_SET_BOOST_TIME:
				sApp.chargConstVoltTime = (uint32_t)(sApp.cmdParam * (float)MILLI_SEC_ON_MINUTE);
				LREP("user set boost time: %d minute\r\n\n", (int)(sApp.cmdParam));				
				break;
			case CMD_SET_LOG_STT:
				if(sApp.cmdParam == 0) {
					Timer_Stop(sApp.hTimerGui);
				} else {
					Timer_Start(sApp.hTimerGui);
				}				
				break;
				
			case CMD_RESTART:
				NVIC_SystemReset();
				break;
			default:
				LREP("no action need passwd\r\n\n");
				break;
			}
			
			sApp.eDevPendCmd = CMD_NONE;
			
		} else {
			LREP("invalid password\r\n\n");
		}
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
void Clb_TimerUpdate(uint32_t tick, void *param) {
	task_set_run(sApp.task_control, 1);	
}
/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_TimerControl(uint32_t tick, void *param) {
	if(sApp.eBuckerSM == BSM_BUCKER_IDLE) {
		LREP("\r\nSet bucker to stop\r\n");
		sApp.eBuckerSM = BSM_BUCKER_STOP;
	}
	
	if(sApp.eBuckerSM == BSM_BUCKER_CHARG_CONST_CURR) {
		LED_SetBlink(PIN_DISP_BATT_FULL_IDX, 1);
		App_SetDevState(&sApp, DS_BATT_VOLT_FULL);
	} else if(sApp.eBuckerSM == BSM_BUCKER_CHARG_VOLT_MAX) {
		LED_SetBlink(PIN_DISP_BATT_FULL_IDX, 0);
		GPIO_SET_HIGH_DISP_BATT_FULL();
		App_SetDevState(&sApp, DS_BATT_VOLT_FULL);
	}
	
	Timer_Stop(sApp.hTimerControl);
}
/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_TimerGui(uint32_t tick, void *param) {
	task_set_run(sApp.task_gui, 1);	
}
