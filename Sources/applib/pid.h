/** @FILE NAME:    pid.h
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
#ifndef APPLICATION_PIDCTRL_PID_H_
#define APPLICATION_PIDCTRL_PID_H_


/***************************** Include Files *********************************/
#include <typedefs.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

typedef struct STUN_ {
    float         Out;
    float         aCoeff;
    float         bCoeff;
}STUN;


typedef struct SPID_ {
	float         KP;
	float         KI;
	float         KD;
	float         currErr;
	float         prevErr;
    float         Integration;
    float         Derative;
    float         Proportional;
    float         PIDOut;
    STUN          kpTun;
}SPID;
/***************** Macros (Inline Functions) Definitions *********************/
#define PID_Reset(pid) {                        \
                        (pid)->currErr = 0;       \
                        (pid)->prevErr = 0;       \
                        (pid)->Integration = 0;   \
                        (pid)->Derative = 0;      \
                        (pid)->Proportional = 0;  \
                        (pid)->PIDOut = 0;        \
                      }


#define PID_Init(pid, Kp, Ki, Kd, F, aCo, bCo) {             \
    PID_Reset(pid);                                			\
    (pid)->KP = Kp;                         				\
    (pid)->KI = (float)(Ki) * 0.5 * (1/(float)(F)) ;         \
    (pid)->KD = (float)(Kd) * (F);                 			\
    (pid)->kpTun.aCoeff = (float)aCo;  							\
    (pid)->kpTun.bCoeff = (float)bCo; 						\
	}

;    								\

#define PID_ProcessM(pPid, setPoint, feedBackV) 			{			\
    (pPid)->currErr       = (setPoint - feedBackV);						\
    (pPid)->Proportional  = (pPid)->KP * (pPid)->currErr;                       	\
    (pPid)->Integration  += (pPid)->KI * ((pPid)->currErr + (pPid)->prevErr);     	\
    (pPid)->Derative      = (pPid)->KD * ((pPid)->currErr - (pPid)->prevErr);     	\
    (pPid)->prevErr       = (pPid)->currErr;                                                        \
    (pPid)->PIDOut        = (pPid)->Proportional + (pPid)->Integration + (pPid)->Derative; }

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/



#endif /* APPLICATION_PIDCTRL_PID_H_ */
