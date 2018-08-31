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
void clear_screen(int32_t argc, char**argv);
void Clb_TimerControl(uint32_t tick, void *param);
void Clb_TimerUpdate(uint32_t tick, void *param);
void Clb_TimerGui(uint32_t tick, void *param);
void clear_screen(int32_t argc, char**argv);
void help_cmd(int32_t argc, char **argv);
void restart(int32_t argc, char**argv);
void led_cmd(int32_t argc, char **argv);
void vusb_cmd(int32_t argc, char **argv);
void set_duty_cmd(int32_t argc, char **argv);
void show_info_cmd(int32_t argc, char **argv);

/************************** Variable Definitions *****************************/
SApp sApp;

const shell_command_t cmd_table[] =
{
	{"help", 0u, 0u, help_cmd, "Display this help message", ""},
	{"led", 2u, 2u, led_cmd, "Led control", "<on|off>"},	
	{"clear", 0u, 0u, clear_screen, "clear screen", ""},
	{"restart", 0u, 0u, restart, "restart mcu", ""},
	{"duty", 1u, 1u, set_duty_cmd, "set duty cycle", ""},
	{"show", 0u, 0u, show_info_cmd, "show board info", ""},
	{"vusb", 1u, 1u, vusb_cmd, "vusb control", "<on|off>"},
	{0, 0u, 0u, 0, 0, 0}
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
	Timer_Start(pApp->hTimerGui);
	
	pApp->hTimerControl = Timer_Create(Clb_TimerControl, NULL);
	Timer_SetRate(pApp->hTimerControl, APP_CHARGE_CONST_VOLT_TIME);
	Timer_Start(pApp->hTimerControl);
	
	pApp->eDevState = 0;
	pApp->eDevState = DS_DEV_RUN | DS_BATT_VOLT_LOW | DS_PANEL_VOLT_LOW;
	
	pApp->eBuckerSM = BSM_BUCKER_STOP;		
	pApp->currDutyPer = 0;
	
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
	pApp->chargConstCurrTime 	= APP_CHARGE_CONST_CURR_TIME;
	pApp->chargConstVoltTime 	= APP_CHARGE_CONST_VOLT_TIME;
	pApp->chargMaxCurrent 		= BATT_MAX_CURRENT_VALUE;
	pApp->chargMaxVolt 			= BATT_VOLT_FULL_VALUE;
	//App_StartBucker(pApp);
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
				Timer_Stop(pApp->hTimerControl);
				Timer_StartAt(pApp->hTimerControl, pApp->chargConstCurrTime);
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
				pApp->currDutyPer += APP_STEP_DUTY_PERCEN;
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
				Timer_Stop(pApp->hTimerControl);
				Timer_StartAt(pApp->hTimerControl, pApp->chargConstCurrTime);
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
void vusb_cmd(int32_t argc, char **argv)
{
	if (argc == 2)
	{
		if (strcmp(argv[1], "on") == 0) {
			GPIO_SET_HIGH_CTRL_VUSB_EN();
			LREP("set vusb on \r\n\n");
		} else if (strcmp(argv[1], "off") == 0) {			
			GPIO_SET_LOW_CTRL_VUSB_EN();
			LREP("set vusb off \r\n\n");
		} else {
			LREP("Argument not supported\r\n");
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


#define SHELL_CFG_TERMINAL_HIGH         85
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
		float value = atof(argv[1]);
		if(value > 0) {
			GPIO_SET_HIGH_CTRL_BUCK_DRV();
		} else {
			GPIO_SET_LOW_CTRL_BUCK_DRV();
		}
		
		LREP("FTM0->MOD = %d\r\n", (int)FTM0->MOD);
		
		//LREP("value = %d\r\n", (int)(value*1000));
		
		sApp.currDutyPer = value / 100.0;
		
		FTM_SetChannelValue(FTM0, FTM_CHANNEL_CHANNEL0, (int)((float)FTM0->MOD*sApp.currDutyPer)); 
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
void show_info_cmd(int32_t argc, char **argv) {	
	LREP("pvolt adc: %d pcurr adc: %d bvolt adc: %d\r\n",  
			(int)sApp.panelVolt.avgValue, (int)sApp.panelCurr.avgValue, 
			(int)sApp.battVolt.avgValue);
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
