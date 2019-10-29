/*
 * OS_Sch.h
 *
 *  Created on: Oct 22, 2019
 *      Author: Mahmoud Rezk Mahmoud
 */

#ifndef OS_SCH_H_
#define OS_SCH_H_

#include "OS_SchpbConfig.h"

typedef void (*pvidPtr)(void);
typedef enum{
	ok_os,
	repeted_init,
	timer_init_error,
	error_timer_calculateOverflow,
	repeted_deinit,
	not_valid_periodicity,
	not_valid_periority,
	null_task,
	max_no_of_tasks,
	empty_Buffer
}EnmOSError_t;



typedef struct {
	uint8 Task_Priority;
	uint16 Task_Periodicity;
	pvidPtr ptr_Task ;
	uint16 Time_ofNext_Excution;
	uint8 Task_State;

}ST_Taskinfo;



#define DEINIT 0
#define INIT 1
#define REER 1
#define FRONT 0
#define OS_READY 0
#define OS_WAIT 1
#define OS_DELETE 2
#define WAIT_BUFFER 0


extern EnmOSError_t OS_Init (const OS_ConfigType * COPY_ConfigPtr ) ;
extern EnmOSError_t OS_DeInit ( void ) ;
extern void OS_Run (void);
extern EnmOSError_t OS_Create_Task (ST_Taskinfo *COPY_St_ptrTaskInfo);
extern EnmOSError_t OS_Delete_Task (ST_Taskinfo * COPY_St_PtrTask);


#endif /* OS_SCH_H_ */
