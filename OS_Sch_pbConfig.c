/*
 * OS_Sch_pbConfig.c
 *
 *  Created on: Oct 22, 2019
 *      Author: Mahmoud Rezk Mahmoud
 */

#include "OS_SchpbConfig.h"
#include "TIMER.h"

OS_ConfigType OS_Config ={TIMER0,250
	};


OS_ConfigType * OS_ptrConfigType= &OS_Config;


