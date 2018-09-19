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
#include <bsp.h>
#include <dispatcher.h>
#include <Timer.h>
#include <app.h>

/******************************************************************************
 * Global variables
 ******************************************************************************/

/******************************************************************************
 * Constants and macros
 ******************************************************************************/
//#define SECTOR_RW_NUM			200
//#define ADDRESS_RW				(SECTOR_RW_NUM*FLASH_SECTOR_SIZE)
/******************************************************************************
 * Local types
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes
 ******************************************************************************/

/******************************************************************************
 * Local variables
 ******************************************************************************/
const char *logo_msg = { "\r\n\n"
		"  *******  ******     ***    \r\n"
		"  **   *   **   *    *   *   \r\n"
		"  *        *          *       \r\n"
		" *****    *****        *      \r\n"
		" **       **            *     \r\n"
		" *        *        *     *     \r\n"
		"**     *  **     * *     *      \r\n"
		"*******  ********   ****        \r\n" };

int count = 0;
//uint8_t  u8DataBuff[512];
/******************************************************************************
 * Global functions
 ******************************************************************************/

void Task_Control(void *arg) {
	App_Control(&sApp);
}

void Task_Gui(void *arg) {
//	LREP("pvolt: %d pcurr %d bvolt: %d duty: 0.%03d\r\n",  
//			(int)(sApp.panelVolt.sEMA.Out), 
//			(int)(sApp.panelCurr.sEMA.Out),			
//			(int)(sApp.battVolt.sEMA.Out),
//			(int)(sApp.currDutyPer * 1000.0));

//	int watz, fwatz;
//	watz = (int)(sApp.panelPower / 1000000);
//	fwatz = (int)((sApp.panelPower - (watz * 1000000)) / 1000);
	
//	LREP("status pv: %d pvs: %d pi %d pp: %d.%d bi %d bv %d duty: 0.%03d stat: 0x%x-0x%x\r\n",
//			(int) (sApp.panelVolt.realValue), (int) (sApp.sMppt.VmppOut),
//			(int) (sApp.panelCurr.realValue),
//			(int) watz, (int)fwatz,(int) (sApp.battCurr), 
//			(int) (sApp.battVolt.realValue),
//			(int) (sApp.currDutyPer * 1000.0), (int) (sApp.eDevState), 
//			(int) sApp.eBuckerSM);
	
//	LREP("status: ID: %d ST: %d PV: %d mV PI: %d mA PP: %d mW BV: %d mV BI: %d mA\r\n",
//				(int)sApp.id,
//				(int)sApp.eBuckerSM,
//				(int)sApp.panelVolt.realValue,
//				(int)sApp.panelCurr.realValue,
//				(int)sApp.panelPower,
//				(int)sApp.battVolt.realValue,
//				(int)sApp.battCurr);
	
	LREP("status: ID: %d ST: %d PV: %d PI: %d PP: %d BV: %d BI: %d\r\n",
				(int)sApp.sCfg.id,
				(int)sApp.eBuckerSM,
				(int)sApp.panelVolt.realValue,
				(int)sApp.panelCurr.realValue,
				(int)sApp.panelPower,
				(int)sApp.battVolt.realValue,
				(int)sApp.battCurr);

	
//	LREP("status: \r\nID: %d \r\nST: %d\r\n PV: %d mV\r\nPI: %d mA\r\nPP: %d mW\r\nBV: %d mV\r\nBI: %d mA\r\n",
//				(int)sApp.id,
//				(int)sApp.eBuckerSM,
//				(int)sApp.panelVolt.realValue,
//				(int)sApp.panelCurr.realValue,
//				(int)sApp.panelPower,
//				(int)sApp.battVolt.realValue,
//				(int)sApp.battCurr);
	
//	LREP("pass\r\n\n");

	
}


