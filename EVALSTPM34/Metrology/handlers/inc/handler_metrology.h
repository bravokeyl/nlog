/**
*   @file      mnsh_metrology.h
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

#ifndef HANDLER_METROLOGY_H
#define HANDLER_METROLOGY_H

/*******************************************************************************
* INCLUDE FILES:
*******************************************************************************/
#include <stdint.h>

/** @addtogroup GENERIC
  * @{
  */


/*******************************************************************************
* TYPES:
*******************************************************************************/

/*******************************************************************************
* FUNCTIONS:
*******************************************************************************/
void MET_Conf(void);
void Metro_com_port_device(void);
void MET_RestoreConfigFromNVM( void );
void MET_SaveConfigToNVM( void );
void MET_RestoreDefaultConfig(uint32_t nbPhase);

#endif /* HANDLER_METROLOGY_H */

/**
  * @}
  */

/* End Of File */

