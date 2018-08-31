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

/******************************************************************************
* Local types
******************************************************************************/

/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/

/******************************************************************************
* Global functions
******************************************************************************/


void Task_Control(void *arg) {
	App_Control(&sApp);
}

void Task_Gui(void *arg) {
//	LREP("pvolt: %d pcurr %d bvolt: %d bC %d duty: 0.%03d cnt: %d stat: 0x%x\r\n",  
//			(int)(sApp.panelVolt.realValue), 
//			(int)(sApp.panelCurr.realValue),			
//			(int)(sApp.battVolt.realValue),
//			(int)(sApp.battCurr),
//			(int)(sApp.currDutyPer * 1000.0),
//			(int)(Timer_GetCurrCount(sApp.hTimerControl)),
//			(int)(sApp.eDevState));
	
	LREP("pv: %d pvs: %d pi %d pp: %d bC %d duty: 0.%03d stat: 0x%x\r\n",  
				(int)(sApp.panelVolt.realValue),
				(int)(sApp.sMppt.VmppOut),				
				(int)(sApp.panelCurr.realValue),
				(int)(sApp.sMppt.PanelPower),
				(int)(sApp.battCurr),
				(int)(sApp.currDutyPer * 1000.0),				
				(int)(sApp.eDevState));
}

int main (void)
{
    BSP_Init(); 
    App_Init(&sApp);
    LREP("\r\napplication started built time " __TIME__ " " __DATE__"\r\n\n");
    shell_init(cmd_table, my_shell_init);
    
    task_init();	
	sApp.task_shell 	= task_create(Debug_Task,	// task function 
									  NULL, 		// parameter
									  TRUE);		// always run
	sApp.task_control 	= task_create(Task_Control, NULL, FALSE);
	sApp.task_gui		= task_create(Task_Gui, NULL, FALSE);

	ASSERT(sApp.task_control != NULL);
	ASSERT(sApp.task_shell != NULL);	
	
	LREP("task create done \r\n\n");
	LREP(SHELL_PROMPT);
    while (1) {        
        Adc_CalcRealValueBgrd(sApp.panelVolt);
        Adc_CalcRealValueBgrd(sApp.panelCurr);
        Adc_CalcRealValueBgrd(sApp.battVolt);
        if(sApp.battVolt.realValue > BATT_VOLT_EMPTY_VALUE) {
			sApp.panelPower = sApp.panelVolt.realValue * sApp.panelCurr.realValue; 
			sApp.battCurr = sApp.panelPower * POWER_FACTOR / sApp.battVolt.realValue;
        } else {
        	sApp.battCurr = 0;        	
        }
        
        if(sApp.battCurr < 100 && sApp.eBuckerSM > BSM_BUCKER_STARTING) {
        	if(sApp.eBuckerSM != BSM_BUCKER_IDLE) {
        		LREP("Detect lost current start timer\r\n\n");
				sApp.eBuckerSM = BSM_BUCKER_IDLE;
				App_StopBucker(&sApp);        	
				Timer_StartAt(sApp.hTimerControl, 5000);
        	}
        }
    	task_main_exec();
    }
}



