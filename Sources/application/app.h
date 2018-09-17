/*
 * app.h
 *
 *  Created on: Aug 22, 2018
 *      Author: MSI
 */

#ifndef APP_H_
#define APP_H_
#include "MPPT_PNO_F.h"
#include "MATH_EMAVG_IQ_C.h"
#include <stdint.h>
#include <console.h>
#include <Timer.h>
#include <app_cfg.h>
#include <dispatcher.h>
#include <bsp.h>
#include <pid.h>

#define ADC_NUM_SAMP_HOLD       10

typedef enum EDeviceState_ {    
	DS_DEV_RUN			 = 0x0001,
    DS_PANEL_VOLT_LOW    = 0x0002,
    DS_PANEL_VOLT_HIGH   = 0x0004,
    DS_BATT_VOLT_LOW     = 0x0008,
    DS_BATT_VOLT_FULL    = 0x0010,
    DS_BATT_VOLT_EMPTY	 = 0x0020,
    DS_PANEL_CURR_HIGH	 = 0x0040,
    DS_BATT_CURR_ZERO	 = 0x0080,
    DS_USER_DISABLE		 = 0x0100,
    DS_ERR_UNKNOWN       = 0x0000
}EDeviceState;

typedef enum EBuckerSM_ {
    BSM_BUCKER_STOP = 0,
    BSM_BUCKER_STARTING,
    BSM_BUCKER_CHARG_CONST_CURR,
    BSM_BUCKER_CHARG_MPPT_VOLT_CTRL,
    BSM_BUCKER_CHARG_VOLT_MAX,
    BSM_BUCKER_CHARG_VOLT_FLOAT,
    BSM_BUCKER_IDLE,
    BSM_BUCKER_MAX,
}EBuckerSM;


typedef enum EDevCommand_ {
	CMD_NONE = 0,
	CMD_ON_OFF_CHARGE,
	CMD_ON_OFF_VUSB,
	CMD_SET_BOARD_ID,
	CMD_SET_FLOAT_VOLT,
	CMD_SET_BOOST_VOLT,
	CMD_SET_CURR_MAX,
	CMD_SET_BOOST_TIME,
	CMD_GET_BOARD_STT,
	CMD_SET_LOG_STT,
	CMD_RESTART
}EDevCmd;

typedef struct SAdcValue_{
    uint8_t         type;
    uint8_t         isSmpWait;
    uint8_t         isLocked;
    uint8_t         smpNum;
    uint16_t        smpQueue[ADC_NUM_SAMP_HOLD];
    uint16_t        count;
    uint16_t        maxCount;
    uint32_t        totalValue;
    uint16_t        avgValue;
    uint16_t        currValue;
    uint16_t        offset;
    float           realValue;
    float           coeff;
    SMATH_EMAVG_IQ_C sEMA;
}SAdcValue;


typedef struct SConfig_ {
	uint8_t			id;
	uint8_t			vUsb;
	uint8_t			log;
	uint8_t			charg;
	uint32_t		chargBoostTime;
	float			chargBoostCurrent;
	float			chargBoostVolt;
	float			chargFloatVolt;	
}SConfig;

typedef struct SApp_ {	
	// state
	EDeviceState	eDevState;
	EBuckerSM		eBuckerSM;
	
	// controller
	MPPT_PNO_F		sMppt;	
	//SPID			sPid;

	// measurement
	float			panelPower;
	SAdcValue		panelVolt;	
	float			panelLastVolt;
	SAdcValue		panelCurr;
	float			panelLastCurr;
	SAdcValue		battVolt;
	SAdcValue		panelAvgCurr;
	float			battLastVolt;
	float			battCurr;
	float			battLastCurr;
	float			battCurrAvg;
	uint16_t 		pvAdcValue;
	uint16_t 		piAdcValue;
	uint16_t 		bvAdcValue;	
	
	// control
	void 			*hTimerControl;
	void 			*hTimerUpdate;
	void 			*hTimerGui;
	task_handle 	task_shell;	
	task_handle 	task_control;
	task_handle		task_gui;
	
	float			currDutyPer;
	
	// setting
	SConfig			sCfg;
	
	EDevCmd			eDevPendCmd;
	float			cmdParam;
	int16_t			downRate;		
}SApp;


#define App_SetDevState(pApp, state)    (pApp)->eDevState |= (state)
#define App_ClearDevState(pApp, state)  (pApp)->eDevState &= ~(state)

#define Adc_PushAdcValue(adc, value) {                                                          \
                                            if(adc.isLocked == FALSE) {                         \
                                                adc.isLocked = TRUE;                            \
                                                adc.smpQueue[adc.smpNum++] = value;             \
                                                if(adc.smpNum >= ADC_NUM_SAMP_HOLD) {           \
                                                    adc.smpNum = 0;                             \
                                                }                                               \
                                                adc.isSmpWait = TRUE;                           \
                                                adc.isLocked = FALSE;                           \
                                                }                                               \
                                            }

