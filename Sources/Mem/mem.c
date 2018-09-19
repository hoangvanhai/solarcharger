
/***************************** Include Files *********************************/
#include <console.h>
#include "mem.h"
#include "queue.h"
#include "typedefs.h"
#include "app_cfg.h"
#include <TransDef.h>

#if (TOTAL_NUMBER_OF_MEMS > 0)
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

#if (TOTAL_NUMBER_OF_SMALL_MEMS > 0)
/**
 * Checks whether the buffer pointer provided is of small buffer or of a large
 * buffer
 */
#define IS_SMALL_MEM(p) ((p)->u8Body >= (u8MemPool +  LARGE_MEM_SIZE * TOTAL_NUMBER_OF_LARGE_MEMS))

#endif

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/**
 * Common Buffer pool holding the buffer user area
 */
#if (TOTAL_NUMBER_OF_SMALL_MEMS > 0)

static uint8_t u8MemPool[(TOTAL_NUMBER_OF_LARGE_MEMS * LARGE_MEM_SIZE) +
                       (TOTAL_NUMBER_OF_SMALL_MEMS * SMALL_MEM_SIZE)];
#else

static uint8_t u8MemPool[TOTAL_NUMBER_OF_LARGE_MEMS * LARGE_MEM_SIZE];

#endif

static SMem  u8MemHeader[TOTAL_NUMBER_OF_LARGE_MEMS + TOTAL_NUMBER_OF_SMALL_MEMS];

/*
 * Queue of free large buffers
 */
#if (TOTAL_NUMBER_OF_LARGE_MEMS > 0)
SQueue qFreeLargeQueue;
#endif

/*
 * Queue of free small buffers
 */
#if (TOTAL_NUMBER_OF_SMALL_MEMS > 0)
SQueue qFreeSmallQueue;
#endif

//static uint16_t AllocCounter = 0, FreeCounter = 0;

/*****************************************************************************/
#ifdef DEBUG
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Mem_Log(void)
{
    
//    LREP("\r\nTotal Alloc: %d; Total Free: %d", (uint32_t)u16ReportMemAlloc, (uint32_t)u16ReportMemFree);
    
	LREP("\r\nFree Large Queue Size = %d",  (int)qFreeLargeQueue.u8Size);
#if (TOTAL_NUMBER_OF_SMALL_MEMS > 0)
	LREP("\r\nFree Small Queue Size = %d",  (int)qFreeSmallQueue.u8Size);
#endif
}
#endif

/*****************************************************************************/
/**
 * @brief Initializes the buffer module.
 *
 *
 *  @param	none
 *  @return Void.
 */
void Mem_Init(void)
{
    uint16_t u16Idx;

    /* Initialize free buffer queue*/
	#if (TOTAL_NUMBER_OF_LARGE_MEMS > 0)
    	Queue_Init(&qFreeLargeQueue);
    #endif
 
    #if (TOTAL_NUMBER_OF_SMALL_MEMS > 0)
            Queue_Init(&qFreeSmallQueue);
    #endif

    #if (TOTAL_NUMBER_OF_LARGE_MEMS > 0)
    for (u16Idx = 0; u16Idx < TOTAL_NUMBER_OF_LARGE_MEMS; u16Idx++)
    {
        u8MemHeader[u16Idx].u8Body = u8MemPool + (u16Idx * LARGE_MEM_SIZE);

//        LREP("\r\nHeader : %d -> addr: 0x%x", (uint32_t)u16Idx, (uint32_t)&u8MemHeader[u16Idx]);
//        LREP("\r\nMem blk: %d -> addr: 0x%x", (uint32_t)u16Idx, (uint32_t)u8MemHeader[u16Idx].u8Body);
  
        Queue_Append(&qFreeLargeQueue, &u8MemHeader[u16Idx]);
    }
    #endif
    
    #if (TOTAL_NUMBER_OF_SMALL_MEMS > 0)
    for (u16Idx = 0; u16Idx < TOTAL_NUMBER_OF_SMALL_MEMS; u16Idx++)
    {
        u8MemHeader[u16Idx + TOTAL_NUMBER_OF_LARGE_MEMS].u8Body = \
            		(u8MemPool + (TOTAL_NUMBER_OF_LARGE_MEMS * LARGE_MEM_SIZE)) + (u16Idx * SMALL_MEM_SIZE);

//        LREP("\r\nSHeader : %d -> addr: 0x%x", (uint32_t)u16Idx, (uint32_t)&u8MemHeader[u16Idx + TOTAL_NUMBER_OF_LARGE_MEMS]);
//        LREP("\r\nSMem blk: %d -> addr: 0x%x", (uint32_t)u16Idx, (uint32_t)u8MemHeader[u16Idx + TOTAL_NUMBER_OF_LARGE_MEMS].u8Body);
        
        Queue_Append(&qFreeSmallQueue, &u8MemHeader[u16Idx + TOTAL_NUMBER_OF_LARGE_MEMS]);

    }
            
    #endif
//    for (u16Idx = 0; u16Idx < TOTAL_NUMBER_OF_LARGE_MEMS + TOTAL_NUMBER_OF_SMALL_MEMS; u16Idx++)
//    {
//        LREP("\r\n %d - Header = 0x%x, Body = 0x%x",(uint32_t)u16Idx,(uint32_t)&u8MemHeader[u16Idx],(uint32_t)u8MemHeader[u16Idx].u8Body);
//
//        if(u16Idx < TOTAL_NUMBER_OF_LARGE_MEMS + TOTAL_NUMBER_OF_SMALL_MEMS - 1)
//        {
//            LREP(", Size = %d",(uint32_t)(u8MemHeader[u16Idx+1].u8Body - u8MemHeader[u16Idx].u8Body));
//        }
//    }
	
//    LREP("\r\nSize of SFrameInfo = %d",(uint32_t)sizeof(SFrameInfo));
//    LREP("\r\nLarge Size = %d",(uint32_t)LARGE_MEM_SIZE);
//    LREP("\r\nSmall Size = %d",(uint32_t)SMALL_MEM_SIZE);
}