#if 1
int main(void) {
	int16_t err;
	WDOG_ConfigType sWDOGConfig = {0};   
	
	BSP_Init();
	
	LREP(logo_msg);
	LREP("SOLAR CHARGER APPLICATION STARTED\r\nbuilt time " __TIME__ " " __DATE__ "\r\n\n");
		
	err = FLASH_Init(BUS_CLK_HZ);
	if(err != 0) {
		LREP("Init error 0x%x\r\n\n", err);
	}
	
	
	
	App_Init(&sApp);
			
	shell_init(cmd_table, my_shell_init);

	task_init();
	sApp.task_shell = task_create(Debug_Task,	// task function 
									NULL, 		// parameter
									TRUE);		// always run

	//sApp.task_control = task_create(Task_Control, NULL, FALSE);
	sApp.task_gui = task_create(Task_Gui, NULL, FALSE);
	sApp.task_trans = task_create(Trans_Task, &sApp.sTransObj, TRUE);
	
	ASSERT(sApp.task_trans != NULL); ASSERT(sApp.task_shell != NULL);

	LREP("firmware version %s\r\n", APP_FIRMWARE_VER);
	LREP("initilized done \r\n\n");
	LREP("type \"help\" to show support command\r\n\n");
	    
    
#ifdef TEST_FLASH
    if(*((uint8_t *)ADDRESS_RW) != (uint8_t)0x55) {
		err = FLASH_EraseSector(ADDRESS_RW);
		LREP("erase return 0x%x\r\n\n", err);
		
		for(i = 0; i < 512; i++)
		{
			u8DataBuff[i] = (uint8_t)(i + 1);
			//printf("0x%x,", (int)i);
		}
		
		u8DataBuff[0] = 0x55;
		
		LREP("\r\n\n");
		
		/* write data to erased sector */
		FLASH_Program(ADDRESS_RW, 	&u8DataBuff[0],	512 );    
		
		LREP("flash return 0x%x\r\n\n", err);
		
		
		for(i = 0; i < 512; i++ )
		{
			printf("0x%x,",*((uint8_t *)(i + ADDRESS_RW)));    	
		}
    }
#endif
	

    sWDOGConfig.sBits.bWaitEnable   = TRUE;
    sWDOGConfig.sBits.bStopEnable   = TRUE;
    sWDOGConfig.sBits.bDbgEnable    = TRUE;
    sWDOGConfig.sBits.bUpdateEnable = FALSE;
    sWDOGConfig.sBits.bDisable      = FALSE;        /* enable WDOG */
    sWDOGConfig.sBits.bClkSrc       = WDOG_CLK_INTERNAL_1KHZ;
    sWDOGConfig.u16TimeOut          = 1000;  /*< 1s */
    sWDOGConfig.u16WinTime          = 0; 
    
    WDOG_Init(&sWDOGConfig);
    
    if(WDOG_IsReset()) {
    	LREP("WDG reseted \r\n\n");
    }
    
    LREP(SHELL_PROMPT);
    
	while (1) {

#if APP_PROCESS_METHOD == APP_PROCESS_IN_BGND
		Adc_CalcRealValueBgrd(sApp.panelVolt);
		Adc_CalcRealValueBgrd(sApp.panelCurr);
		Adc_CalcRealValueBgrd(sApp.battVolt);

		sApp.panelPower = sApp.panelVolt.realValue * sApp.panelCurr.realValue;

		if(sApp.battVolt.realValue > BATT_EMPTY_VOLT_VALUE) {
			sApp.battCurr = sApp.panelPower * POWER_FACTOR / sApp.battVolt.realValue;
		} else {
			sApp.battCurr = 0;
		}

		if(sApp.battCurr < BATT_DETECT_REM_CURR_VAL &&
				sApp.eBuckerSM > BSM_BUCKER_STARTING) {
			if(sApp.eBuckerSM != BSM_BUCKER_IDLE) {
				LREP("Detect lost current start timer\r\n\n");
				sApp.eBuckerSM = BSM_BUCKER_IDLE;
				App_StopBucker(&sApp);
				Timer_StartAt(sApp.hTimerControl, BATT_DETECT_REM_WAIT_TIME);
			}
		}
#endif        
		task_main_exec();		
		WDOG_Feed();		
	}
}

#endif



