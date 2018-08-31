/*
 * bsp.c
 *
 *  Created on: Aug 22, 2018
 *      Author: MSI
 */

#include <bsp.h>
#include <console.h>
#include <Timer.h>
#include <app.h>
#include <hardware_profile.h>
#include <MKE06Z4.h>
#include <MPPT_PNO_F.h>
#include <led.h>


uint32_t		GPIO_Pin[] = {
		PIN_DISP_RUNNING,
		PIN_DISP_BATT_LOW,
		PIN_DISP_BATT_CHARG,
		PIN_DISP_BATT_FULL,
		PIN_DISP_BATT_EMPTY,
		PIN_CTRL_BUCK_DRV,
		PIN_CTRL_VUSB_EN,
		PIN_CTRL_LOAD_EN,
		PIN_CTRL_U1_16,
		PIN_CTRL_U1_48,
		GPIO_PIN_MAX,
};


void BSP_Init() {
	//DisableInterrupts;
	sysinit();
	BSP_InitPwm();
	BSP_InitUart(115200);
	BSP_InitPIT0(APP_TIMER_FREQ);
	//BSP_InitPIT1(ADC_READ_FREQ);	
	BSP_InitAdc();
	BSP_InitGpio();
	//EnableInterrupts;
}

void BSP_InitAdc() {
	ADC_ConfigType  sADC_Config = {{0},0};
    /* initiaze ADC module */	
    sADC_Config.u8ClockDiv = ADC_ADIV_DIVIDE_4;
    sADC_Config.u8ClockSource = CLOCK_SOURCE_BUS_CLOCK;
    sADC_Config.u8Mode = ADC_MODE_12BIT;
    sADC_Config.sSetting.bIntEn = 1;
    sADC_Config.u8FiFoLevel = ADC_FIFO_LEVEL3;
    ADC_SetCallBack(ADC_HandleInt);
    ADC_Init( ADC, &sADC_Config);    
}


void BSP_InitPIT0(uint32_t freq) {
    PIT_ConfigType  sPITConfig0;    
    
    PIT_ConfigType  *pPIT_Config0   = &sPITConfig0; 
    pPIT_Config0->u32LoadValue      = 12250;	//24500;	//BUS_CLK_HZ / freq;	//0xF423F;
    pPIT_Config0->bFreeze           = FALSE;
    pPIT_Config0->bModuleDis        = FALSE;    /*!< enable PIT module */ 
    pPIT_Config0->bInterruptEn      = TRUE;
    pPIT_Config0->bChainMode        = TRUE;
    pPIT_Config0->bTimerEn          = TRUE; 
    
    PIT_Init(PIT_CHANNEL0, pPIT_Config0);                     
    PIT_SetCallback(PIT_CHANNEL0, PIT0_HandleInt);
    NVIC_EnableIRQ(PIT_CH0_IRQn);
}


void BSP_InitPIT1(uint32_t freq) {
    PIT_ConfigType  sPITConfig1;    
    
    PIT_ConfigType  *pPIT_Config1   = &sPITConfig1; 
    pPIT_Config1->u32LoadValue      = 1;	//	BUS_CLK_HZ / freq;
    pPIT_Config1->bFreeze           = FALSE;
    pPIT_Config1->bModuleDis        = FALSE;    /*!< enable PIT module */ 
    pPIT_Config1->bInterruptEn      = TRUE;
    pPIT_Config1->bChainMode        = TRUE;
    pPIT_Config1->bTimerEn          = TRUE; 
    
    PIT_Init(PIT_CHANNEL1, pPIT_Config1);           
    PIT_SetCallback(PIT_CHANNEL1, PIT1_HandleInt);
    NVIC_EnableIRQ(PIT_CH1_IRQn);
}


void BSP_InitUart(uint32_t bitrate) {
	UART_ConfigType sConfig;
	SIM_RemapUART1ToPTF_2_3();
    sConfig.u32SysClkHz = BUS_CLK_HZ;
    sConfig.u32Baudrate = UART_PRINT_BITRATE;
    
    UART_Init(UART1,&sConfig); 
    UART_EnableRx(UART1);
    UART_EnableRxBuffFullInt(UART1);    
    UART_SetCallback(UART_HandleInt);
    NVIC_EnableIRQ(UART1_IRQn);
}


void BSP_InitPwm() {

//    SIM->PINSEL |= SIM_PINSEL_FTM0PS0_MASK;
//    SIM->PINSEL |= SIM_PINSEL_FTM0PS1_MASK;
    
	/*
	 * @Note: fixed lib with modified bit set/clear at bit position: 7/8
	 */
    SIM_RemapFTM0CH0ToPTB2();
    //SIM_RemapFTM0CH1ToPTB3();
    
    /* FTM2 is set as edge aligned pwm mode, high true pulse */
    FTM_PWMInit(FTM0, FTM_PWMMODE_EDGEALLIGNED, FTM_PWM_HIGHTRUEPULSE); 
    /* update MOD value */
    FTM_SetModValue(FTM0, 250);	//9999);
    /* set clock source, start counter */
    FTM_ClockSet(FTM0, FTM_CLOCK_SYSTEMCLOCK, FTM_CLOCK_PS_DIV1);  

    FTM_SetChannelValue(FTM0, FTM_CHANNEL_CHANNEL0, 0);
    //FTM_SetChannelValue(FTM0, FTM_CHANNEL_CHANNEL1, 0); 
}