/*****************************************************************************/
/**
 * @brief Allocates a buffer
 *
 * @param size size of buffer to be allocated.
 * @return pointer to the buffer allocated,  NULL if buffer not available.
 */
SMem *Mem_Alloc(uint16_t u16Size)
{
    SMem *pMem = NULL;
	
    u16Size = u16Size;    /* Keep compiler happy. */

//    LREP("\r\nMem_Alloc: Size = %d; LARGE_MEM_SIZE = %d",(uint32_t)u16Size, (uint32_t)(LARGE_MEM_SIZE));

    #if (TOTAL_NUMBER_OF_SMALL_MEMS > 0)

    if (u16Size <= LARGE_MEM_SIZE)
    {
        if (u16Size <= SMALL_MEM_SIZE)
        {
            pMem = Queue_Remove(&qFreeSmallQueue, NULL);
//            LREP("\r\nSmall pMem = 0x%x",(uint32_t)pMem);
        }

        //if (NULL == pMem)
        else
        {
            pMem = Queue_Remove(&qFreeLargeQueue, NULL);
//            LREP("\r\nLarge pMem = 0x%x",(uint32_t)pMem);
        }
    }
    #else

    pMem = Queue_Remove(&qFreeLargeQueue, NULL);

    #endif
	
    //pMem = Queue_Remove(&free_queue, NULL);
//    if(pMem != NULL)        
//        LREP("\r\nAllocCounter: %d - FreeCounter: %d", 
//                (uint32_t)AllocCounter++, (uint32_t)FreeCounter);
    return pMem;
}

/*****************************************************************************/
/**
 * @brief Frees up a buffer.
 *
 * @param pMem Pointer to buffer that has to be freed.
 * @return Void.
 */
void Mem_Free(SMem *pMem)
{
    if (NULL == pMem)
    {	    
	    ASSERT(FALSE);
        return;
    }
    
    
	
	#if (TOTAL_NUMBER_OF_SMALL_MEMS > 0)
	    if (IS_SMALL_MEM(pMem))
	    {
//            LREP("\r\nAllocCounter: %d - FreeCounter: %d", 
//                (uint32_t)AllocCounter, (uint32_t)FreeCounter++);
	        Queue_Append(&qFreeSmallQueue, pMem);
	    }
	    else
	    {
//            LREP("\r\nAllocCounter: %d - FreeCounter: %d", 
//                (uint32_t)AllocCounter, (uint32_t)FreeCounter++);
	        Queue_Append(&qFreeLargeQueue, pMem);
	    }
	#else 
	
	    Queue_Append(&qFreeLargeQueue, pMem);
	    
	#endif
}
/*****************************************************************************/
/**
 * @brief Frees up a buffer.
 *
 * @param pMem Pointer to free buffer
 * @return Void.
 */
BOOL Mem_IsAvailableSpace(uint16_t u16Size)
{	
    //return (free_queue.u8Size == 0) ? FALSE : TRUE;

    return (Queue_GetFreeQueueSize(u16Size) == 0) ? FALSE : TRUE;
}
#if (TOTAL_NUMBER_OF_SMALL_MEMS > 0)
BOOL Mem_IsAvailableSmallSpace()
{	
    //return (free_queue.u8Size == 0) ? FALSE : TRUE;

    return (Queue_GetFreeSmallQueueSize() == 0) ? FALSE : TRUE;
}
#endif
BOOL Mem_IsAvailableLargeSpace()
{	
    //return (free_queue.u8Size == 0) ? FALSE : TRUE;

    return (Queue_GetFreeLargeQueueSize() == 0) ? FALSE : TRUE;
}	
/*****************************************************************************/
/**
 * @brief Frees up a buffer.
 *
 * @param pMem Pointer to free buffer
 * @return Void.
 */
uint8_t Mem_GetAvailableSpace(uint16_t u16Size)
{
    return Queue_GetFreeQueueSize(u16Size);

    //return (uint8_t)(free_queue.u8Size);

}
#if (TOTAL_NUMBER_OF_SMALL_MEMS > 0)
uint8_t Mem_GetAvailableSmallSpace()
{
	return Queue_GetFreeSmallQueueSize();		
}
#endif
uint8_t Mem_GetAvailableLargeSpace()
{
	return Queue_GetFreeLargeQueueSize();
}
/*****************************************************************************/
#endif //TOTAL_NUMBER_OF_MEMS

