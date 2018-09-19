#ifndef _CRC8_H_
#define _CRC8_H_

/***************************** Include Files *********************************/
#include "typedefs.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/
#define ByteCRC8(crc, ch) 	(sa_bCRC8Table[crc ^ ch])

/************************** Function Prototypes ******************************/
BYTE CalCRC8(const LPBYTE pBuff, int nSize);
/************************** Variable Definitions *****************************/
extern const BYTE sa_bCRC8Table[];
/*****************************************************************************/

#endif
