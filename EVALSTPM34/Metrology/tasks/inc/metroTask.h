/**
*   @file      metroTask.h
*   @author    STMicroelectronics
*   @version   V1.0
*   @date      11-March-2016
*   @brief     This code includes exported items for Metrology task
*   @note      (C) COPYRIGHT 2013 STMicroelectronics
*
* @attention
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
*/

#ifndef METROTASK_H
#define METROTASK_H

/*******************************************************************************
* INCLUDE FILES:
*******************************************************************************/
#include "stpm_metrology.h"
#include "metrology.h"
//#include "metroTask.h"
#include "nvram.h"


/** @addtogroup LEGAL
  * @{
  */

/*******************************************************************************
* CONSTANTS & MACROS:
*******************************************************************************/

#define METRO_PHASE_1           0
#define METRO_PHASE_2           1
#define METRO_PHASE_3           2
#define METRO_MAX_PHASES        3

#define METRO_DATA_ACTIVE       0
#define METRO_DATA_REACTIVE     1
#define METRO_DATA_APPARENT     2

#define NVRAM_SIZE_LEG           512

#define METRO_TIMER              (1) // 500ms  steps


/*******************************************************************************
* TYPES:
*******************************************************************************/
typedef struct
{
  uint8_t       metroTimerActive;
  uint8_t       nbPhase;
  uint16_t      metroInactiveTime;
  int32_t       powerActive;
  int32_t       powerReactive;
  int32_t       powerApparent;
  int32_t       energyActive;
  int32_t       energyReactive;
  int32_t       energyApparent;
  int32_t       chanPower[3][METRO_MAX_PHASES];
  uint32_t      rawEnergy[3][METRO_MAX_PHASES];
  int32_t       rawEnergyExt[3][METRO_MAX_PHASES];
  uint32_t      rmsvoltage;
  uint32_t      rmscurrent;
  nvmLeg_t      *nvm;
} metroData_t;


/*******************************************************************************
* GLOBAL VARIABLES:
*******************************************************************************/
extern metroData_t metroData;

/*******************************************************************************
* FUNCTIONS:
*******************************************************************************/
void METRO_Init();
void METRO_Task();
void METRO_UpdateData();
void METRO_Latch_Measures();
void METRO_Get_Measures();
/**
  * @}
  */

#endif /* METROTASK_H */

/* End Of File */
