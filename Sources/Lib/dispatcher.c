/*
 * dispatcher.c
 *
 *  Created on: Jun 19, 2018
 *      Author: MSI
 */

#include <dispatcher.h>
#include <console.h>
#include <common/assert.h>
#include <cstdlib>

/***************** Macros (Inline Functions) Definitions *********************/

#define TASK_ENTER_CRITICAL()    //DisableIntT1
#define TASK_EXIT_CRITICAL()     //EnableIntT1

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

stask_desc_list task_list;


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void 		task_init() {

    TASK_ENTER_CRITICAL();
    task_list.taskHead = 0;
    task_list.taskTail = 0;
    TASK_EXIT_CRITICAL();
}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
task_handle task_create(task_func_t task, void *arg, uint8_t always) {

	task_desc*  newNode=0;
	if(task)
	{
		newNode = malloc(sizeof(*newNode));
		if(newNode)
		{
			newNode->func_ptr = task;
			newNode->always = always;
			newNode->run = 0;
			newNode->arg = arg;
			TASK_ENTER_CRITICAL();

			// add tail
			if(task_list.taskTail == 0)
			{
				task_list.taskHead = task_list.taskTail= newNode;
				newNode->next = NULL;
			}
			else
			{
				task_list.taskTail->next= (struct task_desc *)newNode;
				task_list.taskTail= newNode;
				newNode->next = NULL;
			}

			TASK_EXIT_CRITICAL();

		} else {
			ASSERT(FALSE);
		}
	}

	return (task_handle)newNode;
}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
uint8_t 		task_delete(task_handle handle) {
    task_desc   *pTask, *prev;

    if(task_list.taskHead == 0)
    {
        return FALSE;   // no registered handlers
    }

    TASK_ENTER_CRITICAL();

    if((pTask=task_list.taskHead) == (task_desc*)handle)
    {   // remove head
        if(task_list.taskHead==task_list.taskTail)
        {
        	task_list.taskHead=task_list.taskTail=0;
        }
        else
        {
        	task_list.taskHead=(task_desc*)pTask->next;
        }
    }
    else
    {
        for(prev=(task_desc*)task_list.taskHead, pTask=(task_desc*)task_list.taskHead->next;
        		pTask!=0; prev=(task_desc*)pTask, pTask=(task_desc*)pTask->next)
        {   // search within the list
            if(pTask == (task_desc*)handle)
            {   // found it
                prev->next=pTask->next;
                if(task_list.taskTail==pTask)
                {   // adjust tail
                	task_list.taskTail=prev;
                }
                break;
            }
        }
    }
    TASK_EXIT_CRITICAL();

    if(pTask)
    {
        free(pTask);
    }

    return pTask != 0;
}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void 		task_main_exec() {

    task_desc   *pTask;
    for(pTask=(task_desc*)task_list.taskHead; pTask!=0;
    		pTask=(task_desc*)pTask->next)
    {
        if(pTask->always || pTask->run > 0)
        {
			pTask->func_ptr(pTask->arg);
			if(pTask->always == FALSE) pTask->run--;
        }
    }

}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
uint8_t		task_is_running(task_handle handle) {
	task_desc*  pHandle;
	uint8_t  bRunning = FALSE;
	pHandle = (task_desc*)handle;
	TASK_ENTER_CRITICAL();
	bRunning = pHandle->run > 0 ? TRUE : FALSE;
	TASK_EXIT_CRITICAL();
	return bRunning;
}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void 		task_set_run(task_handle handle, uint8_t times) {
	task_desc*  pHandle;
	pHandle = (task_desc*)handle;
	TASK_ENTER_CRITICAL();
	pHandle->run += times;
	TASK_EXIT_CRITICAL();
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
uint8_t		task_get_run(task_handle handle) {
	task_desc*  pHandle;
	uint8_t  run = 0;
	pHandle = (task_desc*)handle;
	TASK_ENTER_CRITICAL();
	run = pHandle->run;
	TASK_EXIT_CRITICAL();
	return run;
}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void 		task_cancel(task_handle handle) {
	task_desc*  pHandle;
	pHandle = (task_desc*)handle;
	TASK_ENTER_CRITICAL();
	pHandle->run = 0;
	TASK_EXIT_CRITICAL();
}