#define Adc_CalcRealValueIsr(adc, value)    { 													\
                                            if(adc.type == 0) { 								\
                                                if(value < adc.offset) adc.sEMA.In = 0; 		\
                                                else adc.sEMA.In = (uint16_t)(value - adc.offset); \
                                                MATH_EMAVG_IQ_C(adc.sEMA); 						\
                                                adc.realValue = adc.coeff * adc.sEMA.Out; 		\
                                            } else { 											\
                                                adc.count++; 									\
                                                adc.currValue = value;  						\
                                                adc.totalValue += value; 						\
                                                if(adc.count >= adc.maxCount) 					\
                                                { 												\
                                                    adc.avgValue = (adc.totalValue / adc.maxCount); \
                                                    if(adc.avgValue <= adc.offset) adc.avgValue = 0; \
                                                    else adc.avgValue -= adc.offset; 			\
                                                    adc.realValue = adc.avgValue * adc.coeff; 	\
                                                    adc.count = 0; 								\
                                                    adc.totalValue = 0;							\
                                                } 												\
                                            } 													\
                                            }


#define Adc_CalcRealValueBgrd(adc)       { \
                                            if((adc.isSmpWait == TRUE) && (adc.isLocked == FALSE)) {            \
                                            uint8_t i = 0;                                                  \
                                            adc.isLocked = TRUE;                                            \
                                            while(i < adc.smpNum) {                                         \
                                                if(adc.type == 0) {                                         \
                                                    if(adc.smpQueue[i] <= adc.offset) adc.sEMA.In = 0;      \
                                                    else adc.sEMA.In = adc.smpQueue[i] - adc.offset; \
                                                    MATH_EMAVG_IQ_C(adc.sEMA);                              \
                                                    adc.realValue = adc.coeff * adc.sEMA.Out; \
                                                } else {                                    \
                                                    adc.count++;                            \
                                                    adc.currValue = adc.smpQueue[i];                  \
                                                    adc.totalValue += adc.smpQueue[i];                \
                                                    if(adc.count >= adc.maxCount) {         \
                                                        adc.avgValue = (adc.totalValue / adc.maxCount); \
                                                        if(adc.avgValue <  adc.offset) adc.avgValue = 0; \
                                                        else adc.avgValue -= adc.offset; \
                                                        adc.realValue = (float)adc.avgValue * adc.coeff; \
                                                        adc.count = 0; \
                                                        adc.totalValue = 0;\
                                                    }   \
                                                }       \
                                                i++;    \
                                            } \
                                            adc.smpNum = 0;             \
                                            adc.isSmpWait = FALSE;      \
                                            adc.isLocked = FALSE;       \
                                            } }

#define Adc_InitValue(adc, typ, coe, cnt, offs, fc)          {      \
                                            adc.type = typ;       \
                                            adc.isSmpWait = FALSE; \
                                            adc.isLocked = FALSE; \
                                            adc.smpNum = 0;       \
                                            adc.count = 0;        \
                                            adc.currValue = 0;    \
                                            adc.totalValue = 0;   \
                                            adc.maxCount = cnt;   \
                                            adc.avgValue = 0;     \
                                            adc.offset = offs;    \
                                            adc.realValue = 0;    \
                                            adc.coeff = coe;      \
                     MATH_EMAVG_IQ_C_INIT(adc.sEMA, (float)(2*PI* (fc) / (float)(1000))); \
                                            }

#define App_SetDutyPercen(percen)			{ 	\
												FTM_SetChannelValue(FTM0, FTM_CHANNEL_CHANNEL0, (int)((float)FTM0->MOD*percen));}

#define App_StartBucker(pApp)				{	\
												pApp->currDutyPer = 0.15; 				\
												App_SetDutyPercen(pApp->currDutyPer);	\
												GPIO_SET_HIGH_CTRL_BUCK_DRV(); 			\
												GPIO_SET_HIGH_DISP_BATT_CHARG(); 			\
											}

#define App_StopBucker(pApp)				{	\
												(pApp)->currDutyPer = 0.0; 				\
												App_SetDutyPercen((pApp)->currDutyPer);	\
												GPIO_SET_LOW_CTRL_BUCK_DRV(); 	\
												GPIO_SET_LOW_DISP_BATT_CHARG(); 	\
												ASSERT(0);	\
											}

#define App_ResetSpInfo(pApp)				{ \
												(pApp)->sSPInfo.currCurr = 0;  (pApp)->sSPInfo.currVolt = 0; \
												(pApp)->sSPInfo.currPower = 0; \
											}

#define App_ResetControl(pApp)				{ \
												(pApp)->eBuckerSM = BSM_BUCKER_STOP; \
												Timer_Stop((pApp)->hTimerControl); \
												App_StopBucker(pApp); \
											}

#define putch(ch)							{\
												UART1->D = (ch); \
											}

/*
#define App_PidProcess(pApp, measVoltage) { 						\
											PID_ProcessM(&((pApp)->sPid), (pApp)->sMppt.VmppOut, measVoltage);	\
											(pApp)->currDutyPer = (pApp)->sPid.PIDOut; \    
											App_SetDutyPercen((pApp)->currDutyPer); }
*/

void App_Init(SApp *pApp);
void App_Control(SApp *pApp);
void App_ProcessInput(SApp *pApp);
void App_LoadConfig(SConfig *pHandle);
uint16_t App_SaveConfig(SConfig *pHandle);


void my_shell_init();
extern const shell_command_t cmd_table[];
extern SApp sApp;
#endif /* APP_H_ */
