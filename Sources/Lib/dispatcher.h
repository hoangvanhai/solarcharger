/*
 * dispatcher.h
 *
 *  Created on: Jun 19, 2018
 *      Author: MSI
 */

#ifndef LIB_DISPATCHER_H_
#define LIB_DISPATCHER_H_

/***************************** Include Files *********************************/
#include <typedefs.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

typedef void(*task_func_t)(void *arg);
typedef void* task_handle;

typedef struct task_desc_ {
	struct task_desc 	*next;
	uint8_t 			run;
	uint8_t				always;
	void				*arg;
	task_func_t 		func_ptr;
}task_desc;


typedef struct stask_desc_list_ {
	task_desc *taskHead;
	task_desc *taskTail;
}stask_desc_list;


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/


void 		task_init();
task_handle task_create(task_func_t task_, void *arg, uint8_t always);
uint8_t 		task_delete(task_handle handle);
void 		task_main_exec();
uint8_t		task_is_running(task_handle handle);
void 		task_set_run(task_handle handle, uint8_t times);
uint8_t		task_get_run(task_handle handle);
void 		task_cancel(task_handle handle);

/************************** Variable Definitions *****************************/

extern stask_desc_list task_list;

#endif /* LIB_DISPATCHER_H_ */
