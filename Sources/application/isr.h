/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef __ISR_H
#define __ISR_H


/* Example */
/*
#undef  VECTOR_036
#define VECTOR_036 RTC_Isr

// ISR(s) are defined in your project directory.
extern void RTC_Isr(void);
*/


#undef  VECTOR_028
#define VECTOR_028  UART0_Isr

#undef  VECTOR_029
#define VECTOR_029  UART1_Isr

#undef  VECTOR_030
#define VECTOR_030  UART2_Isr

#undef 	VECTOR_031
#define VECTOR_031	ADC_Isr

#undef  VECTOR_033
#define VECTOR_033  FTM0_Isr

#undef 	VECTOR_035
#define VECTOR_035	FTM2_Isr

#undef  VECTOR_036
#define VECTOR_036 	RTC_Isr

#undef  VECTOR_038 
#define VECTOR_038 	PIT_Ch0Isr   /*!< Vector 38 points to PIT channel 0 interrupt service routine */

#undef  VECTOR_039 
#define VECTOR_039 	PIT_Ch1Isr   /*!< Vector 39 points to PIT channel 1 interrupt service routine */

extern void PIT_Ch1Isr(void);
extern void PIT_Ch0Isr(void);
extern void RTC_Isr(void);
extern void UART0_Isr(void);
extern void UART1_Isr(void);
extern void UART2_Isr(void);
extern void FTM0_Isr(void);
extern void FTM2_Isr(void);
extern void ADC_Isr(void);

#endif  //__ISR_H

/* End of "isr.h" */
