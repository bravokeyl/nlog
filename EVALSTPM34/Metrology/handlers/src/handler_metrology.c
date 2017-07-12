/**
*   @file      handler_metrology.c
*   @author    IPC - Industrial BU
*   @date      17 May 2016
*   @brief     Implements routines to handle Metrology
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

#include "handler_metrology.h"
#include "metrology.h"
#include "st_device.h"
#include "metroTask.h"
#include "nvram.h"
#include "handler_nvram.h"
#include "readstpm.h"
#include <string.h>

/** @addtogroup GENERIC
  * @{
  */
/*
#define NVRAM_MARKER_LEG            0xAA3333AA
#define NVRAM_VERSION_LEG           1
*/
/*******************************************************************************
* TYPES:
*******************************************************************************/

/*******************************************************************************
* MACROS & CONSTANTS:
*******************************************************************************/
       /*+------------------------------------------------------------------------------------+
         |                                        U32                                         |
         |---------------------|-------------------|-------------------|----------------------|
         |     STPM EXT4       |     STPM EXT3     |     STPM EXT2     |     STPM EXT1        |
         |---------------------|-------------------|-------------------|----------------------|
         |    u4   |     u4    |   u4    |   u4    |     u4  |     u4  |      u4   |  u4      |
         |---------|-----------|--------------------------------------------------------------|
         |CH masks | STPM type |CH masks |STPM type|CH masks |STPM type|  CH masks |STPM type |
         |---------|-----------|--------------------------------------------------------------|

        STPM CFG EXTx (u8):
        -----------------
        MSB u4 : Channel  Mask :  Channels affected to STPM
            0 : No Channel affected
            1 : Channel 1 affected
            2 : Channel 2 affected
            4 : Channel 3 affected
            8 : Channel 4 affected

        LSB u4 :  STPM type : 6 to 8
            0 : No STPM
            6 : STPM32
            7 : STPM33
            8 : STPM34

        EX : STPM EXT 1: One STPM34 with Channels 2 and 3 affected on it
        LSB u4 = 8 (STPM34)
        MSB u4 = 6 ( 4:Channel 3 + 2:Channel 2)

        STPM CONFIG : U32 = 0x00000068

        +------------------------------------------------------------------------------------+*/

const nvmLeg_t metroDefaultNvm = {
  NVRAM_MARKER_LEG, // marker
  NVRAM_VERSION_LEG,// version
  PLTF_STM32_METER, // crc
  {                 // config
    0x00000005,
    0x00000000
  },
  {                 // data1[19] STPM (Config for CT)
    0x040000a0,
    0x240000a0,
    0x000004e0,
    0x00000000,
    0x003ff800,
    0x003ff800,
    0x003ff800,
    0x003ff800,
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x03270327,
    0x03270327,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00004007,
  },
  {                 // data2[19] STPM (Config for CT)
    0x040000a1,
    0x240000a0,
    0x000004e0,
    0x00000000,
    0x003ff800,
    0x003ff800,
    0x003ff800,
    0x003ff800,
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x03270327,
    0x03270327,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00004007,
  },
  {                 // data3[19] STPM (Config for CT)
    0x040000a0,
    0x240000a0,
    0x000004e0,
    0x00000000,
    0x003ff800,
    0x003ff800,
    0x003ff800,
    0x003ff800,
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x03270327,
    0x03270327,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00004007,
  },
  {                 // data4[19] STPM (Config for CT)
    0x040000a0,
    0x240000a0,
    0x000004e0,
    0x00000000,
    0x003ff800,
    0x003ff800,
    0x003ff800,
    0x003ff800,
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x00000fff,
    0x03270327,
    0x03270327,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00004007,
  },
  {               // powerFact[8]
    30154605,     // ch 1
    30154605,     // ch 2
    30154605,     // ch 3
    30154605,     // ch 4
    30154605,     // ch 5
    30154605,     // ch 6
    30154605,     // ch 7
    30154605      // ch 8
  },
  {               // voltageFact[8]
    116274,       // ch 1
    116274,       // ch 2
    116274,       // ch 3
    116274,       // ch 4
    116274,       // ch 5
    116274,       // ch 6
    116274,       // ch 7
    116274        // ch 8
  },
  {               // currentFact[8]
    25934,        // ch 1
    25934,        // ch 2
    25934,        // ch 3
    25934,        // ch 4
    25934,        // ch 5
    25934,        // ch 6
    25934,        // ch 7
    25934         // ch 8
  }
};