void BSP_InitGpio() {
	GPIO_PinInit(PIN_DISP_BATT_EMPTY, 		GPIO_PinOutput);
	GPIO_PinInit(PIN_DISP_BATT_LOW, 		GPIO_PinOutput);
	GPIO_PinInit(PIN_DISP_BATT_CHARG, 		GPIO_PinOutput);
	GPIO_PinInit(PIN_DISP_BATT_FULL, 		GPIO_PinOutput);
	GPIO_PinInit(PIN_DISP_RUNNING, 			GPIO_PinOutput);
	GPIO_PinInit(PIN_CTRL_BUCK_DRV,			GPIO_PinOutput);
	GPIO_PinInit(PIN_CTRL_VUSB_EN,			GPIO_PinOutput);
	GPIO_PinInit(PIN_CTRL_LOAD_EN,			GPIO_PinOutput);
	GPIO_PinInit(PIN_CTRL_U1_16,			GPIO_PinOutput);
	GPIO_PinInit(PIN_CTRL_U1_48,			GPIO_PinOutput);
	
	
	GPIO_SET_HIGH_RUN();
	GPIO_SET_LOW_DISP_BATT_LOW();
	GPIO_SET_LOW_DISP_BATT_CHARG();
	GPIO_SET_LOW_DISP_BATT_FULL();
	GPIO_SET_LOW_DISP_BATT_EMPTY();
	GPIO_SET_LOW_CTRL_BUCK_DRV();
	GPIO_SET_LOW_CTRL_VUSB_EN();
	GPIO_SET_LOW_CTRL_LOAD_EN();
	GPIO_SET_LOW_CTRL_U1_16();
	GPIO_SET_LOW_CTRL_U1_48();
	
	
	LED_InitAll();
}

void UART_HandleInt(UART_Type *pUART) {
	
	uint8_t u8Port;
	(void)u8Port;
    volatile uint8_t read_temp = 0;
    
    u8Port = ((uint32_t)pUART-(uint32_t)UART0)>>12;
    
    /* check overrun flag */
    if(UART_CheckFlag(pUART,UART_FlagOR)) {
        read_temp = UART_ReadDataReg(pUART);     
    } else if(UART_CheckFlag(pUART, UART_FlagRDRF)) {
    	read_temp = UART_ReadDataReg(pUART);
    	PushCommand(read_temp);    	
    }
}

void PIT0_HandleInt(void) {	
	sSysTick.u32SystemTickCount++;
	Timer_Update();
    ADC_SetChannel(ADC,ADC_CHANNEL_AD8);
    ADC_SetChannel(ADC,ADC_CHANNEL_AD10);
    ADC_SetChannel(ADC,ADC_CHANNEL_AD11);
    
#if BUCKER_CONTROL_METHOD == CONTROL_PID
    
    switch(sApp.eBuckerSM) {
    case BSM_BUCKER_CHARG_CONST_CURR:
    	PID_ProcessM(&sApp.sPid, 			
    				BATT_MAX_CURRENT_VALUE, 		// set point
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
    				BATT_VOLT_FULL_VALUE, 			// set point
					sApp.battVolt.realValue);		// feedback
    	sApp.currDutyPer = sApp.sPid.PIDOut;
    	if(sApp.currDutyPer > APP_MAX_DUTY_PERCEN) sApp.currDutyPer = APP_MAX_DUTY_PERCEN;
    	if(sApp.currDutyPer < APP_MIN_DUTY_PERCEN) sApp.currDutyPer = APP_MIN_DUTY_PERCEN;
    	App_SetDutyPercen(sApp.currDutyPer);
    	break;
    	
    default:
    	break;
    }
          
#elif 	BUCKER_CONTROL_METHOD == CONTROL_THRESHOLD
//    if(sApp.eBuckerSM == BSM_BUCKER_CHARG_MPPT_VOLT_CTRL) {
//    	sApp.sMppt.Ipv = sApp.panelCurr.realValue;
//    	sApp.sMppt.Vpv = sApp.panelVolt.realValue;				
//    	MPPT_PNO_F_MACRO(sApp.sMppt);
//    }
#endif
    
    
    
    LED_ActAll();
}


void PIT1_HandleInt(void) {
    ADC_SetChannel(ADC,ADC_CHANNEL_AD8);
    ADC_SetChannel(ADC,ADC_CHANNEL_AD10);
    ADC_SetChannel(ADC,ADC_CHANNEL_AD11);
}


uint16_t u16ChV_old, u16ChV_new;

void FTM2_HandleInt(void) {

    /* clear the flag */
    FTM_ClrOverFlowFlag(FTM2);
}



void FTM0_HandleInt(void)
{    
    /* clear the flag */
    FTM_ClrOverFlowFlag(FTM0);   
}


void ADC_HandleInt(void) {
    //while(!ADC_IsFIFOEmptyFlag(ADC)) // should not use while in ISR, can be cause 
									   // program hanged
    {       
    	Adc_PushAdcValue(sApp.panelVolt, 	ADC_ReadResultReg(ADC));
    	Adc_PushAdcValue(sApp.panelCurr, 	ADC_ReadResultReg(ADC));
    	Adc_PushAdcValue(sApp.battVolt, 	ADC_ReadResultReg(ADC));
    }
}


void BSP_InitGpioPin(uint32_t pin) {
	GPIO_PinInit(GPIO_Pin[pin], 		GPIO_PinOutput);
}


void BSP_SetHighGpioPin(uint32_t pin) {
	GPIO_PinSet(GPIO_Pin[pin]);
}


void BSP_SetLowGpioPin(uint32_t pin) {
	GPIO_PinClear(GPIO_Pin[pin]);
}


void BSP_ToggleGpioPin(uint32_t pin) {
	GPIO_PinToggle(GPIO_Pin[pin]);
}


void BSP_SetGpioPin(uint32_t pin, uint8_t lev) {
	if(lev) {
		GPIO_PinSet(GPIO_Pin[pin]);
	} else {
		GPIO_PinClear(GPIO_Pin[pin]);
	}
}
