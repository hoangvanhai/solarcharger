/** @FILE NAME:    app.c
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
#include <utils.h>
/************************** Constant Definitions *****************************/
#define SECTOR_RW_NUM			200
#define ADDRESS_RW				(SECTOR_RW_NUM*FLASH_SECTOR_SIZE)
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
void set_config(int32_t argc, char **argv);
void set_vusbx(int32_t argc, char **argv);
void reset_flash(int32_t argc, char **argv);
void set_logx(int32_t argc, char **argv);
void cancel(int32_t argc, char **argv);
void mppt_volt(int32_t argc, char **argv);


/************************** Variable Definitions *****************************/
SApp sApp;
uint8_t  u8DataBuff[256];

const shell_command_t cmd_table[] =
{
	{"help", 	0u, 0u, help_cmd, 		"display this help message", ""},
	{"clear", 	0u, 0u, clear_screen, 	"clear screen", ""},
	{"pass", 	1u, 1u, set_passwd, 	"enter password", ""},
	{"reset", 	0u, 0u, restart, 		"reset system", ""},	
	{"vusb", 	1u, 1u, set_vusb, 		"control 5v usb port, ex: \"vusb on\" for turn on 5v usb port", "<on|off>"},
	{"charg", 	1u, 1u, set_charge_onoff, "control charger, ex: \"charg on\" for enable charger", "<on|off>"},
	{"bid", 	1u, 1u, set_board_id, 	"set this board id, ex: \"bid 10\" for set board id = 10", "<id>"},
	{"fvolt", 	1u, 1u, set_float_volt, "set float volt (mV), ex: \"fvolt 13800\" for set float volt = 13800 mV", "<volt>"},
	{"bvolt", 	1u, 1u, set_boost_volt, "set boost volt (mV), ex: \"bvolt 14200\" for set boost volt = 14200 mV", "<volt>"},
	{"bcurr", 	1u, 1u, set_boost_curr, "set boost current max (mA), ex: \"bcurr 5000\" for set boost current = 5000 mA", "<ampe>"},
	{"btime", 	1u, 1u, set_const_volt_time, "set boost time (minute), ex: \"btime 120\" for set boost time = 120 minute", "<time>"},	
	{"sinfo", 	0u, 0u, get_info, 		"show board info", ""},
	{"log", 	1u, 1u, set_log, 		"continuous log board status", "<on|off>"},	
	{"scfg", 	0u, 0u, get_setting, 	"show board setting", ""},
	{"rst", 	1u, 1u, reset_flash, 	"reserved", ""},
	/*{"config", 	8u, 8u, set_config, 	"reserved", ""},
	{"lx", 	1u, 1u, set_logx, 			"reserved", ""},	
	{"mppt", 	1u, 1u, mppt_volt, 		"set mppt volt", ""},
	{"cancel", 	0u, 0u, cancel, 		"discard pending command", ""},*/	
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
	
	SConfig cfg = {0};
	
	Timer_Init();
	
	pApp->hTimerUpdate = Timer_Create(Clb_TimerUpdate, NULL);
	Timer_SetRate(pApp->hTimerUpdate, 100);
	
	
	pApp->hTimerGui = Timer_Create(Clb_TimerGui, NULL);
	Timer_SetRate(pApp->hTimerGui, 2000);
	//Timer_Start(pApp->hTimerGui);
	
	pApp->hTimerControl = Timer_Create(Clb_TimerControl, NULL);
	Timer_SetRate(pApp->hTimerControl, APP_CHARGE_CONST_VOLT_TIME);
	//Timer_Start(pApp->hTimerControl);
	
	pApp->eDevState = 0;
	pApp->eDevState = DS_DEV_RUN | DS_BATT_VOLT_LOW | DS_PANEL_VOLT_LOW;
	
	pApp->eBuckerSM = BSM_BUCKER_STOP;		
	pApp->currDutyPer = 0;
	pApp->sCfg.id = APP_DEVICE_ID_DEFAULT;
	
	MPPT_PNO_F_init(&pApp->sMppt);

	// init input caclculating
	Adc_InitValue(pApp->battVolt,                   /* adc node */
				  0,                                /* type adc node 1 avg 0 filter  */
				  ADC_BATT_VOLT_COEFF,       		/* coefficient for calculate realvalue */
				  1000,                             /* average counter */
				  0,                                /* offset value */
				  2);          						/* cuttoff freq */
	
	Adc_InitValue(pApp->panelCurr,
				  0,
				  ADC_PANEL_CURR_COEFF,
				  100,
				  1976,	//1939,	//1963,
				  2);
	
	Adc_InitValue(pApp->panelVolt,
				  0,
				  ADC_PANEL_VOLT_COEFF,
				  2048,
				  0,
				  2);
	
	Adc_InitValue(pApp->panelAvgCurr,
				  1,
				  ADC_PANEL_CURR_COEFF,
				  3000,
				  1978,
				  2);
	
	
	pApp->panelLastVolt 			= 0;
	pApp->panelLastCurr 			= 0;
	pApp->battLastVolt 				= 0;	
	pApp->battCurr 					= 0;
	pApp->battLastCurr 				= 0;
	pApp->panelPower 				= 0;
	
	// init control value 
	pApp->sCfg.chargBoostTime 			= APP_CHARGE_CONST_VOLT_TIME;
	pApp->sCfg.chargBoostCurrent 		= BATT_BOOST_CURRENT_VALUE;
	pApp->sCfg.chargBoostVolt 			= BATT_BOOST_VOLT_VALUE;
	pApp->sCfg.chargFloatVolt			= BATT_FLOAT_VOLT_VALUE;
	pApp->sCfg.vUsb						= TRUE;
	pApp->sCfg.charg					= TRUE;
	pApp->sCfg.log						= FALSE;
	pApp->downRate						= 0;
	

	if(*((uint8_t *)ADDRESS_RW) == (uint8_t)0x55) {
		App_LoadConfig(&cfg);
	}
	
	if(pApp->sCfg.log > 0) {
		Timer_Start(pApp->hTimerGui);
	}
#if APP_PROCESS_METHOD == APP_PROCESS_IN_BGND
	Timer_Start(pApp->hTimerUpdate);
#endif
	
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
	
	static float diffval = 0;
	/* *
	 * Check panel voltage
	 */
	if(pApp->panelVolt.realValue <= PANEL_VOLT_LOW_PROTECT) {				
		App_SetDevState(pApp, DS_PANEL_VOLT_LOW);
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
	if(pApp->battVolt.realValue < BATT_EMPTY_VOLT_VALUE) {
		
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
	} else if (pApp->battVolt.realValue >= BATT_EMPTY_VOLT_VALUE && 
			pApp->battVolt.realValue < pApp->sCfg.chargBoostVolt) {
				
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
			//pApp->eBuckerSM = BSM_BUCKER_STARTING;
			pApp->eBuckerSM = BSM_BUCKER_CHARG_MPPT_VOLT_CTRL;				
			LREP("\r\nSTART BUCKER MPPT\r\n\n");
		}
		
		break;
	case BSM_BUCKER_STARTING:
		if(pApp->eDevState == DS_DEV_RUN) {
			// if max current is reached
			if(sApp.battCurr >= pApp->sCfg.chargBoostCurrent) {
				// change to const current phase
				pApp->eBuckerSM = BSM_BUCKER_CHARG_CONST_CURR;
				LREP("S I MAX -> CONST I\r\n\n");
			}

			if(pApp->battVolt.realValue >= pApp->sCfg.chargBoostVolt) {
				pApp->eBuckerSM = BSM_BUCKER_CHARG_VOLT_MAX;
				Timer_Stop(pApp->hTimerControl);
				Timer_StartAt(pApp->hTimerControl, pApp->sCfg.chargBoostTime);
				LREP("S V MAX -> CONST V %d\r\n\n", (int)pApp->sCfg.chargBoostTime);
			}
			
			// if duty is not reached max 
			if(pApp->currDutyPer < APP_MAX_DUTY_PERCEN) {
				// increment duty
				pApp->currDutyPer += APP_STEP_DUTY_PERCEN	;//0.01;
				App_SetDutyPercen(pApp->currDutyPer);
			} else { // is max duty				
				// change to mppt to get const voltage
				pApp->eBuckerSM = BSM_BUCKER_CHARG_MPPT_VOLT_CTRL;				
				LREP("S D MAX -> MPPT d: 0.%03d v %d i %d\r\n\n", 
						(int)(pApp->currDutyPer * 1000), 
						(int)pApp->battVolt.realValue, 
						(int)pApp->battCurr);
				App_StartBucker(pApp);
			}			
		} else if(pApp->eDevState == (DS_DEV_RUN | DS_PANEL_VOLT_LOW)) {
			App_StartBucker(pApp);
			pApp->eBuckerSM = BSM_BUCKER_CHARG_MPPT_VOLT_CTRL;			
			LREP("SLD->MPPT");
		} else {			
			App_StopBucker(pApp);
			pApp->eBuckerSM = BSM_BUCKER_STOP;
		}		
		
		break;
	
	case BSM_BUCKER_CHARG_CONST_CURR:
		if(pApp->eDevState == DS_DEV_RUN) {

			#if BUCKER_CONTROL_METHOD == CONTROL_THRESHOLD
			diffval = (pApp->battCurr - pApp->sCfg.chargBoostCurrent);			
			if(ABS(diffval) > APP_CONTROL_OFFSET_VALUE) {
				// if max current is reached
				if(pApp->battCurr > pApp->sCfg.chargBoostCurrent) {
					pApp->currDutyPer -= APP_STEP_DUTY_PERCEN;
					if(pApp->currDutyPer < 0) pApp->currDutyPer = 0;
					App_SetDutyPercen(pApp->currDutyPer);
				} else if(sApp.battCurr < pApp->sCfg.chargBoostCurrent){
					pApp->currDutyPer += APP_STEP_DUTY_PERCEN;
					if(pApp->currDutyPer > APP_MAX_DUTY_PERCEN) pApp->currDutyPer = APP_MAX_DUTY_PERCEN;
					App_SetDutyPercen(pApp->currDutyPer);
				}
				pApp->battLastCurr = pApp->battCurr;
			}						
			#endif
			
			if(pApp->battVolt.realValue >= pApp->sCfg.chargBoostVolt) {
				pApp->eBuckerSM = BSM_BUCKER_CHARG_VOLT_MAX;
				Timer_StartAt(pApp->hTimerControl, pApp->sCfg.chargBoostTime);
				LREP("I ->V ");
			}

			// if duty is reached max 
			if(pApp->currDutyPer >= APP_MAX_DUTY_PERCEN) {
				App_StartBucker(pApp);
				pApp->eBuckerSM = BSM_BUCKER_CHARG_MPPT_VOLT_CTRL;
				LREP("DI->MPPT ");
			}				
			
		} else if(pApp->eDevState == (DS_DEV_RUN | DS_PANEL_VOLT_LOW)) {
			App_StartBucker(pApp);
			pApp->eBuckerSM = BSM_BUCKER_CHARG_MPPT_VOLT_CTRL;			
			LREP("LI->MPPT ");
		} else {					
			App_StopBucker(pApp);
			pApp->eBuckerSM = BSM_BUCKER_STOP;
		}
		break;
		
	case BSM_BUCKER_CHARG_MPPT_VOLT_CTRL:
		{
	//		if(pApp->eDevState == DS_DEV_RUN || 
	//				pApp->eDevState == (DS_DEV_RUN | DS_PANEL_VOLT_LOW)) {		
				if(pApp->eDevState == DS_DEV_RUN) {
				pApp->sMppt.Ipv = pApp->panelCurr.realValue;
				pApp->sMppt.Vpv = pApp->panelVolt.realValue;				
				MPPT_PNO_F_MACRO(pApp->sMppt);
				
				#if BUCKER_CONTROL_METHOD == CONTROL_THRESHOLD
				diffval = (pApp->panelVolt.realValue - pApp->sMppt.VmppOut);			
				if(ABS(diffval) > 10.0) 
				{
					// if max current is reached
					if(sApp.panelVolt.realValue > pApp->sMppt.VmppOut) {
						pApp->currDutyPer += APP_STEP_DUTY_PERCEN;
						if(pApp->currDutyPer > APP_MAX_DUTY_PERCEN) pApp->currDutyPer = APP_MAX_DUTY_PERCEN;
						App_SetDutyPercen(pApp->currDutyPer);
					} else if(sApp.panelVolt.realValue < pApp->sMppt.VmppOut){
						pApp->currDutyPer -= APP_STEP_DUTY_PERCEN;
						if(pApp->currDutyPer < APP_MIN_DUTY_PERCEN) pApp->currDutyPer = APP_MIN_DUTY_PERCEN;
						App_SetDutyPercen(pApp->currDutyPer);
					}
				}			
				#endif
				
				if(pApp->battVolt.realValue >= pApp->sCfg.chargBoostVolt) {
					pApp->eBuckerSM = BSM_BUCKER_CHARG_VOLT_MAX;
					Timer_Stop(pApp->hTimerControl);
					Timer_StartAt(pApp->hTimerControl, pApp->sCfg.chargBoostTime);
					LREP("MPPT->V ");
				}
				
				if(sApp.battCurr >= pApp->sCfg.chargBoostCurrent) {
					// change to const current phase
					pApp->eBuckerSM = BSM_BUCKER_CHARG_CONST_CURR;
					LREP("MPPT->I ");				
				}
				
			} else {			
				App_StopBucker(pApp);
				pApp->eBuckerSM = BSM_BUCKER_STOP;
			}
		}
		break;		
		
	case BSM_BUCKER_CHARG_VOLT_MAX:
		if(pApp->eDevState == DS_DEV_RUN) {		
		#if BUCKER_CONTROL_METHOD == CONTROL_THRESHOLD
			diffval = (pApp->battVolt.realValue - pApp->sCfg.chargBoostVolt);
			if(ABS(diffval) > APP_CONTROL_OFFSET_VALUE) {
				if(sApp.battVolt.realValue > pApp->sCfg.chargBoostVolt) {
					pApp->currDutyPer -= APP_STEP_DUTY_PERCEN;
					if(pApp->currDutyPer < 0) pApp->currDutyPer = 0;
					App_SetDutyPercen(pApp->currDutyPer);
				} else if(pApp->battVolt.realValue < pApp->sCfg.chargBoostVolt){
					pApp->currDutyPer += APP_STEP_DUTY_PERCEN;
					if(pApp->currDutyPer > APP_MAX_DUTY_PERCEN) pApp->currDutyPer = APP_MAX_DUTY_PERCEN;
					App_SetDutyPercen(pApp->currDutyPer);
				}				
			}
			
			if(sApp.battCurr >= pApp->sCfg.chargBoostCurrent) {
				pApp->eBuckerSM = BSM_BUCKER_CHARG_CONST_CURR;
				LREP("V->I ");				
			}
		#endif
			
		} else {			
			App_StopBucker(pApp);
			pApp->eBuckerSM = BSM_BUCKER_STOP;
		}
		break;
		
	case BSM_BUCKER_CHARG_VOLT_FLOAT:
		
		if(pApp->eDevState == DS_DEV_RUN) {		
		#if BUCKER_CONTROL_METHOD == CONTROL_THRESHOLD
			diffval = (pApp->battVolt.realValue - pApp->sCfg.chargFloatVolt);
			if(ABS(diffval) > APP_CONTROL_OFFSET_VALUE) {
				if(sApp.battVolt.realValue > pApp->sCfg.chargFloatVolt) {
					pApp->currDutyPer -= APP_STEP_DUTY_PERCEN;
					if(pApp->currDutyPer < 0) pApp->currDutyPer = 0;
					App_SetDutyPercen(pApp->currDutyPer);
				} else if(pApp->battVolt.realValue < pApp->sCfg.chargFloatVolt){
					pApp->currDutyPer += APP_STEP_DUTY_PERCEN;
					if(pApp->currDutyPer > APP_MAX_DUTY_PERCEN) pApp->currDutyPer = APP_MAX_DUTY_PERCEN;
					App_SetDutyPercen(pApp->currDutyPer);
				}				
				//pApp->battLastVolt = pApp->battVolt.realValue;
			}
			
			if(sApp.battCurr >= pApp->sCfg.chargBoostCurrent) {
				// change to const current phase
				//pApp->eBuckerSM = BSM_BUCKER_CHARG_CONST_CURR;
				LREP("FV->I ");					
			}
		#endif
			
		} else {			
			App_StopBucker(pApp);
			pApp->eBuckerSM = BSM_BUCKER_STOP;
		}
		break;
		
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
			isPwdMode = TRUE;
		} else if (strcmp(argv[1], "off") == 0) {
			sApp.eDevPendCmd = CMD_ON_OFF_VUSB;
			sApp.cmdParam = 0;
			isPwdMode = TRUE;
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
	isPwdMode = TRUE;
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


#define SHELL_CFG_TERMINAL_HIGH         120
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

	LREP("status:\r\n ID: %d\r\n PV: %d mV\r\n PI: %d mA\r\n PP: %d mW\r\n BV: %d mV\r\n BI: %d mA\r\n\n",
			(int)sApp.sCfg.id,
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
			isPwdMode = TRUE;
		} else if (strcmp(argv[1], "off") == 0) {
			sApp.eDevPendCmd = CMD_ON_OFF_CHARGE;
			sApp.cmdParam = 0;
			isPwdMode = TRUE;
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
		if(cont >= 0) {
			sApp.eDevPendCmd = CMD_SET_BOARD_ID;
			sApp.cmdParam = cont;
			isPwdMode = TRUE;
		} else {
			LREP("invalid value: %d, should be > 0\r\n", (int)cont);
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
void set_float_volt(int32_t argc, char **argv) {
	if (argc == 2)
	{		
		float cont = atof(argv[1]);	
		if(cont > 5000) {
			sApp.eDevPendCmd = CMD_SET_FLOAT_VOLT;
			sApp.cmdParam = cont;
			isPwdMode = TRUE;
		} else {
			LREP("invalid value: %d, should be > 5000 && < boost volt = %d\r\n", 
					(int)cont, (int)sApp.sCfg.chargBoostVolt);
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
void set_boost_volt(int32_t argc, char **argv) {
	if (argc == 2)
	{		
		float cont = atof(argv[1]);
		if(cont > sApp.sCfg.chargFloatVolt) {
			sApp.eDevPendCmd = CMD_SET_BOOST_VOLT;
			sApp.cmdParam = cont;
			isPwdMode = TRUE;
		} else {
			LREP("invalid value: %d, should be > float volt = %d\r\n", 
					(int)cont, (int)sApp.sCfg.chargFloatVolt);
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
void set_boost_curr(int32_t argc, char **argv) {
	if (argc == 2)
	{		
		float cont = atof(argv[1]);
		if(cont > 300) {
			sApp.eDevPendCmd = CMD_SET_CURR_MAX;
			sApp.cmdParam = cont;
			isPwdMode = TRUE;
		} else {
			LREP("invalid value: %d, should be > 300\r\n", (int)cont);
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
void set_const_volt_time(int32_t argc, char **argv) {
	if (argc == 2)
	{		
		float cont = atof(argv[1]);
		if(cont > 0) {
			sApp.eDevPendCmd = CMD_SET_BOOST_TIME;
			sApp.cmdParam = cont;
			isPwdMode = TRUE;
		} else {
			LREP("invalid value: %d, should be > 0\r\n", (int)cont);
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
void set_log(int32_t argc, char **argv) {
	if (argc == 2)
	{		
		if (strcmp(argv[1], "on") == 0) {
			sApp.eDevPendCmd = CMD_SET_LOG_STT;
			sApp.cmdParam = 1;
			isPwdMode = TRUE;
		} else if (strcmp(argv[1], "off") == 0) {
			sApp.eDevPendCmd = CMD_SET_LOG_STT;
			sApp.cmdParam = 0;
			isPwdMode = TRUE;
		} else {
			LREP("argument not supported\r\n\n");
		}
	}
}

void get_setting(int32_t argc, char **argv) {
	
	char *stt = (sApp.eDevState & DS_USER_DISABLE) == 0 ? "on" : "off";
	char *usb = sApp.sCfg.vUsb == 0 ? "off" : "on";
	
	LREP("show config:\r\n bid: %d\r\n charg: %s\r\n bvolt: %d mV\r\n btime: %d min\r\n fvolt: %d mV\r\n bcurr: %d mA\r\n vusb: %s\r\n\n",			
			(int)sApp.sCfg.id,
			stt,
			(int)sApp.sCfg.chargBoostVolt,
			(int)(sApp.sCfg.chargBoostTime / MILLI_SEC_ON_MINUTE),
			(int)sApp.sCfg.chargFloatVolt,
			(int)sApp.sCfg.chargBoostCurrent,
			usb);
			
}


void set_config(int32_t argc, char **argv) {
	
	if (argc == 9)
	{		
		sApp.sCfg.id 				= atoi(argv[1]);
		sApp.sCfg.chargFloatVolt 	= atof(argv[2]);
		sApp.sCfg.chargBoostVolt 	= atof(argv[3]);
		sApp.sCfg.chargBoostCurrent = atof(argv[4]);
		sApp.sCfg.chargBoostTime 	= (uint32_t)(atof(argv[5]) * (float)MILLI_SEC_ON_MINUTE);
		
		if(strcmp(argv[6],"on") == 0) {
			sApp.sCfg.vUsb = TRUE;
			GPIO_SET_HIGH_CTRL_VUSB_EN();
		} else {
			sApp.sCfg.vUsb = FALSE;
			GPIO_SET_LOW_CTRL_VUSB_EN();
		}
		
		if(strcmp(argv[7],"on") == 0) {			
			App_ClearDevState(&sApp, DS_USER_DISABLE);
			sApp.sCfg.charg = TRUE;
		} else {
			App_SetDevState(&sApp, DS_USER_DISABLE);
			sApp.sCfg.charg = FALSE;
		}
		
		
		if(strcmp(argv[8],"on") == 0) {
			sApp.sCfg.log = TRUE;
			Timer_Start(sApp.hTimerGui);
		} else {
			sApp.sCfg.log = FALSE;
			Timer_Stop(sApp.hTimerGui);			
		}
		
		LREP("***command success\r\n\r\n");
		
		App_ResetControl(&sApp);
	}
			
}

void mppt_volt(int32_t argc, char **argv) {
	if (argc == 2)
	{		
		float cont = atof(argv[1]);
		if(cont > 1) {			
			sApp.sMppt.VmppOut = cont;
			LREP("set mppt volt = %d\r\n\n", (int)cont);
		} else {
			LREP("invalid value: %d, should be > 1\r\n", (int)cont);
		}
	}
}

void cancel(int32_t argc, char **argv) {
	
	isPwdMode = FALSE;
	sApp.eDevPendCmd = CMD_NONE;
	LREP("discard pending command\r\n");
}


void set_vusbx(int32_t argc, char **argv) {
	if (argc == 2)
	{		
		if (strcmp(argv[1], "1") == 0) {
			sApp.sCfg.vUsb = TRUE;
			GPIO_SET_HIGH_CTRL_VUSB_EN();
			LREP("***command success\r\n\r\n");
		} else if (strcmp(argv[1], "0") == 0) {
			sApp.sCfg.vUsb = FALSE;
			GPIO_SET_LOW_CTRL_VUSB_EN();
			LREP("***command success\r\n\r\n");
		} else {
			LREP("argument not supported\r\n\n");
		}
		
	}		
}


void set_logx(int32_t argc, char **argv) {
	if (argc == 2)
	{		
		if (strcmp(argv[1], "1") == 0) {
			sApp.sCfg.log = TRUE;
			Timer_Start(sApp.hTimerGui);
			LREP("***command success\r\n\r\n");
		} else if (strcmp(argv[1], "0") == 0) {
			sApp.sCfg.log = FALSE;
			Timer_Stop(sApp.hTimerGui);	
			LREP("***command success\r\n\r\n");
		} else {
			LREP("argument not supported\r\n\n");
		}
		
	}	
}


void reset_flash(int32_t argc, char **argv) {
	if (argc == 2)
	{		
		if (strcmp(argv[1], "1") == 0) {
			int err;
			err = FLASH_EraseSector(ADDRESS_RW);
			LREP("erase return 0x%x\r\n\n", err);			
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
void set_passwd(int32_t argc, char **argv) {
	static uint8_t enter_cnt = 0;
	if (argc == 2)
	{
		LREP("\r\n");
		if (strcmp(argv[1], APP_SETTING_PASSWD) == 0) {
			
			switch(sApp.eDevPendCmd) {
			case CMD_ON_OFF_CHARGE:
				if(sApp.cmdParam == 1) {
					App_ClearDevState(&sApp, DS_USER_DISABLE);
					LREP("user enabled charger\r\n\n");			
					sApp.sCfg.charg = TRUE;
				} else {
					App_SetDevState(&sApp, DS_USER_DISABLE);
					sApp.sCfg.charg = FALSE;
					LREP("user disabled charger\r\n\n");					
				}
				App_SaveConfig(&sApp.sCfg);
				break;
			case CMD_ON_OFF_VUSB:
				if(sApp.cmdParam == 1) {
					GPIO_SET_HIGH_CTRL_VUSB_EN();
					LREP("user control on vusb\r\n\n");					
					sApp.sCfg.vUsb = TRUE;
				} else {
					GPIO_SET_LOW_CTRL_VUSB_EN();
					LREP("user control off vusb\r\n\n");
					sApp.sCfg.vUsb = FALSE;
				}
				App_SaveConfig(&sApp.sCfg);
				break;
			case CMD_SET_BOARD_ID:
				sApp.sCfg.id = (int)sApp.cmdParam;
				LREP("user set board id = %d\r\n\n", sApp.sCfg.id);	
				App_SaveConfig(&sApp.sCfg);
				break;
			case CMD_SET_FLOAT_VOLT:				
				sApp.sCfg.chargFloatVolt = sApp.cmdParam;
				LREP("user set float battery voltage: %d\r\n\n", (int)sApp.sCfg.chargFloatVolt);
				App_ResetControl(&sApp);
				App_SaveConfig(&sApp.sCfg);
				break;
			case CMD_SET_BOOST_VOLT:
				sApp.sCfg.chargBoostVolt = sApp.cmdParam;
				LREP("user set boost battery voltage: %d\r\n\n", (int)sApp.sCfg.chargBoostVolt);				
				App_ResetControl(&sApp);
				App_SaveConfig(&sApp.sCfg);
				break;
			case CMD_SET_CURR_MAX:
				sApp.sCfg.chargBoostCurrent = sApp.cmdParam;
				LREP("user set boost battery current: %d\r\n\n", (int)sApp.sCfg.chargBoostCurrent);
				App_ResetControl(&sApp);
				App_SaveConfig(&sApp.sCfg);
				break;
			case CMD_SET_BOOST_TIME:
				sApp.sCfg.chargBoostTime = (uint32_t)(sApp.cmdParam * (float)MILLI_SEC_ON_MINUTE);				
				LREP("user set boost time: %d minute\r\n\n", (int)(sApp.cmdParam));
				App_ResetControl(&sApp);
				App_SaveConfig(&sApp.sCfg);
				break;
			case CMD_SET_LOG_STT:
				if(sApp.cmdParam == 0) {
					Timer_Stop(sApp.hTimerGui);
					sApp.sCfg.log = FALSE;
				} else {
					Timer_Start(sApp.hTimerGui);
					sApp.sCfg.log = TRUE;
				}		
				App_SaveConfig(&sApp.sCfg);
				break;
				
			case CMD_RESTART:
				NVIC_SystemReset();
				break;
			default:
				LREP("no action need passwd\r\n\n");
				break;
			}
			
			sApp.eDevPendCmd = CMD_NONE;
			isPwdMode = FALSE;
			enter_cnt = 0;
		} else {
			LREP("invalid password\r\n\n");
			if(enter_cnt++ > 2) {
				enter_cnt = 0;
				isPwdMode = FALSE;
				sApp.eDevPendCmd = CMD_NONE;
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
		GPIO_SET_LOW_DISP_BATT_FULL();
	}
	
	
	if(sApp.eBuckerSM == BSM_BUCKER_CHARG_VOLT_MAX) {
		GPIO_SET_HIGH_DISP_BATT_FULL();
		GPIO_SET_LOW_DISP_BATT_CHARG();
		LREP("batt full change to float volt\r\n\n");
		sApp.eBuckerSM = BSM_BUCKER_CHARG_VOLT_FLOAT;
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


/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */
void UART_HandleInt(UART_Type *pUART) {
	
	(void)pUART;
	//uint8_t u8Port;
	//(void)u8Port;
    volatile uint8_t read_temp = 0;
    
    //u8Port = ((uint32_t)pUART-(uint32_t)UART0)>>12;
    
    /* check overrun flag */
    if(UART_CheckFlag(pUART,UART_FlagOR)) {
        read_temp = UART_ReadDataReg(pUART);
        if(pUART == UART1)
        	PushCommand(read_temp);
    }  else if (UART_CheckFlag(pUART, UART_FlagRDRF)) {
    	read_temp = UART_ReadDataReg(pUART);
    	if(pUART == UART1)
    		PushCommand(read_temp);    	
    }
        
    
    
    //if(UART_IsRxBuffFull(pUART))
}
/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */
void PIT0_HandleInt(void) {	
	
	//GPIO_SET_HIGH_DISP_BATT_EMPTY();
	
	ADC_SetChannel(ADC,ADC_CHANNEL_AD8);
	ADC_SetChannel(ADC,ADC_CHANNEL_AD10);
	ADC_SetChannel(ADC,ADC_CHANNEL_AD11);
	    
	sSysTick.u32SystemTickCount++;
	Timer_Update();
    
#if APP_PROCESS_METHOD == APP_PROCESS_IN_ISR
	Adc_CalcRealValueIsr(sApp.panelVolt, 	sApp.pvAdcValue);
	Adc_CalcRealValueIsr(sApp.panelCurr, 	sApp.piAdcValue);
	Adc_CalcRealValueIsr(sApp.battVolt, 	sApp.bvAdcValue);
	Adc_CalcRealValueIsr(sApp.panelAvgCurr,	sApp.piAdcValue);
	
    sApp.panelPower = sApp.panelVolt.realValue * sApp.panelCurr.realValue;
    if(sApp.battVolt.realValue > BATT_EMPTY_VOLT_VALUE) {			 
		sApp.battCurr = sApp.panelPower * POWER_FACTOR / sApp.battVolt.realValue;		
    } else {
    	sApp.battCurr = 0;        	
    }
           
    sApp.downRate--;
    if(sApp.downRate <= 0) {
    	sApp.panelPower = sApp.panelVolt.realValue * sApp.panelAvgCurr.realValue;
    	if(sApp.battVolt.realValue > BATT_EMPTY_VOLT_VALUE) {			 
			sApp.battCurrAvg = sApp.panelPower * POWER_FACTOR / sApp.battVolt.realValue;		
		} else {
			sApp.battCurrAvg = 0;        	
		}
		if(sApp.eBuckerSM != BSM_BUCKER_IDLE && 
			sApp.eBuckerSM != BSM_BUCKER_STOP && 
			sApp.battCurrAvg < BATT_DETECT_REM_CURR_VAL) {		
			LREP("BI %d\r\n\n", (int)sApp.battCurrAvg);
			sApp.eBuckerSM = BSM_BUCKER_IDLE;
			App_StopBucker(&sApp);
			GPIO_SET_LOW_DISP_BATT_FULL();
			Timer_StartAt(sApp.hTimerControl, BATT_DETECT_REM_WAIT_TIME);		
		}	    
		sApp.downRate = 6000;
    }
    		
	App_Control(&sApp);
#endif
	    
#if BUCKER_CONTROL_METHOD == CONTROL_PID
    
    switch(sApp.eBuckerSM) {
    case BSM_BUCKER_CHARG_CONST_CURR:
    	PID_ProcessM(&sApp.sPid, 			
    				BATT_BOOST_CURRENT_VALUE, 		// set point
					sApp.battCurr);			  		// feedback
    	sApp.currDutyPer = sApp.sPid.PIDOut;
    	if(sApp.currDutyPer > APP_MAX_DUTY_PERCEN) sApp.currDutyPer = APP_MAX_DUTY_PERCEN;
    	if(sApp.currDutyPer < APP_MIN_DUTY_PERCEN) sApp.currDutyPer = APP_MIN_DUTY_PERCEN;
    	App_SetDutyPercen(sApp.currDutyPer);
    	break;
    case BSM_BUCKER_CHARG_MPPT_VOLT_CTRL:
    	PID_ProcessM(&sApp.sPid, 
    					sApp.sMppt.VmppOut, 		// set point
    					sApp.panelVolt.realValue);  // feedback	
		sApp.currDutyPer = sApp.sPid.PIDOut;		
		if(sApp.currDutyPer > APP_MAX_DUTY_PERCEN) sApp.currDutyPer = APP_MAX_DUTY_PERCEN;
		if(sApp.currDutyPer < APP_MIN_DUTY_PERCEN) sApp.currDutyPer = APP_MIN_DUTY_PERCEN;
		App_SetDutyPercen(1 - sApp.currDutyPer);		
    	break;
    	
    case BSM_BUCKER_CHARG_VOLT_MAX:
    	PID_ProcessM(&sApp.sPid, 
    				BATT_BOOST_VOLT_VALUE, 			// set point
					sApp.battVolt.realValue);		// feedback
    	sApp.currDutyPer = sApp.sPid.PIDOut;
    	if(sApp.currDutyPer > APP_MAX_DUTY_PERCEN) sApp.currDutyPer = APP_MAX_DUTY_PERCEN;
    	if(sApp.currDutyPer < APP_MIN_DUTY_PERCEN) sApp.currDutyPer = APP_MIN_DUTY_PERCEN;
    	App_SetDutyPercen(sApp.currDutyPer);
    	break;
    	
    default:
    	break;
    }          
#endif
        
	
    LED_ActAll();
    
    //GPIO_SET_LOW_DISP_BATT_EMPTY();
}

/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */
void ADC_HandleInt(void) {
    //while(!ADC_IsFIFOEmptyFlag(ADC)) // should not use while in ISR, can be cause 
									   // program hanged
	
    {
    	//GPIO_SET_HIGH_DISP_BATT_FULL();
#if APP_PROCESS_METHOD == APP_PROCESS_IN_BGND
    	Adc_PushAdcValue(sApp.panelVolt, 	ADC_ReadResultReg(ADC));
    	Adc_PushAdcValue(sApp.panelCurr, 	ADC_ReadResultReg(ADC));
    	Adc_PushAdcValue(sApp.battVolt, 	ADC_ReadResultReg(ADC));
#elif APP_PROCESS_METHOD == APP_PROCESS_IN_ISR
		sApp.pvAdcValue = ADC_ReadResultReg(ADC);
		sApp.piAdcValue = ADC_ReadResultReg(ADC);
		sApp.bvAdcValue = ADC_ReadResultReg(ADC);
#else
#error must define process method
#endif
    	//GPIO_SET_LOW_DISP_BATT_FULL();
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
void App_LoadConfig(SConfig *pHandle) {
	
	SConfig cfg;
	
	uint32_t idx = 0;
	for(idx = 0; idx < sizeof(SConfig); idx++) {
		u8DataBuff[idx] = *((uint8_t *)(idx + 1 + ADDRESS_RW));
	}
	
	memcpy((uint8_t*)(&cfg), u8DataBuff, sizeof(SConfig));
	
	
	sApp.sCfg.id 				= cfg.id;
	sApp.sCfg.vUsb 				= cfg.vUsb;
	sApp.sCfg.log 				= cfg.log;
	sApp.sCfg.charg 			= cfg.charg;
	sApp.sCfg.chargBoostTime 	= cfg.chargBoostTime;
	sApp.sCfg.chargBoostCurrent = cfg.chargBoostCurrent;
	sApp.sCfg.chargBoostVolt 	= cfg.chargBoostVolt;
	sApp.sCfg.chargFloatVolt 	= cfg.chargFloatVolt;
	
	/*
	LREP("load cfg: id: %d - vusb: %d - log: %d - charg: %d - "
			"btime: %d - bcurr: %d - bvolt: %d - fvolt: %d\r\n", 				
			(int)sApp.sCfg.id,
			(int)sApp.sCfg.vUsb,
			(int)sApp.sCfg.log,
			(int)sApp.sCfg.charg,
			(int)(sApp.sCfg.chargBoostTime),
			(int)sApp.sCfg.chargBoostCurrent,
			(int)sApp.sCfg.chargBoostVolt,
			(int)sApp.sCfg.chargFloatVolt);
	 */
}


/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */
uint16_t App_SaveConfig(SConfig *pHandle) {
	uint16_t err = 0;
	uint32_t i, max_try = 4;
	uint8_t *data = (uint8_t*)pHandle;
	uint16_t size = sizeof(SConfig);
	//LREP("size cfg = %d\r\n\n", size);
	err = FLASH_EraseSector(ADDRESS_RW);
	
	if(err != 0) {
		LREP("erase return 0x%x\r\n\n", err);		
	}
	
	for(i = 1; i < size + 1 ; i++) {
		u8DataBuff[i] = *((uint8_t*)(data+i-1));	
	}
	
	u8DataBuff[0] = 0x55;
	err = 0;
	
	err = FLASH_Program(ADDRESS_RW, &u8DataBuff[0],	size + 1);
	err = FLASH_Program(ADDRESS_RW, &u8DataBuff[0],	size + 1);
	
//	do {
//		err = FLASH_Program(ADDRESS_RW, &u8DataBuff[0],	size + 1);
//		//LREP("retry ... \r\n\n");
//		max_try--;
//	} while(err != 0 && max_try > 0);
	    	
	if(err != 0) {
		LREP("program return 0x%x\r\n\n", err);
	}
	
	return err;
	
}












