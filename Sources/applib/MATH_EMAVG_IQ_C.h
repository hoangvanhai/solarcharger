//================================================================================
// Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
//  ALL RIGHTS RESERVED
//================================================================================
/**
 * Performs exponential moving average calculation.  This macro requires the use of the
 * MATH_EMAVG_IQ_C structure.  The structure should be initialized with the MATH_EMAVG_IQ_C_INIT
 * macro.
 */

#ifndef MATH_EMAVG_IQ_C_H_
#define MATH_EMAVG_IQ_C_H_

#define PI          (3.1415926535897932384626433832795)

/**
 * Exponential-moving average structure
 */
typedef struct {
	float In;
	float Out;
	float Multiplier;
} SMATH_EMAVG_IQ_C;


/**
 * Performs exponential moving average calculation.
 * @param m - MATH_EMAVG_IQ_C structure with values.
 * @return MATH_EMAVG_IQ_C Out parameter.
 */
#define MATH_EMAVG_IQ_C(v)  {   \
                                v.Out = ((v.In - v.Out) * v.Multiplier) + v.Out; \
                            }

/**
 * Initial values for MATH_EMAVG_IQ_C structure.
 */
#define MATH_EMAVG_IQ_C_INIT(v, mult)	\
	v.In = 0;						\
	v.Out = 0;						\
	v.Multiplier = mult

#endif /* MATH_EMAVG_IQ_C_H_ */
