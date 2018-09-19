#ifndef BUFFER_H_
#define BUFFER_H_


/***************************** Include Files *********************************/
#include "typedefs.h"
#include "app_cfg.h"
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

typedef struct _SMem
{
    /** Pointer to the buffer body */
    uint8_t *u8Body;
    /** Pointer to next free buffer */
    struct _SMem *pNext;
} SMem;


/***************** Macros (Inline Functions) Definitions *********************/

#define MEM_BODY(buf) ((buf)->u8Body)

/************************** Function Prototypes ******************************/

#ifdef __cplusplus
extern "C" {
#endif


void 	Mem_Init(void);
SMem* 	Mem_Alloc(uint16_t u16Size);
void 	Mem_Free(SMem *pbuffer);


BOOL 	Mem_IsAvailableSpace(uint16_t u16Size);
uint8_t 	Mem_GetAvailableSpace(uint16_t u16Size);

BOOL 	Mem_IsAvailableSmallSpace();
uint8_t 	Mem_GetAvailableSmallSpace();

BOOL 	Mem_IsAvailableLargeSpace();
uint8_t 	Mem_GetAvailableLargeSpace();

#ifdef DEBUG
void 	Mem_Log(void);
#endif


/************************** Variable Definitions *****************************/

/*****************************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* BUFFER_H_ */
