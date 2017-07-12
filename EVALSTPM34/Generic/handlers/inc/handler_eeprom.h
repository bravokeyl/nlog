/**
*   @file      handler_eeprom.h
*   @author    IPC - Industrial BU
*   @date      15 september 2013
*   @brief     Defines the public interface for the eeprom handler routines
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

#ifndef HANDLER_EEPROM_H
#define HANDLER_EEPROM_H

/*******************************************************************************
* INCLUDE FILES:
*******************************************************************************/
#include <stdint.h>

/** @addtogroup GENERIC
  * @{
  */

/*******************************************************************************
* MACROS & CONSTANTS:
*******************************************************************************/
#define EEPROM_CMD_RD_MEMORY_ARAY       0xA1
#define EEPROM_CMD_WR_MEMORY_ARAY       0xA0
#define EEPROM_PAGE_SIZE                64

/*******************************************************************************
* TYPES:
*******************************************************************************/

/*******************************************************************************
* FUNCTIONS:
*******************************************************************************/
void EEPROM_Conf(void);
void EEPROM_Read(uint8_t *buf, uint32_t address, uint16_t size);
void EEPROM_Write(uint32_t address, uint8_t *buf, uint16_t size);

#endif /* HANDLER_EEPROM_H */

/** 
  * @}
  */

/* End Of File */

