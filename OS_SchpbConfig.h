/*
 * OS_SchpbConfig.h
 *
 *  Created on: Oct 22, 2019
 *      Author: alx
 */

#ifndef OS_SCHPBCONFIG_H_
#define OS_SCHPBCONFIG_H_
#include "OS_Sch_Config.h"
#include "STD_Types.h"

typedef struct {
	uint8 OS_TimerId;
	uint16 OS_SystemTick;
}OS_ConfigType;

extern OS_ConfigType * OS_ptrConfigType;

#endif /* OS_SCHPBCONFIG_H_ */