#ifdef UART_XFER_STPM3X
const STPM_Com_port_t STPM_com_port[4] ={
  {
    USART6,     //USART used by device 1
    SCS_A_GPIO_Port,      //CS used by device 1
    SCS_A_Pin,
    SYN_A_GPIO_Port,      //SYN used by device 1
    SYN_A_Pin,
    EN_A_GPIO_Port,      //EN used by device 1
    EN_A_Pin
    },
  {
  USART1,    //USART used by device 2
  SCS_B_GPIO_Port,      //CS used by device 3
  SCS_B_Pin,
  SYN_B_GPIO_Port,      //SYN used by device 3
  SYN_B_Pin,
  EN_B_GPIO_Port,      //EN used by device 3
  EN_B_Pin
  },

  {
  USART2,    //USART used by device 3
  SCS_C_GPIO_Port,     //CS used by device 2
  SCS_C_Pin,
  SYN_C_GPIO_Port,     //SYN used by device 2
  SYN_C_Pin,
  EN_C_GPIO_Port,     //EN used by device 2
  EN_C_Pin
  },
  {
  USART3,    //USART used by device 4
  SCS_D_GPIO_Port,     //CS used by device 4
  SCS_D_Pin,
  SYN_D_GPIO_Port,     //SYN used by device 4
  SYN_D_Pin,
  EN_D_GPIO_Port,     //EN used by device 4
  EN_D_Pin
  }
};
#endif

#ifdef SPI_XFER_STPM3X /* SPI MODE */   

const STPM_Com_port_t STPM_com_port[4] ={  
  {
  SPI2,    //SPI used by device 1
  GPIOE,     //CS used by device 1
  GPIO_PIN_14,
  GPIOE,     //SYN used by device 1
  GPIO_PIN_15,
  GPIOE,     //EN used by device 1
  GPIO_PIN_13
  },
  {
  SPI3,     //SPI used by device 2
  GPIOC,      //CS used by device 2
  GPIO_PIN_8,
  GPIOC,      //SYN used by device 2
  GPIO_PIN_9,
  GPIOA,      //EN used by device 2
  GPIO_PIN_8
  },
  {
  SPI1,    //SPI used by device 3
  GPIOA,     //CS used by device 3
  GPIO_PIN_0,
  GPIOA,     //SYN used by device 3
  GPIO_PIN_1,
  GPIOC,     //EN used by device 3
  GPIO_PIN_3
  },
  {
  SPI1,     //SPI used by device 4
  GPIOA,     //CS used by device 4
  GPIO_PIN_0,
  GPIOA,     //SYN used by device 4
  GPIO_PIN_1,
  GPIOC,     //EN used by device 4
  GPIO_PIN_3
  }
};

#endif

/*******************************************************************************
* GLOBAL VARIABLES:
*******************************************************************************/
extern metroData_t metroData;
extern METRO_Device_Config_t Tab_METRO_internal_Devices_Config[NB_MAX_DEVICE];

/*******************************************************************************
* LOCAL VARIABLES:
*******************************************************************************/

/*******************************************************************************
* LOCAL FUNCTION PROTOTYPES:
*******************************************************************************/
static uint8_t  MET_CheckConfigInNVM();
static void     MET_ApplyNvmConfig(void);
static void     MET_UpdateNvmConfig(void);


/*******************************************************************************
*
*                       IMPLEMENTATION: Public functions
*
*******************************************************************************/
/**
  * @brief  Configures the Metrology peripheral.
  * @param  None
  * @retval None
  */
void MET_Conf(void)
{
    metroData.nvm = (nvmLeg_t*)NVM_GetPtr(NVRAM_LEG_ID);
    if (MET_CheckConfigInNVM() == 0)
    {
      memcpy(metroData.nvm, &metroDefaultNvm, sizeof(nvmLeg_t));
      metroData.nbPhase = 3;
    }
}

/**
  * @brief  Initialize the Port to communicated to Metrology peripheral
  *         
  * @param[in]   None
  * @retval None
  */

void Metro_com_port_device(void)
{

  for (METRO_NB_Device_t i=EXT1;i<(NB_MAX_DEVICE);i++)
  {
    if(Tab_METRO_internal_Devices_Config[i].device != 0)
    {
      Tab_METRO_internal_Devices_Config[i].STPM_com_port= STPM_com_port[(i-1)];
    }
  }

}

/**
  * @brief  restore Metrology configuration from NVM
  * @param  None
  * @retval 0 if KO - 1 if OK
  */
void MET_RestoreConfigFromNVM( void )
{
  if (MET_CheckConfigInNVM())
  {
    MET_ApplyNvmConfig();
  }
  else
  {
    MET_RestoreDefaultConfig(3);
  }
}

/**
  * @brief  restore Metrology configuration from default
  * @param  None
  * @retval None
  */

