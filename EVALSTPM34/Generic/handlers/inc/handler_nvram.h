/**
*   @file      mnsh_nvram.h
*   @author    IPC - Industrial BU
*   @date      15 september 2013
*   @brief     Defines the public interface for the NVRAM
*   @note      (C) COPYRIGHT 2013 STMicroelectronics
*
* @attention
*
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
*/

#ifndef HANDLER_NVRAM_H
#define HANDLER_NVRAM_H

/*******************************************************************************
* INCLUDE FILES:
*******************************************************************************/
#include "nvram.h"

/** @addtogroup GENERIC
* @{
*/

/*******************************************************************************
* MACROS & CONSTANTS:
*******************************************************************************/
#define NVRAM_GEN_ID   (0)
#define NVRAM_LEG_ID   (3)

/*******************************************************************************
* TYPES:
*******************************************************************************/
extern nvm_t nvm;

/*******************************************************************************
* FUNCTIONS:
*******************************************************************************/
void NVM_Init (void);
void NVM_Erase(void);
uint8_t * NVM_GetPtr (uint8_t id);
void NVM_Write(nvmSave_t partialSave);

#endif /* HANDLER_NVRAM_H */

/**
* @}
*/

/* End Of File */

