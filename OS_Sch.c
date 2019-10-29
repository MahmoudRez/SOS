/*
 * OS_Sch.c
 *
 *  Created on: Oct 22, 2019
 *      Author: Mahmoud Rezk Mahmoud
 */
#include "OS_Sch.h"
#include "TIMER.h"
#include <avr/sleep.h>

typedef struct
{
	ST_Taskinfo Task_arrStr[OS_MAX_TASK_NB];
	uint8 Front;
	uint8 Rear;
	uint8 No_Of_Available_Tasks;
}OS_Buffer;


static volatile uint8 g_InitFlag;
static volatile uint8 g_matchingTickCount=0;
static volatile uint8 g_systemTickFlag=0;
static volatile OS_Buffer OS_arrBuffer[OS_MAX_TASK_PRIORITY];

static EnmOSError_t OS_Inque(ST_Taskinfo *COPY_St_ptrTaskInfo, uint8 COPY_BufferIndex);
static EnmOSError_t OS_Deque(uint8 COPY_BufferIndex);
static EnmOSError_t OS_UpdateStates(void);
static EnmOSError_t OS_Execute(void);


void systemTickCallBack(void)
{
	static uint8 timeCount=0;
	timeCount++;
	DDRB|=0x80;
	PORTB^=0x80;

	if(timeCount ==g_matchingTickCount)
	{
		g_systemTickFlag=1;
		timeCount=0;
	}
}

EnmOSError_t OS_Init (const OS_ConfigType * COPY_ConfigPtr )
{
	static EnmOSError_t LOC_InitStatus=ok_os;
	static volatile uint32 maxApplicableTickTime;
	static uint8 LOC_OverFlowError=OK;

	if(DEINIT== g_InitFlag)
	{
		LOC_InitStatus=TIMER_init();
		TIMER_setCallback(COPY_ConfigPtr->OS_TimerId,systemTickCallBack);
		if(OK==LOC_InitStatus)
		{
			LOC_OverFlowError = TIMER_calculateOverFlowTime(COPY_ConfigPtr->OS_TimerId,&maxApplicableTickTime);
			if(NOK==LOC_OverFlowError)
			{
				LOC_InitStatus=error_timer_calculateOverflow;
			}
			else if(OK==LOC_OverFlowError)
			{
				if(maxApplicableTickTime >= COPY_ConfigPtr->OS_SystemTick)
				{
					g_matchingTickCount = 1;
				}
				else
				{
					g_matchingTickCount = COPY_ConfigPtr->OS_SystemTick/maxApplicableTickTime;
				}

				g_InitFlag=INIT;
			}
		}
		else if(NOK==LOC_InitStatus)
		{
			LOC_InitStatus=timer_init_error;
		}
		else
		{

		}
	}
	else
	{
		LOC_InitStatus=repeted_init;
	}

	return LOC_InitStatus;
}


EnmOSError_t OS_DeInit ( void )
{
	EnmOSError_t LOC_DeinitStatus=ok_os;
	if(INIT==g_InitFlag)
	{
		TIMER_stop(OS_ptrConfigType->OS_TimerId);
	}
	else
	{
		LOC_DeinitStatus=repeted_deinit;
	}

	return LOC_DeinitStatus;
}


void OS_Run (void)
{
	uint8 LOC_Execute_Return;
	TIMER_start(OS_ptrConfigType->OS_TimerId);
	while(1)
	{
		if(g_systemTickFlag == 1)
		{
			g_systemTickFlag=0;
			OS_UpdateStates();
		}

		LOC_Execute_Return = OS_Execute();
		if(LOC_Execute_Return == empty_Buffer)
		{
			sleep_mode();
		}
	}


}


EnmOSError_t OS_Create_Task (ST_Taskinfo *COPY_St_ptrTaskInfo)
{
	EnmOSError_t LOC_CreatStatus=ok_os;
	if(COPY_St_ptrTaskInfo->ptr_Task!=NULL)
	{
		if(COPY_St_ptrTaskInfo->Task_Periodicity!=0)
		{
			if(COPY_St_ptrTaskInfo->Task_Priority!=0)
			{
				if(COPY_St_ptrTaskInfo->Time_ofNext_Excution>0)
				{
					COPY_St_ptrTaskInfo->Task_State=OS_WAIT;
					LOC_CreatStatus = OS_Inque(COPY_St_ptrTaskInfo,0);
				}
				else
				{
					COPY_St_ptrTaskInfo->Task_State=OS_READY;
					LOC_CreatStatus = OS_Inque(COPY_St_ptrTaskInfo,COPY_St_ptrTaskInfo->Task_Priority);
				}

			}
			else
			{
				LOC_CreatStatus=not_valid_periority;
			}

		}
		else
		{
			LOC_CreatStatus=not_valid_periodicity;
		}
	}
	else
	{
		LOC_CreatStatus=null_task;
	}

	return LOC_CreatStatus;
}

EnmOSError_t OS_Delete_Task (ST_Taskinfo * COPY_St_PtrTask)
{
	EnmOSError_t LOC_DeletStatus=ok_os;


	return LOC_DeletStatus;

}