void MET_RestoreDefaultConfig(uint32_t nbPhase)
{
  memcpy(metroData.nvm, &metroDefaultNvm, sizeof(nvmLeg_t));
  if (nbPhase == 1)
  {
    metroData.nvm->config[1] = 0x16;
  }
  MET_ApplyNvmConfig();
}

/**
  * @brief  save current Metrology configuration to NVM
  * @param  None
  * @retval None
  */
void MET_SaveConfigToNVM( void )
{
  MET_UpdateNvmConfig();
  NVM_Write(NVM_SAVE_LEG);
}


/*******************************************************************************
*
*                       IMPLEMENTATION: Private functions
*
*******************************************************************************/
/**
  * @brief  checks for a valid  Metrology configuration in NVM
  * @param  None
  * @retval Status : 0 if KO - 1 if OK
  */
static uint8_t MET_CheckConfigInNVM()
{
  int i;
  if (metroData.nvm->marker != NVRAM_MARKER_LEG)
  {
    return(0);
  }
  metroData.nvm->crc = 0;

  if (metroData.nvm->config[0] != 0x00000005)          // STM32
  {
    metroData.nvm->config[0] = 0x00000005;
    metroData.nvm->config[1] = 0x00000016;
  }
  else if (((metroData.nvm->config[1] & (DEVICE_MASK_CONF + CHANNEL_MASK_CONF)) != 0x00000016) &&  // STPM32 for phase 1
           ((metroData.nvm->config[1] & (DEVICE_MASK_CONF + CHANNEL_MASK_CONF)) != 0x00000097) &&  // STPM33 for phase 1 & Tamper
           ((metroData.nvm->config[1] & (DEVICE_MASK_CONF + CHANNEL_MASK_CONF)) != 0x00000038))    // STPM34 for phase 1 & 2
  {
    metroData.nvm->config[1] = 0x00000016;
  }
  else if (((metroData.nvm->config[1] & (DEVICE_MASK_CONF + CHANNEL_MASK_CONF)<<8) != 0x00002600) &&  // STPM32 for phase 2
           ((metroData.nvm->config[1] & (DEVICE_MASK_CONF + CHANNEL_MASK_CONF)<<8) != 0x0000A700) &&  // STPM33 for phase 2 & Tamper
           ((metroData.nvm->config[1] & (DEVICE_MASK_CONF + CHANNEL_MASK_CONF)<<8) != 0x00005800))    // STPM34 for phase 2 & 3
  {
    metroData.nvm->config[1] &= ~((DEVICE_MASK_CONF + CHANNEL_MASK_CONF)<<8);
  }
  else if (((metroData.nvm->config[1] & (DEVICE_MASK_CONF + CHANNEL_MASK_CONF)<<16) != 0x00460000) &&  // STPM32 for phase 3
           ((metroData.nvm->config[1] & (DEVICE_MASK_CONF + CHANNEL_MASK_CONF)<<16) != 0x00C70000))  // STPM33 for phase 3 & Tamper
  {
    metroData.nvm->config[1] &= ~((DEVICE_MASK_CONF + CHANNEL_MASK_CONF)<<16);
  }
  else if (((metroData.nvm->config[1] & (uint32_t)((DEVICE_MASK_CONF + CHANNEL_MASK_CONF)<<24)) != 0x86000000))   // STPM32 for phase Tamper
  {
    metroData.nvm->config[1] &= ~((DEVICE_MASK_CONF + CHANNEL_MASK_CONF)<<24);
  }
  
    
   metroData.nbPhase = (uint8_t)((uint8_t)((metroData.nvm->config[1]&(CHANNEL_MASK_CONF_CHANNEL_1<<4))>>4) 
                      + (uint8_t)((metroData.nvm->config[1]&(CHANNEL_MASK_CONF_CHANNEL_2<<4))>>5));
   metroData.nbPhase += (uint8_t)((uint8_t)((metroData.nvm->config[1]&(CHANNEL_MASK_CONF_CHANNEL_2<<12))>>13) 
                       + (uint8_t)((metroData.nvm->config[1]&(CHANNEL_MASK_CONF_CHANNEL_3<<12))>>14));
   metroData.nbPhase += (uint8_t)((uint8_t)((metroData.nvm->config[1]&(CHANNEL_MASK_CONF_CHANNEL_3<<20))>>22)); 


   // check for power factor - force to default power Factor if not
  for (i=0; i<4; i++)
  {
    if ((metroData.nvm->powerFact[i] == 0) || (metroData.nvm->powerFact[i] == 0xFFFFFFFF))
    {
      metroData.nvm->powerFact[i] = metroDefaultNvm.powerFact[i];
    }
  }
  return(1);
}

