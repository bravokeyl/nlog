/**
*   @file      handler_nvram.c
*   @author    IPC - Industrial BU
*   @date      17 May 2016
*   @brief     Implements routines to handle NVRAM
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

/*******************************************************************************
* INCLUDE FILES:
*******************************************************************************/
#include "st_device.h"
#include "handler_nvram.h"
#include "handler_eeprom.h"
//#include "mnsh_rx.h"

#include <string.h>

/*******************************************************************************
* TYPES:
*******************************************************************************/

/*******************************************************************************
* CONSTANTS & MACROS:
*******************************************************************************/
#define TABLE_MARKER                0xAAFFFFAA
#define NVRAM_SIZE                  (sizeof(nvm_t))

#define NVRAM_COPY_SIZE             (EEPROM_PAGE_SIZE)
#define NVRAM_START_ADDRESS         0
#define TABLE_MARKER_ADDRESS        0x1000
#define TABLE_START_ADDRESS         0x1010



const uint32_t nvmSectionSize[] = {
  sizeof(nvm_t),
  sizeof(nvmGen_t),
  sizeof(nvmLeg_t),
  sizeof(nvmLeg_t),
};

const uint32_t nvmSectionOffset[] = {
  0,
  0,
  NVRAM_SIZE_GEN,
  NVRAM_SIZE_GEN,
};

/*******************************************************************************
* GLOBAL VARIABLES:
*******************************************************************************/
nvm_t nvm;
uint8_t  nvmEntrySize;

/*******************************************************************************
* LOCAL VARIABLES:
*******************************************************************************/

/*******************************************************************************
* LOCAL FUNCTION PROTOTYPES:
*******************************************************************************/

/*******************************************************************************
*
*                       IMPLEMENTATION: Public functions
*
*******************************************************************************/
/**
  * @brief  Initialise NVRAM
  * @param  None
  * @retval None
  */
void NVM_Init (void)
{

#ifdef EEPROM_PRESENT

  uint32_t     address = NVRAM_START_ADDRESS;
  uint32_t     offset = 0;
    
  EEPROM_Conf();

  /* read EEPROM page by page */
  while (offset <  NVRAM_SIZE)  {
    EEPROM_Read(((uint8_t *)&nvm) + offset, address, EEPROM_PAGE_SIZE);
    offset += EEPROM_PAGE_SIZE;
    address += EEPROM_PAGE_SIZE;
  }
  
  if (nvm.gen.marker != NVRAM_MARKER_GEN)
  {
    memset(&nvm, 0xFF, NVRAM_SIZE);
    nvm.gen.marker = NVRAM_MARKER_GEN;
    nvm.gen.version = NVRAM_VERSION_GEN;
    nvm.gen.platform = PLTF_STM32_METER;
    EEPROM_Write(NVRAM_START_ADDRESS, (uint8_t *)&nvm.gen.marker, 8);
  }

#else
  
    memset(&nvm, 0xFF, NVRAM_SIZE);
    nvm.gen.marker = NVRAM_MARKER_GEN;
    nvm.gen.version = NVRAM_VERSION_GEN;
    nvm.gen.platform = PLTF_STM32_METER;

#endif  
}

/**
  * @brief  erase NVRAM
  * @param  None
  * @retval None
  */
void NVM_Erase(void)
{
}

/**
  * @brief  Read from NVRAM
  * @param  id the binary id
  * @retval returns pointer to NVRAM structure in RAM for the binary id
  */
uint8_t * NVM_GetPtr (uint8_t id)
{
  uint8_t *ptr;
  switch (id) 
  {
  case NVRAM_GEN_ID:
    ptr = nvm.tmpGen;
    break;
  case NVRAM_LEG_ID:
    ptr = nvm.tmpLeg;
    break;
  }
  return (ptr);
}

/**
  * @brief  force write into NVRAM of structure stored in RAM
  * @param  partialSave indicates the part of NVRAM to update
  *         it also indicates if it requires a change/erase of sector
  * @retval None
  */
void NVM_Write(nvmSave_t partialSave)
{

#ifdef EEPROM_PRESENT
  
  // without erase is meaningless for EEPROM
  if (partialSave >= NVM_SAVE_WITHOUT_ERASE_ALL)
  {
    partialSave -= NVM_SAVE_WITHOUT_ERASE_ALL;
  }
  // write EEPROM
    uint32_t     address;
    uint32_t     offset;
    offset = nvmSectionOffset[partialSave];
    address = NVRAM_START_ADDRESS + nvmSectionOffset[partialSave];
    // partial save can be supported on SFLASH if no erase is performed
    while (offset < nvmSectionOffset[partialSave] + nvmSectionSize[partialSave])
    {
      uint32_t size;
      if ((NVRAM_SIZE - offset) < NVRAM_COPY_SIZE)
      {
        size = NVRAM_SIZE - offset;
      }
      else
      {
        size = NVRAM_COPY_SIZE;
      }
      // write invalid sector with NVM data
      EEPROM_Write(address, (uint8_t *)&nvm + offset, size);
      offset += NVRAM_COPY_SIZE;
      address += NVRAM_COPY_SIZE;
    }
#endif 
}

/*******************************************************************************
*
*                       IMPLEMENTATION: Private functions
*
*******************************************************************************/

/**
  * @}
  */

/* End Of File */
