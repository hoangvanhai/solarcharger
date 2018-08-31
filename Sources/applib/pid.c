/** @FILE NAME:    pid.c
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
#include <pid.h>

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

//void PID_Init(SPID *pPid, _iq Kp, _iq Ki, _iq Kd, _iq T) {
//    PID_Reset(pPid);
//    pPid->KP = Kp;
//    pPid->KI = _IQ24mpy(Ki, _IQ24mpy(T, _IQ24(0.5)));
//    pPid->KD = _IQ24div(Kd, T);
//}
/*****************************************************************************/
/** @brief
 *
 *  Error_value = setPoint - feedBack;
    P_part  = Kp * Error_value;                                 => KP = kp
    I_part += Ki * (Error_value + pre_Error_value) / 2 * T;     => KI = Ki * T / 2
    D_part  = Kd * (Error_value - pre_Error_value) / T;         => KD = Kd / T
    out    += P_part + I_part + D_part;

    pre_Error = Error


    P_part = Kp*(Error - pre_Error);
    I_part = 0.5*Ki*T*(Error + pre_Error);
    D_part = Kd/T*( Error - 2*pre_Error+ pre_pre_Error);
    Out = pre_out + P_part + I_part + D_part ;
    pre_pre_Error = pre_Error
    pre_Error = Error
    pre_Out = Out

 *
 *  @param
 *  @return Void.
 *  @note
 */

//void PID_Process(SPID *pPid, float setPoint, _iq feedBack)     {
//    (pPid)->currErr       = setPoint - feedBack;
//    (pPid)->Proportional  = _IQ24mpy((pPid)->KP, (pPid)->currErr);
//    (pPid)->Integration  += _IQ24mpy((pPid)->KI, (pPid)->currErr + (pPid)->prevErr);
//    (pPid)->Derative      = _IQ24mpy((pPid)->KD, (pPid)->currErr - (pPid)->prevErr);
//    (pPid)->prevErr       = (pPid)->currErr;
//    (pPid)->PIDOut        = (pPid)->Proportional + (pPid)->Integration + (pPid)->Derative;
//}