/**
  * @brief  Apply NVM config to various devices
  * @param  None
  * @retval None
  */
static void MET_ApplyNvmConfig(void)
{

  if ((metroData.nvm->config[1] && (DEVICE_MASK_CONF<<24)) != 0)
  {
    /* Check if Config device inside RAM can permit access to EXT chip */
    if ((Tab_METRO_internal_Devices_Config[EXT4].device >= STPM32)&&(Tab_METRO_internal_Devices_Config[EXT4].device < NB_MAX_STPM))
    {  
      /* write configuration into STPM */
      Metro_Write_Block_to_Device(EXT4, 0, 19, metroData.nvm->data4);
  
      /* Read back configuration to show the read block access */
      Metro_Read_Block_From_Device(EXT4, 0, 19, (uint32_t *)&Tab_METRO_internal_Devices_Config[EXT4].metro_stpm_reg);    
    }
  }

  if ((metroData.nvm->config[1] && (DEVICE_MASK_CONF<<16)) != 0)
  {
    /* Check if Config device inside RAM can permit access to EXT chip */
    /* Check if Config device inside RAM can permit access to EXT chip */
    if ((Tab_METRO_internal_Devices_Config[EXT3].device >= STPM32)&&(Tab_METRO_internal_Devices_Config[EXT3].device < NB_MAX_STPM))
    {  
      /* write configuration into STPM */
      Metro_Write_Block_to_Device(EXT3, 0, 19, metroData.nvm->data3);
  
      /* Read back configuration to show the read block access */
      Metro_Read_Block_From_Device(EXT3, 0, 19, (uint32_t *)&Tab_METRO_internal_Devices_Config[EXT3].metro_stpm_reg);    
    }
  }

  if ((metroData.nvm->config[1] && (DEVICE_MASK_CONF<<8)) != 0)
  {
    /* Check if Config device inside RAM can permit access to EXT chip */
    /* Check if Config device inside RAM can permit access to EXT chip */
    if ((Tab_METRO_internal_Devices_Config[EXT2].device >= STPM32)&&(Tab_METRO_internal_Devices_Config[EXT2].device < NB_MAX_STPM))
    {  
      /* write configuration into STPM */
      Metro_Write_Block_to_Device(EXT2, 0, 19, metroData.nvm->data2);
  
      /* Read back configuration to show the read block access */
      Metro_Read_Block_From_Device(EXT2, 0, 19, (uint32_t *)&Tab_METRO_internal_Devices_Config[EXT2].metro_stpm_reg);    
    }
  }
   
  if ((metroData.nvm->config[1] && DEVICE_MASK_CONF) != 0)
  {
    /* Check if Config device inside RAM can permit access to EXT chip */
    if ((Tab_METRO_internal_Devices_Config[EXT1].device >= STPM32)&&(Tab_METRO_internal_Devices_Config[EXT1].device < NB_MAX_STPM))
    {  
      /* write configuration into STPM */
      Metro_Write_Block_to_Device(EXT1, 0, 19, metroData.nvm->data1);
  
      /* Read back configuration to show the read block access */
      Metro_Read_Block_From_Device(EXT1, 0, 19, (uint32_t *)&Tab_METRO_internal_Devices_Config[EXT1].metro_stpm_reg);    
    }
    else /*  not correct init made or no chip detected */
    {
    /* reset to single phase the NVRAM config */
    metroData.nvm->config[0] = 0x00000005;
    metroData.nvm->config[1] = 0x00000016;
    }
  }
}

/**
  * @brief  Update NVM config by reading from various devices
  * @param  None
  * @retval None
  */

static void MET_UpdateNvmConfig(void)
{
  if ((metroData.nvm->config[1] & DEVICE_MASK_CONF) != 0)
  {
    Metro_Read_Block_From_Device(EXT1, 0, 19, metroData.nvm->data1 );
  }

  if ((metroData.nvm->config[1] & (DEVICE_MASK_CONF<<8)) != 0)
  {
    Metro_Read_Block_From_Device(EXT2, 0, 19, metroData.nvm->data2 );
  }

  if ((metroData.nvm->config[1] & (DEVICE_MASK_CONF<<16)) != 0)
  {
    Metro_Read_Block_From_Device(EXT3, 0, 19, metroData.nvm->data3 );
  }

  if ((metroData.nvm->config[1] & (DEVICE_MASK_CONF<<24)) != 0)
  {
    Metro_Read_Block_From_Device(EXT4, 0, 19, metroData.nvm->data4 );
  }
}

/**
  * @}
  */

/* End Of File */