static EnmOSError_t OS_UpdateStates(void)
{



	EnmOSError_t LOC_UpdateStatus=ok_os;
	uint8 LOC_indx;

	//PORTD=OS_arrBuffer[WAIT_BUFFER].No_Of_Available_Tasks;


	for(LOC_indx=0 ; LOC_indx < OS_arrBuffer[WAIT_BUFFER].No_Of_Available_Tasks ; LOC_indx++)
	{
		if(NULL != OS_arrBuffer[WAIT_BUFFER].Task_arrStr[LOC_indx].ptr_Task)
		{
			OS_arrBuffer[WAIT_BUFFER].Task_arrStr[LOC_indx].Time_ofNext_Excution--;
			if(0 ==( OS_arrBuffer[WAIT_BUFFER].Task_arrStr[LOC_indx].Time_ofNext_Excution))
			{
				OS_arrBuffer[WAIT_BUFFER].Task_arrStr[LOC_indx].Task_State = OS_READY;
				OS_Inque(&(OS_arrBuffer[WAIT_BUFFER].Task_arrStr[LOC_indx]),OS_arrBuffer[WAIT_BUFFER].Task_arrStr[LOC_indx].Task_Priority);
				OS_arrBuffer[WAIT_BUFFER].Task_arrStr[LOC_indx].ptr_Task = NULL;
				OS_arrBuffer[WAIT_BUFFER].Task_arrStr[LOC_indx] = OS_arrBuffer[WAIT_BUFFER].Task_arrStr[OS_arrBuffer[WAIT_BUFFER].Rear - 1];
				OS_arrBuffer[WAIT_BUFFER].No_Of_Available_Tasks--;
				(OS_arrBuffer[WAIT_BUFFER].Rear)--;
				LOC_indx--;
			}
			else
			{

			}
		}
		else
		{
			LOC_UpdateStatus = null_task;
		}
	}

	return LOC_UpdateStatus;
}


static EnmOSError_t OS_Execute(void)
{
	EnmOSError_t LOC_ExecuteStatus=ok_os;
	uint8 LOC_indx=1;
	uint8 LOC_emptyTasks_Flag=0;
	pvidPtr funcToExecute = NULL;

	for(LOC_indx=1 ; LOC_indx <= OS_MAX_TASK_PRIORITY ; LOC_indx++)
	{
		if(0 != OS_arrBuffer[LOC_indx].No_Of_Available_Tasks)
		{
			LOC_emptyTasks_Flag=1;
			funcToExecute = OS_arrBuffer[LOC_indx].Task_arrStr[OS_arrBuffer[LOC_indx].Front].ptr_Task;
			if(NULL != funcToExecute)
			{
				funcToExecute();
			}
			else
			{
				LOC_ExecuteStatus = null_task;
			}
			OS_arrBuffer[LOC_indx].Task_arrStr[OS_arrBuffer[LOC_indx].Front].Task_State = OS_WAIT;
			OS_arrBuffer[LOC_indx].Task_arrStr[OS_arrBuffer[LOC_indx].Front].Time_ofNext_Excution = OS_arrBuffer[LOC_indx].Task_arrStr[OS_arrBuffer[LOC_indx].Front].Task_Periodicity-1;
			OS_Inque(&(OS_arrBuffer[LOC_indx].Task_arrStr[OS_arrBuffer[LOC_indx].Front]),WAIT_BUFFER);
			OS_Deque(LOC_indx);
			break;
		}
		else
		{

		}
	}

	if(LOC_emptyTasks_Flag == 0)
	{
		LOC_ExecuteStatus = empty_Buffer;
	}


	return LOC_ExecuteStatus;
}



static EnmOSError_t OS_Inque(ST_Taskinfo *COPY_St_ptrTaskInfo, uint8 COPY_BufferIndex)
{
	EnmOSError_t LOC_InqueStatus=ok_os;

	if(OS_arrBuffer[COPY_BufferIndex].No_Of_Available_Tasks == OS_MAX_TASK_NB)
	{
		LOC_InqueStatus = max_no_of_tasks;
	}
	else
	{
		if(OS_arrBuffer[COPY_BufferIndex].Rear == OS_MAX_TASK_NB)
		{
			OS_arrBuffer[COPY_BufferIndex].Rear = 0;
		}
		else
		{

		}
		OS_arrBuffer[COPY_BufferIndex].Task_arrStr[OS_arrBuffer[COPY_BufferIndex].Rear] = *(COPY_St_ptrTaskInfo);
		OS_arrBuffer[COPY_BufferIndex].Rear++;
		OS_arrBuffer[COPY_BufferIndex].No_Of_Available_Tasks++;
	}

	return LOC_InqueStatus;
}
static EnmOSError_t OS_Deque(uint8 COPY_BufferIndex)
{
	EnmOSError_t LOC_DequeStatus=ok_os;

	if(0 == OS_arrBuffer[COPY_BufferIndex].No_Of_Available_Tasks)
	{
		LOC_DequeStatus = empty_Buffer;
	}
	else
	{
		if(OS_arrBuffer[COPY_BufferIndex].Front == (OS_MAX_TASK_NB-1))
		{
			OS_arrBuffer[COPY_BufferIndex].Front = 0;
		}
		else
		{
			OS_arrBuffer[COPY_BufferIndex].Front++;
		}

		OS_arrBuffer[COPY_BufferIndex].No_Of_Available_Tasks--;
	}

	return LOC_DequeStatus;
}

