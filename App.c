/*
 * App.c
 *
 *  Created on: Oct 22, 2019
 *      Author: Mahmoud Rezk Mahmoud
 */
#include "OS_Sch.h"
#include "REG_Lib.h"


void BLINK_vidGreen(void)
{
	uint8 LOC_ToggleFlag=0;
	DDRB|=0x10;
	PORTB ^=0x10;
	LOC_ToggleFlag=1;
}

void BLINK_vidRed(void)
{
	uint8 LOC_ToggleFlag=0;
	DDRB|=0x20;
	PORTB^=0x20;
	LOC_ToggleFlag=1;


}

void BLINK_vidYellow(void)
{
	uint8 LOC_ToggleFlag=0;
	DDRB|=0x40;
	PORTB^=0x40;
	LOC_ToggleFlag=1;


}
void main (void)
{
	ST_Taskinfo T_Blink_Green = {
			2,
			8,
			BLINK_vidGreen,
			0
	};

	ST_Taskinfo T_Blink_Red = {
			2,
			8,
			BLINK_vidRed,
			0
	};
	ST_Taskinfo T_Blink_Yellow = {
			1,
			8,
			BLINK_vidYellow,
			0
	};


	OS_Create_Task(&T_Blink_Yellow);
	OS_Create_Task(&T_Blink_Green);
	OS_Create_Task(&T_Blink_Red);
	OS_Init(OS_ptrConfigType);

	OS_Run();





}
