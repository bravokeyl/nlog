/**
*   @file      metroTask.c
*   @author    STMicroelectronics
*   @version   V1.0
*   @date      17 May 2016
*   @brief     This source code includes Metrology legal task related functions
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

/*******************************************************************************
* INCLUDE FILES:
*******************************************************************************/
#include "metroTask.h"
#include "metrology.h"
#include "handler_metrology.h"
//#include "mnsh_tx.h"
#include "string.h"

#include <stdint.h>
#include "st_device.h"

/** @addtogroup LEGAL
  * @{
  */

/*******************************************************************************
* CONSTANTS & MACROS:
*******************************************************************************/

#define FACTOR_POWER_ON_ENERGY      (858)   // (3600 * 16000000 / 0x4000000) = 858.3...



/*******************************************************************************
* TYPES:
*******************************************************************************/

/*******************************************************************************
* GLOBAL VARIABLES:
*******************************************************************************/
metroData_t metroData;
METRO_Device_Config_t Tab_METRO_Global_Devices_Config[NB_MAX_DEVICE];

extern METRO_Device_Config_t Tab_METRO_internal_Devices_Config[NB_MAX_DEVICE];

/*******************************************************************************
* LOCAL FUNCTION PROTOTYPES:
*******************************************************************************/
static void METRO_UnlockMnsh(void);

static void METRO_HandleMetroDevice(uint32_t *pData);
static void METRO_HandleMetroSetup(uint32_t *pData);
static void METRO_HandleMetroMetro(uint32_t *pData);
static void METRO_HandleMetroCal(uint32_t *pData);
static void METRO_HandleMetroIRQ(uint32_t *pData);
static void METRO_HandleMetroSagSwell(uint32_t *pData);

/*******************************************************************************
* LOCAL VARIABLES:
*******************************************************************************/
char text[100];

/*******************************************************************************
*
*                       IMPLEMENTATION: Public functions
*
*******************************************************************************/


/*******************************************************************************
*
*                       IMPLEMENTATION: Private functions
*
*******************************************************************************/

/**
  * @brief  This function implements the Metrology init
  * @param  None
  * @retval None
  */
void METRO_Init()
{

  MET_Conf();
  
  /* initialization device type and number of channel */
  Metro_Setup(metroData.nvm->config[0],metroData.nvm->config[1]);
  
  /* initialization device communication port */ 
  Metro_com_port_device();
  
  /* Enable for STPM device */
  Metro_power_up_device();
  
  /* initialization steps for STPM device */
  Metro_Init();

#ifdef UART_XFER_STPM3X /* UART MODE */   
  /* Change UART speed for STPM communication between Host and EXT1*/
  Metro_UartSpeed(USART_SPEED); 
#endif
  
  MET_RestoreConfigFromNVM();

  /* Initialize the factors for the computation */
  for(int i=0;i<NB_MAX_CHANNEL;i++)
  {
  Metro_Set_Hardware_Factors( (METRO_Channel_t)(CHANNEL_1+i), metroData.nvm->powerFact[i], metroData.nvm->powerFact[i]/ FACTOR_POWER_ON_ENERGY,metroData.nvm->voltageFact[i],metroData.nvm->currentFact[i]);
  }
  
  for (int i=EXT1;i<(NB_MAX_DEVICE);i++)
  {
    if(Tab_METRO_internal_Devices_Config[i].device != 0)
    {
      /* Set default latch device type inside Metro struct for Ext chips */
      Metro_Register_Latch_device_Config_type((METRO_NB_Device_t)i, LATCH_SW);
    }
  }
}

//void METRO_Task()
//{
//  uint32_t data[2];
//
//  switch (mnshVars.msg.id)
//  {
//    case X_METRO_FACTOR:
//      {
//        /* Get Payload pointer from Payload[0] */
//        /* *pData      = Channel Id  */
//        /* *(pData+1)  = Power factor for channel  */
//        /* *(pData+2)  = Energy Factor for channel  */
//
//        uint32_t * pData = (uint32_t*)(*mnshVars.msg.payload);
//
//        /* Set power, nrj, voltage , current  factors depending of channel requested */
//        Metro_Set_Hardware_Factors((METRO_Channel_t)*pData,*(pData+1),*(pData+2), *(pData+3),*(pData+4));
//      }
//      METRO_UnlockMnsh();
//      break;
//    case X_METRO_RST:
//      {
//        /* Get Payload pointer from Payload[0] */
//        /* (*pData) = 1 : HW_SYN_reset, 2: SW_Reset   */
//        uint32_t * pData = (uint32_t*)(*mnshVars.msg.payload);
//
//        if ((uint8_t)(*pData)==RESET_SYN_SCS)
//         {
//           Metro_Config_Reset(RESET_SYN_SCS);
//         }
//         else if((uint8_t)(*pData)==RESET_SW)
//         {
//           Metro_Config_Reset(RESET_SW);
//         }
//      }
//      METRO_UnlockMnsh();
//      break;
//    case X_METRO_DEVICE:
//      METRO_HandleMetroDevice((uint32_t*)(*mnshVars.msg.payload));
//      METRO_UnlockMnsh();
//      break;
//    case X_METRO_METRO:
//      METRO_HandleMetroMetro((uint32_t*)(*mnshVars.msg.payload));
//      METRO_UnlockMnsh();
//      break;
//    case X_METRO_CAL:
//      METRO_HandleMetroCal((uint32_t*)(*mnshVars.msg.payload));
//      METRO_UnlockMnsh();
//      break;
//    case X_METRO_IRQ:
//      METRO_HandleMetroIRQ((uint32_t*)(*mnshVars.msg.payload));
//      METRO_UnlockMnsh();
//      break;
//     case X_METRO_SAG_SWELL:
//      METRO_HandleMetroSagSwell((uint32_t*)(*mnshVars.msg.payload));
//      METRO_UnlockMnsh();
//      break;
//    case X_METRO_INIT:
//      Metro_Init();
//#ifdef UART_XFER_STPM3X /* UART MODE */
//    /* Change UART speed for STPM communication between Host and EXT1*/
//      Metro_UartSpeed(USART_SPEED);
//#endif
//      METRO_UnlockMnsh();
//      break;
//    case X_METRO_SW_RELEASE:
//      {
//        data[0] = Metro_Get_SW_Rev();
//        MNSH_Printf("Metrology Version :  %08x\n",data[0]);
//      }
//      METRO_UnlockMnsh();
//      break;
//    case X_METRO_PING:
//      {
//        if (Metro_Ping_Metro() == 0)
//        {
//           /* Ping is OK, send 0x5A to MNSH print console */
//           data[0] = 0x5A;
//        }
//        else
//        {
//           /* Ping is KO, send 0xFF to MNSH print console */
//           data[0] = 0xFF;
//        }
//
//        MNSH_Printf("Metrology PING :  0x%08x\n",data[0]);
//      }
//      METRO_UnlockMnsh();
//      break;
//    case X_METRO_RD_REG:
//      {
//        int i;
//        /* Max Buffer size =  50*U32 MAX ->  GUI or Metrology Application get Config or Status  group not all registers in one read*/
//        uint32_t buffer[70];
//        /* Get Payload pointer from Payload[0] */
//        /* Payload  DATA  : [0] = Device Id, [1] = @ , [2] Size */
//        /* (*pData) = DeviceID, (u8)(*(pData+1)) = Offset @ , (u8)(*(pData+2)) = Nb U32 registers to read */
//        uint32_t * pData = (uint32_t*)(*mnshVars.msg.payload);
//        strcpy(text, "Metrology REG OFFSET 0x%04x: 0x%08x\n");
//
//        /* Read the Number of register at Offset address inside the device Requested */
//        if ((*pData < (NB_MAX_DEVICE))&&(Tab_METRO_internal_Devices_Config[*pData].device != 0))
//          Metro_Read_Block_From_Device((METRO_NB_Device_t)(*pData), (uint8_t)(*(pData+1)), (uint8_t)(*(pData+2)), buffer);
//
//        /* Print inside MNSH  each 32 bit regsiter read from Device  : One line for each register */
//        for (i=0; i< (*(pData+2)); i++)
//        {
//          /* if Device Id is an EXT chip tha address is based on U16 and not on U32 offset */
//          data[0] = (uint8_t)(*(pData+1) + (i<<1));
//          data[1] = buffer[i];
//
//          MNSH_Printf(text,data[0],data[1]);
//        }
//      }
//      METRO_UnlockMnsh();
//      break;
//    case X_METRO_WR_REG:
//      {
//        /* Get Payload pointer from Payload[0] */
//        /* Payload  DATA  : [0] = Device Id, [1] = @ , [2] Size  [3 to size] Data to write*/
//        /* (*pData) = DeviceID, (u8)(*(pData+1)) = Offset @ , (u8)(*(pData+2)) = Nb U32 registers to write, (u32)(*(pData+3) pointer of Data to write) */
//        uint32_t * pData = (uint32_t*)(*mnshVars.msg.payload);
//
//        /* Write the Number of register at Offset address inside the device Requested */
//        if ((*pData < (NB_MAX_DEVICE))&&(Tab_METRO_internal_Devices_Config[*pData].device != 0))
//          Metro_Write_Block_to_Device((METRO_NB_Device_t)(*pData), (uint8_t)(*(pData+1)), (uint8_t)(*(pData+2)), pData+3);
//
//        mnshVars.msg.id = (msgId_t)0;
//        mnshVars.lockRXNE = 0;
//
//      }
//      METRO_UnlockMnsh();
//      break;
//    case X_METRO_SETUP:
//      METRO_HandleMetroSetup((uint32_t*)(*mnshVars.msg.payload));
//      METRO_UnlockMnsh();
//      break;
//    case X_METRO_CLOSE:
//      METRO_UnlockMnsh();
//      break;
//    case X_METRO_CONFIG_INIT:
//      MET_RestoreDefaultConfig(mnshVars.msg.payload[0]);
//      METRO_UnlockMnsh();
//      break;
//    case X_METRO_CONFIG_REST:
//      MET_RestoreConfigFromNVM();
//      METRO_UnlockMnsh();
//      break;
//    case X_METRO_CONFIG_SAVE:
//      MET_SaveConfigToNVM();
//      METRO_UnlockMnsh();
//      break;
//    case X_METRO_PROD_TEST_PARAM:
//      METRO_UnlockMnsh();
//      break;
//    }
//}
  
/**
  * @brief  This function implements the Metrology latch device
  *         Set the HW latch for next update
  * @param  None
  * @retval None
  */
void METRO_Latch_Measures()
{
  METRO_NB_Device_t i;

  for (i=EXT1;i<(NB_MAX_DEVICE);i++)
  {
    if(Tab_METRO_internal_Devices_Config[i].device != 0)
    {
      Metro_Set_Latch_device_type(i,LATCH_SW);
    }
  }
}

/**
  * @brief  This function implements the Metrology get DSP data inside device
  * @param  None
  * @retval None
  */
void METRO_Get_Measures()
{
  METRO_NB_Device_t i;

  for (i=EXT1;i<(NB_MAX_DEVICE);i++)
  {
    if(Tab_METRO_internal_Devices_Config[i].device != 0)
    {
      Metro_Get_Data_device(i);
    }
  }

}

/**
  * @brief  This function unlock minishell after message treatment
  * @param  None
  * @retval None
  */

//static void METRO_UnlockMnsh(void)
//{
//  mnshVars.lockRXNE = 0;
//  mnshVars.msg.id = X_MNSH_UNLOCKRX_EVENT;
//}

/**
  * @brief  This function updates the Metro measurements values
  * @param  None
  * @retval None
  */
void METRO_UpdateData(void)
{
  metroData.powerActive    = 0;
  metroData.powerReactive  = 0;
  metroData.powerApparent  = 0;

  for(int i=0;i<metroData.nbPhase;i++)
  {
    metroData.rawEnergyExt[METRO_DATA_ACTIVE  ][METRO_PHASE_1+i] = Metro_Read_energy((METRO_Channel_t)(CHANNEL_1+i), E_W_ACTIVE);
    metroData.rawEnergyExt[METRO_DATA_REACTIVE  ][METRO_PHASE_1+i] = Metro_Read_energy((METRO_Channel_t)(CHANNEL_1+i), E_REACTIVE);
    metroData.rawEnergyExt[METRO_DATA_APPARENT  ][METRO_PHASE_1+i] = Metro_Read_energy((METRO_Channel_t)(CHANNEL_1+i), E_APPARENT);

    metroData.chanPower[METRO_DATA_ACTIVE  ][METRO_PHASE_1+i] = Metro_Read_Power((METRO_Channel_t)(CHANNEL_1+i), W_ACTIVE);
    metroData.chanPower[METRO_DATA_REACTIVE][METRO_PHASE_1+i] = Metro_Read_Power((METRO_Channel_t)(CHANNEL_1+i), REACTIVE);
    metroData.chanPower[METRO_DATA_APPARENT][METRO_PHASE_1+i] = Metro_Read_Power((METRO_Channel_t)(CHANNEL_1+i), APPARENT_RMS);
    metroData.powerActive    += metroData.chanPower[METRO_DATA_ACTIVE  ][METRO_PHASE_1+i];
    metroData.powerReactive  += metroData.chanPower[METRO_DATA_REACTIVE][METRO_PHASE_1+i];
    metroData.powerApparent  += metroData.chanPower[METRO_DATA_APPARENT][METRO_PHASE_1+i];
  }

}

static void METRO_HandleMetroDevice(uint32_t *pData)
{
  /* Get Payload pointer from Payload[0] */
  /* Payload  DATA  : [0] = Device service, [1] = set (0) or get(1)   , [2]...[N] params */
  /* Device service (*pData) :   */
  /*   - 0 :  Set baude rate service      */
  /*   - 1 :  Config latch service      */
  /*   - 2 :  Trig Latch ALL devices At the same time   */
  /*   - 3 :  Set/Get ZCR Config and ZCR Enable/Disable bit*/
  /*   - 4 :  Set/Get CLK Config and CLK Enable/Disable bit*/
  /*   - 5 :  Set/Get LEd Configs and LED Enable/Disable bit*/
  /*   - 6 :  Set/Get STPM  LINK IRQ management */
  /*   - 7 :  Set(Clear) STPM LINK  IRQ / Get IRQ Status */

  uint32_t data[2] = {0,0};
  METRO_NB_Device_t i;

  switch (*pData)
  {
    /* UART baud rate device service */
  case (0):
    {
      /* test set or get request */
      if (*(pData+1) == 0)
      {
        /* it is a set baud rate service requested */
        /* get the two next params in the cmd line : (*pData+2) = Device Id , (*pData+3) = baudrate */
        /* (*pData+2) : Device Id :   HOST=0, EXT1 = 1,  EXT2 = 2,  EXT3 = 3,  EXT4 = 4 */
        /* (*pData+3) : baud rate :   U32 value : */
        Metro_Set_uart_baudrate_to_device((METRO_NB_Device_t)*(pData+2), *(pData+3));
      }
      else if (*(pData+1) == 1)
      {
        /* it is a get */
        /* no get service for the moment */
      }
    }
    break;

     /* Config Latch device service */
  case (1):
    {
      /* test set or get request */
      /* it is a set */
      if (*(pData+1) == 0)
      {
        /* it is a set latch service requested */
        /* get the two next params in the cmd line : (*pData+2) = Device Id , (*pData+3) = Lact type requested */
        /* *(pData+2) : Device Id :   HOST=0, EXT1 = 1,  EXT2 = 2,  EXT3 = 3,  EXT4 = 4 */
        /* *(pData+3) : Latch type :   LATCH_SYN_SCS = 1,  LATCH_SW = 2, LATCH_AUTO = 3 */

         /* Save latch device type inside Metro struct */
        Metro_Register_Latch_device_Config_type((METRO_NB_Device_t)(*(pData+2)), (METRO_Latch_Device_Type_t)(*(pData+3)));

      }
    }
    break;
     /* Trig Latch ALL devices At the same time */
  case (2):
    {
        /* it is a trig latch  service requested */
        /* latch all devices on the system */
        for(i=EXT1;i<(NB_MAX_DEVICE);i++)
        {      
          if(Tab_METRO_internal_Devices_Config[i].device != 0)
          {
            Metro_Set_Latch_device_type(i, Tab_METRO_internal_Devices_Config[i].latch_device_type);
          }
        }

        for(i=EXT1;i<(NB_MAX_DEVICE);i++)
        {      
          if(Tab_METRO_internal_Devices_Config[i].device != 0)
          {
            Metro_Get_Data_device(i);
          }
        }

    }
    break;

      /*  Set/Get ZCR Config and ZCR Enable/Disable bit*/
  case (3):
    {
      /* test set or get request */
      /* It is a set */
      if (*(pData+1) == 0)
      {
        /* it is a metro set  ZCR config requested */
        /* get the  next param in the cmd line : *(pData+2) = Device */
        /* get the  next param in the cmd line : *(pData+3) = ZCR Config change : 1 , ZCR enable change = 2*/

        /* get the  next param in the cmd line : *(pData+4) = ZCR sel value  = ZCR_SEL_V1 = 0,  ZCR_SEL_C1, ZCR_SEL_V2, ZCR_SEL_C2  */
        /*or */
        /* get the  next param in the cmd line : *(pData+4) = ZCR  Enable Bit  = 0 or 1 */

        /* ZCR change requested */
        if ( *(pData+3) == 1)
        {
          /* Ask Metro driver to set the new ZCR  config */
          Metro_Set_ZCR((METRO_NB_Device_t)*(pData+2), (METRO_ZCR_Sel_t) *(pData+4),NO_CHANGE);

        }
        /* Enable/Disable ZCR requested */
        else if ( *(pData+3) == 2)
        {
          /* Ask Metro driver to set the new ZCR enable bit config*/
          Metro_Set_ZCR((METRO_NB_Device_t)*(pData+2),NO_CHANGE_ZCR,(METRO_CMD_Device_t) *(pData+4));
        }
      }
      /* It is a get */
      else if (*(pData+1) == 1)
      {
        /*  it is a metro Get ZCR config requested */
        /* get the  next param in the cmd line : *(pData+2) = Device   */
        /* get the  next param in the cmd line : *(pData+3) = ZCR config requested : 1 , ZCR enable bit Requested = 2*/

        /*  ZCR config requested */
        if (*(pData+3) == 1)
        {
          /* GEt and Display ZCR config  */
          strcpy(text, "Metro ZCR config  = 0x%02x \n");
          data[1] = Metro_Get_ZCR((METRO_NB_Device_t)*(pData+2),(METRO_ZCR_Sel_t*)&data[0]);
        }
        /*  Get Enable ZCR bit  requested*/
        else if (*(pData+3) == 2)
        {
          /* Display ZCR Enable bit */
          strcpy(text, "Metro ZCR Config Enable bit = 0x%02x \n");
          data[0] = Metro_Get_ZCR((METRO_NB_Device_t)*(pData+2),(METRO_ZCR_Sel_t*)&data[1]);
        }
        MNSH_Printf(text,data[0],data[1]);
      }
     }
    break;

      /*  Set/Get CLK Config and CLK Enable/Disable bit*/
  case (4):
    {
      /* test set or get request */
      /* It is a set */
      if (*(pData+1) == 0)
      {
        /* it is a metro set  CLK config requested */
        /* get the  next param in the cmd line : *(pData+2) = Device */
        /* get the  next param in the cmd line : *(pData+3) = CLK Config change : 1 , CLK enable change = 2*/

        /* get the  next param in the cmd line : *(pData+4) = CLK sel value  ->  CLK_SEL_7KHz = 0,  CLK_SEL_4MHz, CLK_SEL_4MHz_50,CLK_SEL_16MHz  */
        /*or */
        /* get the  next param in the cmd line : *(pData+4) = CLK  Enable Bit  = 0 or 1 */

        /* CLK change requested */
        if ( *(pData+3) == 1)
        {
          /* Ask Metro driver to set the new CLK  config */
          Metro_Set_CLK((METRO_NB_Device_t)*(pData+2), (METRO_CLK_Sel_t) *(pData+4),NO_CHANGE);

        }
        /* Enable/Disable CLK requested */
        else if ( *(pData+3) == 2)
        {
          /* Ask Metro driver to set the new CLK enable bit config*/
          Metro_Set_CLK((METRO_NB_Device_t)*(pData+2),NO_CHANGE_CLK,(METRO_CMD_Device_t) *(pData+4));
        }
      }
      /* It is a get */
      else if (*(pData+1) == 1)
      {
        /*  it is a metro Get CLK config requested */
        /* get the  next param in the cmd line : *(pData+2) = Device   */
        /* get the  next param in the cmd line : *(pData+3) = CLK config requested : 1 , CLK enable bit Requested = 2*/

        /*  CLK config requested */
        if (*(pData+3) == 1)
        {
          /* GEt and Display CLK config  */
          strcpy(text, "Metro CLK config  = 0x%02x \n");
          data[1] = Metro_Get_CLK((METRO_NB_Device_t)*(pData+2),(METRO_CLK_Sel_t*)&data[0]);
        }
        /*  Get Enable CLK bit  requested*/
        else if (*(pData+3) == 2)
        {
          /* Display CLK Enable bit */
          strcpy(text, "Metro CLK Config Enable bit = 0x%02x \n");
          data[0] = Metro_Get_CLK((METRO_NB_Device_t)*(pData+2),(METRO_CLK_Sel_t*)&data[1]);
        }
        MNSH_Printf(text,data[0],data[1]);
      }
     }
    break;
      /*  Set/Get LEd Configs and LED Enable/Disable bit*/
  case (5):
    {
      /* test set or get request */
      /* It is a set */
      if (*(pData+1) == 0)
      {
        /* it is a metro set  LEds config requested */
        /* get the  next param in the cmd line : *(pData+2) = Device */
        /* get the  next param in the cmd line : *(pData+3) = Led Number : 1 or 2  */
        /* get the  next param in the cmd line : *(pData+4) = LEd Set Config =
                                                                Power  =  1 ,
                                                                Channel = 2 ,
                                                                Speed_Divisor = 3,
                                                                ON_OFF  = 4*/


        /* get the  next param in the cmd line : *(pData+5) = Power value  ->  LED_W_ACTIVE = 0,  LED_F_ACTIVE, LED_REACTIVE, LED_APPARENT_RMS   */

        /*or */

        /* get the  next param in the cmd line : *(pData+5) = Channel  value  ->  PRIMARY = 0,  SECONDARY, ALGEBRIC, SIGMA_DELTA   */

        /*or */

        /* get the  next param in the cmd line : *(pData+5) = Speed divisor  value  -> u8 :  0 to 15   */

        /* Or */

        /* get the  next param in the cmd line : *(pData+5) = LEd  Enable Bit  = 0 or 1 */

        /* Power change requested */
        if ( *(pData+4) == 1)
        {
           Metro_Set_Led_Power_Config((METRO_NB_Device_t)*(pData+2), (METRO_LED_Selection_t) *(pData+3),(METRO_LED_Power_selection_t) *(pData+5));
        }
        /* Channel change requested */
        else if ( *(pData+4) == 2)
        {
           Metro_Set_Led_Channel_Config((METRO_NB_Device_t)*(pData+2), (METRO_LED_Selection_t) *(pData+3),(METRO_LED_Channel_t) *(pData+5));
        }
        /* Led Divisor change requested */
        else if ( *(pData+4) == 3)
        {
          Metro_Set_Led_Speed_divisor((METRO_NB_Device_t)*(pData+2), (METRO_LED_Selection_t) *(pData+3),(uint8_t) *(pData+5));
        }
        /* Enable/Disable Led requested */
        else if ( *(pData+4) == 4)
        {
          Metro_Set_Led_On_Off((METRO_NB_Device_t)*(pData+2), (METRO_LED_Selection_t) *(pData+3),(METRO_CMD_Device_t) *(pData+5));
        }
      }
      /* It is a get */
      else if (*(pData+1) == 1)
      {
        /*  it is a metro Get LED config requested */
        /* get the  next param in the cmd line : *(pData+2) = Device   */
        /* get the  next param in the cmd line : *(pData+3) = Led Number : 1 or 2  */
        /* get the  next param in the cmd line : *(pData+4) = LEd Set Config =
                                                                Power  =  1 ,
                                                                Channel = 2 ,
                                                                Speed_Divisor = 3,
                                                                ON_OFF  = 4  */

        uint32_t Led_Power = 0;
        uint32_t Led_Channel = 0;

         /* Power change requested */
        if ( *(pData+4) == 1)
        {
          strcpy(text, "Metro Led Power config  = 0x%02x \n");
          Metro_Get_Led_Power_Config((METRO_NB_Device_t)(*(pData+2)), (METRO_LED_Selection_t) (*(pData+3)),(METRO_LED_Power_selection_t*) (&Led_Power));
          data[0] = Led_Power;
        }
        /* Channel change requested */
        else if ( *(pData+4) == 2)
        {
          strcpy(text, "Metro Led Channel config  = 0x%02x \n");
          Metro_Get_Led_Channel_Config((METRO_NB_Device_t)*(pData+2), (METRO_LED_Selection_t) *(pData+3),(METRO_LED_Channel_t*) &Led_Channel);
          data[0] = Led_Channel;
        }
        /* Led Divisor change requested */
        else if ( *(pData+4) == 3)
        {
          strcpy(text, "Metro Led Divider config  = 0x%02x \n");
          data[0] = Metro_Get_Led_Speed_divisor((METRO_NB_Device_t)*(pData+2), (METRO_LED_Selection_t) *(pData+3));
        }
        /* Enable/Disable Led requested */
        else if ( *(pData+4) == 4)
        {
          strcpy(text, "Metro Led On / Off  config  = 0x%02x \n");
          data[0]= Metro_Get_Led_On_Off((METRO_NB_Device_t)*(pData+2), (METRO_LED_Selection_t) *(pData+3));
        }

        MNSH_Printf(text,data[0],data[1]);
      }
     }
    break;
    /* Set/Get STPM  LINK IRQ management*/
    case (6):
    {
      /* test set or get request */

      /* Set IRQ mask*/
      if (*(pData+1) == 0)
      {
        /* it is a  Set IRQ mask */
        /* get the  next param in the cmd line : *(pData+2) = Device */
        /* get the  next param in the cmd line : *(pData+3) = u32 Metro_IT_Mask */

        /* Ask Metro driver to set the new IRQ STPM mask value */
        Metro_Set_IRQ_Mask_for_STPM_device((METRO_NB_Device_t)*(pData+2), (uint16_t) *(pData+3));

      }
      /* Get IRQ Mask*/
      else if (*(pData+1) == 1)
      {
        /* it is a cal  Get  IRQ mask */
        /* get the  next param in the cmd line : *(pData+2) = device */

        strcpy(text, "Get  LINK IRQ mask from device %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Get_IRQ_Mask_from_STPM_device((METRO_NB_Device_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;

    /* Set(Clear) STPM LINK IRQ / Get IRQ Status*/
    case (7):
    {
      /* test set or get request */

      /* Set (Clear IRQ) */
      if (*(pData+1) == 0)
      {
        /* it is a  Clear IRQ  service */
        /* get the  next param in the cmd line : *(pData+2) = Device */
        /* get the  next param in the cmd line : *(pData+3) = METRO_STPM_LINK_IRQ_Status_Type_t Metro_Status_requested*/
        /*
            METRO_STPM_LINK_IRQ_Status_Type_t  =

            ALL_STPM_LINK_STATUS = 0,
            STATUS_STPM_UART_LINK_BREAK,
            STATUS_STPM_UART_LINK_CRC_ERROR,
            STATUS_STPM_UART_LINK_TIME_OUT_ERROR,
            STATUS_STPM_UART_LINK_FRAME_ERROR,
            STATUS_STPM_UART_LINK_NOISE_ERROR,
            STATUS_STPM_UART_LINK_RX_OVERRUN,
            STATUS_STPM_UART_LINK_TX_OVERRUN,
            STATUS_STPM_SPI_LINK_RX_FULL,
            STATUS_STPM_SPI_LINK_TX_EMPTY,
            STATUS_STPM_LINK_READ_ERROR,
            STATUS_STPM_LINK_WRITE_ERROR,
            STATUS_STPM_SPI_LINK_CRC_ERROR,
            STATUS_STPM_SPI_LINK_UNDERRUN,
            STATUS_STPM_SPI_LINK_OVERRRUN */

        /* Ask Metro driver to clear the IRQ status requested */
        Metro_Clear_Status_for_STPM_device((METRO_NB_Device_t)*(pData+2), (METRO_STPM_LINK_IRQ_Status_Type_t) *(pData+3));

      }
      /* Get IRQ status */
      else if (*(pData+1) == 1)
      {
        /* it is a Get IRQ status service requested*/
        /* get the  next param in the cmd line : *(pData+2) = Device */
        /* get the  next param in the cmd line : *(pData+3) = METRO_STPM_LINK_IRQ_Status_Type_t Metro_Status_requested*/
        /*    METRO_STPM_LINK_IRQ_Status_Type_t  =

            ALL_STPM_LINK_STATUS = 0,
            STATUS_STPM_UART_LINK_BREAK,
            STATUS_STPM_UART_LINK_CRC_ERROR,
            STATUS_STPM_UART_LINK_TIME_OUT_ERROR,
            STATUS_STPM_UART_LINK_FRAME_ERROR,
            STATUS_STPM_UART_LINK_NOISE_ERROR,
            STATUS_STPM_UART_LINK_RX_OVERRUN,
            STATUS_STPM_UART_LINK_TX_OVERRUN,
            STATUS_STPM_SPI_LINK_RX_FULL,
            STATUS_STPM_SPI_LINK_TX_EMPTY,
            STATUS_STPM_LINK_READ_ERROR,
            STATUS_STPM_LINK_WRITE_ERROR,
            STATUS_STPM_SPI_LINK_CRC_ERROR,
            STATUS_STPM_SPI_LINK_UNDERRUN,
            STATUS_STPM_SPI_LINK_OVERRRUN */


        strcpy(text, "Metrology Read LINK IRQ Status from Device %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Read_Status_from_STPM_device((METRO_NB_Device_t)*(pData+2),(METRO_STPM_LINK_IRQ_Status_Type_t) *(pData+3));
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;
  }
}

static void METRO_HandleMetroSetup(uint32_t *pData)
{
  /* Get Payload pointer from Payload[0] */
  /* (*pData) = 0 : set, 1: Get , (u8)(*(pData+1)) = Host Config , (u8)(*(pData+2)) = STPM config */

  /****************/
  /* STPM Config  */
  /****************/
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

 |---------|-----------|--------------------------------------------------------------|
  Full SET cmd for this example : met setup 0 0x00000091 0x00000068
 |---------|-----------|--------------------------------------------------------------| */


  /********** SET ************/
  /* If cmd is set , retreive the 2 followings parameters in Cmd line : Host Config , STPM config */
  if ((uint8_t)(*(pData)) == 0)
  {
     /* update config in RAM */
    metroData.nvm->config[0] = *(pData+1);
    metroData.nvm->config[1] = *(pData+2);

    /* setup metrology Driver */
    Metro_Setup(*(pData+1), *(pData+2));
    Metro_Init();

#ifdef UART_XFER_STPM3X /* UART MODE */   
  /* Change UART speed for STPM communication between Host and EXT1*/
    Metro_UartSpeed(USART_SPEED); 
#endif

    MET_RestoreConfigFromNVM();
     
    /* Initialize the factors for the computation */
    for(int i=0;i<NB_MAX_CHANNEL;i++)
    {
      Metro_Set_Hardware_Factors( (METRO_Channel_t)(CHANNEL_1+i), metroData.nvm->powerFact[i], metroData.nvm->powerFact[i]/ FACTOR_POWER_ON_ENERGY,metroData.nvm->voltageFact[i],metroData.nvm->currentFact[i]);
    }

    for (int i=EXT1;i<(NB_MAX_DEVICE);i++)
    {
      if(Tab_METRO_internal_Devices_Config[i].device != 0)
      {
        /* Set default latch device type inside Metro struct for Ext chips */
        Metro_Register_Latch_device_Config_type((METRO_NB_Device_t)i, LATCH_SYN_SCS);
      }
    }

  }
  /********** GET ************/
  else if ((uint8_t)(*(pData)) == 1)/* It is a get cmd, retreive information from Global table and send the config to mnsh Uart  */
  {
    /* Max Buffer size =  50*U32 MAX ->  GUI or Metrology Application get Config or Status  group not all registers in one read*/
    uint32_t data[2] = {0,0};
    strcpy(text, "Metrology GET SETUP - HOST CONFIG :0x%08x: STPM CONFIG:0x%08x\n");

    /* Get setup from Metro Driver and save it in the Global Table of Mnsh metro task*/
    Metro_Get_Setup(&data[0],&data[1]);

    /* Print inside MNSH  the two 32 bit variables : Host Config and STPM config  */
    MNSH_Printf(text,data[0],data[1]);
  }
  /************ CMD ERROR ***********/
  else /* cmd is not 0 or 1, error !!! Wrong Cmd entered by User */
  {

  }
}

static void METRO_HandleMetroMetro(uint32_t *pData)
{
  /* Get Payload pointer from Payload[0] */
  /* Payload  DATA  : [0] = Metro service, [1] = set (0) or get(1)   , [2]...[N] params */
  /* Metro service (*pData) :   */
  /*   - 0 :  Read Period      */
  /*   - 1 :  Read  Power     */
  /*   - 2 :  Read NRJ      */
  /*   - 3 :  Set/Get Current Gain      */
  /*   - 4 : Read Momentary Voltage */
  /*   - 5 : Read Momentatry Current */
  /*   - 6 : Read RMS Voltage and RMS Current */
  /*   - 7 : Read Phase angle */
  /*   - 8 : Set/Get Temperature Compensation */
  /*   - 9 : Set/Get Tamper config */
  /*   - 10 : Set/Get Vref config */
  /*   - 11 : Set/Get HPF filter */
  /*   - 12 : Set/Get LPF filter - Removed */
  /*   - 13 : Set/Get Coil Rogowski integrator */
  /*   - 14:  Set/Get Ah Accumulation Down Threshold*/
  /*   - 15 : Set/Get Ah Accumulation Up Threshold*/
  /*   - 16 : Read AH Acc for channel */

  uint32_t data[2] = {0,0};

  switch (*pData)
  {
    /* read period service*/
  case (0):
    {
      /* test set or get request */
      /* Only Get is implemented */
      if (*(pData+1) == 1)
      {
        /* it is a metro read period service requested */
        /* get the  next param in the cmd line : (*pData+2) = Channel */

        strcpy(text, "Metrology Read period  for Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Read_Period((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);

      }
     }
    break;
    /* read power service*/
  case (1):
    {
      /* test set or get request */
      /* Only Get is implemented */
      if (*(pData+1) == 1)
      {
        /* it is a metro read power service requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel 1, 2 , 3 or 4 */
        /* get the next param in the cmd line  :  *(pData+3) = Power type requested */
        /* power type : W_ACTIVE = 1 , F_ACTIVE = 2, REACTIVE = 3, APPARENT_RMS = 4, APPARENT_VEC = 5, MOM_WIDE_ACT = 6, MOM_FUND_ACT = 7 )*/


        /* build text display according to Power type */
        switch (*(pData+3))
        {
        case (W_ACTIVE):
          {
            strcpy(text, "Metrology Read Power W_ACTIVE for Channel %02x :  0x%08x\n");
          }
          break;
        case (F_ACTIVE):
          {
            strcpy(text, "Metrology Read Power F_ACTIVE for Channel %02x :  0x%08x\n");
          }
          break;
        case (REACTIVE):
          {
            strcpy(text, "Metrology Read Power REACTIVE for Channel %02x :  0x%08x\n");
          }
          break;
        case (APPARENT_RMS):
          {
            strcpy(text, "Metrology Read Power APPARENT_RMS for Channel %02x :  0x%08x\n");
          }
          break;
        case (APPARENT_VEC):
          {
            strcpy(text, "Metrology Read Power  APPARENT_VEC for Channel %02x :  0x%08x\n");
          }
          break;
        case (MOM_WIDE_ACT):
          {
            strcpy(text, "Metrology Read Power MOM_WIDE_ACT for Channel %02x :  0x%08x\n");
          }
          break;
        case (MOM_FUND_ACT):
          {
            strcpy(text, "Metrology Read Power MOM_FUND_ACT for Channel %02x :  0x%08x\n");
          }
          break;
        }
        data[0] = *(pData+2);
        data[1] = Metro_Read_Power((METRO_Channel_t)*(pData+2),(METRO_Power_selection_t)*(pData+3));
        MNSH_Printf(text,data[0],data[1]);

      }
     }
    break;
    /* read Nrj service*/
  case (2):
    {
      /* test set or get request */
      /* Only Get is implemented */
      if (*(pData+1) == 1)
      {
        /* it is a metro read NRJ service requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel 1, 2 , 3 or 4 */
        /* get the next param in the cmd line  :  *(pData+3) = NRJ type requested */
        /* power type : E_W_ACTIVE = 1 , E_F_ACTIVE = 2, E_REACTIVE = 3, E_APPARENT = 4 */

        /* build text display according to Power type */
        switch (*(pData+3))
        {
        case (E_W_ACTIVE):
          {
            strcpy(text, "Metrology Read NRJ W_ACTIVE for Channel %02x :  0x%08x\n");
          }
          break;
        case (E_F_ACTIVE):
          {
            strcpy(text, "Metrology Read NRJ F_ACTIVE for Channel %02x :  0x%08x\n");
          }
          break;
        case (E_REACTIVE):
          {
            strcpy(text, "Metrology Read NRJ REACTIVE for Channel %02x :  0x%08x\n");
          }
          break;
        case (E_APPARENT):
          {
            strcpy(text, "Metrology Read NRJ APPARENT for Channel %02x :  0x%08x\n");
          }
          break;

        }
        data[0] = *(pData+2);
        data[1] = Metro_Read_energy((METRO_Channel_t)*(pData+2),(METRO_Energy_selection_t)*(pData+3));
        MNSH_Printf(text,data[0],data[1]);

      }
     }
    break;
  /*Current gain */
  case (3):
    {
      /* test set or get request */

      /* Set */
      if (*(pData+1) == 0)
      {
        /* it is a metro set Current gain requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = Gain Value METRO_Gain_t type = 1, 2, 3, 4 (  x2 , x4, x8, x16) */

        /* Ask Metro driver to set the new Current gain */
        Metro_Set_Current_gain((METRO_Channel_t)*(pData+2), (METRO_Gain_t) *(pData+3));
      }
      /* Get */
      else if (*(pData+1) == 1)
      {
        /* it is a metro Get Current gain requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        strcpy(text, "Metrology Read Current Gain  from Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Get_Current_gain((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
     }

     }
    break;
     /* read Momentary Voltage  service*/
  case (4):
    {
      /* test set or get request */
      /* Only Get is implemented */
      if (*(pData+1) == 1)
      {
        /* it is a metro read M Voltage service requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel 1, 2 , 3 or 4 */
        /* get the next param in the cmd line  :  *(pData+3) = M Voltage type requested */
        /* M Voltage type : V_WIDE = 1,  V_FUND = 2 */

        /* build text display according to M Voltage type */
        switch (*(pData+3))
        {
        case (V_WIDE):
          {
            strcpy(text, "Metro Read M Voltage  V_WIDE for Channel %02x :  0x%08x\n");
          }
          break;
        case (V_FUND):
          {
            strcpy(text, "Metro Read M Voltage  V_FUND for Channel %02x :  0x%08x\n");
          }
          break;
        }
        data[0] = *(pData+2);
        data[1] = Metro_Read_Momentary_Voltage((METRO_Channel_t)*(pData+2), (METRO_Voltage_type_t)*(pData+3));
        MNSH_Printf(text,data[0],data[1]);

      }
     }
    break;
     /* read Momentary Current   service*/
  case (5):
    {
      /* test set or get request */
      /* Only Get is implemented */
      if (*(pData+1) == 1)
      {
        /* it is a metro read M Current service requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel 1, 2 , or tamper */
        /* get the next param in the cmd line  :  *(pData+3) = M Current type requested */
        /* M Voltage type : C_WIDE = 1,  C_FUND = 2 */

        /* build text display according to M Voltage type */
        switch (*(pData+3))
        {
        case (V_WIDE):
          {
            strcpy(text, "Metro Read M Current  C_WIDE for Channel %02x :  0x%08x\n");
          }
          break;
        case (V_FUND):
          {
            strcpy(text, "Metro Read M Current  C_FUND for Channel %02x :  0x%08x\n");
          }
          break;
        }
        data[0] = *(pData+2);
        data[1] = Metro_Read_Momentary_Current((METRO_Channel_t)*(pData+2), (METRO_Current_type_t)*(pData+3));
        MNSH_Printf(text,data[0],data[1]);
      }
     }
    break;

     /* read RMS current and voltage  service*/
  case (6):
    {
      /* test set or get request */
      /* Only Get is implemented */
      if (*(pData+1) == 1)
      {
        /* it is a metro read RMS voltage and current service requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel 1, 2 , 3 */
        /* get the  next param in the cmd line : *(pData+3) = RMS type : 1 : Current  2: Voltage */
        /* get the  next param in the cmd line : *(pData+4) = output format : RAW type : 0 : RMS type : 1 */

        uint32_t RMS_V = 0;
        uint32_t RMS_I = 0;

        data[0] = *(pData+2);

        /* Read RMS v and I from a channel */
        Metro_Read_RMS((METRO_Channel_t)*(pData+2),&RMS_V,&RMS_I,(uint8_t)*(pData+4));

        /*  Current requested */
        if (*(pData+3) == 1)
        {
          /* Display RMS Current */
          strcpy(text, "Metro Read RMS Current for Channel %02x :  0x%08x\n");
          data[1] = RMS_I;
        }
        /*  Voltage  requested*/
        else if (*(pData+3) == 2)
        {
          /* Display RMS voltage */
          strcpy(text, "Metro Read RMS Voltage for Channel %02x :  0x%08x\n");
          data[1] = RMS_V;
        }

        MNSH_Printf(text,data[0],data[1]);
      }
     }
    break;

     /* read PHI   service*/
  case (7):
    {
      /* test set or get request */
      /* Only Get is implemented */
      if (*(pData+1) == 1)
      {
        /* it is a metro read PHI service requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel 1, 2 or 3 ( channel 4 exlcuded because only TAMPER */
        strcpy(text, "Metro Read   PHI for Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Read_PHI((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
      }
     }
    break;
     /*  Set/Get temperature compensation service*/
  case (8):
    {
      /* test set or get request */
      /* It is a set */
      if (*(pData+1) == 0)
      {
        /* it is a metro set Current gain requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = Tempeature Compasation value  = 0 to 7 */

        /* Ask Metro driver to set the new Temperature compensation */
        Metro_Set_Temperature_Compensation((METRO_Channel_t)*(pData+2), (METRO_Gain_t) *(pData+3));
      }
      /* It is a get */
      else if (*(pData+1) == 1)
      {
        /* it is a metro read PHI service requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel 1, 2 or 3 ( channel 4 exlcuded because only TAMPER) */
        strcpy(text, "Metro Get TC for Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Get_Temperature_Compensation((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
      }
     }
    break;
      /*  Set/Get TAMPER tolerance and Enable/Disable bit*/
  case (9):
    {
      /* test set or get request */
      /* It is a set */
      if (*(pData+1) == 0)
      {
        /* it is a metro set Channel tamper config requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel tamper*/
        /* get the  next param in the cmd line : *(pData+3) = tolerance change : 1 , Tamper change = 2*/

        /* get the  next param in the cmd line : *(pData+4) = Tamper tolerance value value  = 0 to 3 */
        /*or */
        /* get the  next param in the cmd line : *(pData+4) = Tamper Enable Bit  = 0 or 1 */

        /* Tolerance change requested */
        if ( *(pData+3) == 1)
        {
          /* Ask Metro driver to set the new Tamper tolerance config */
          Metro_Set_Tamper((METRO_Channel_t)*(pData+2), (METRO_Tamper_Tolerance_t) *(pData+4),NO_CHANGE);

        }
        /* Enable/Disable tamper requested */
        else if ( *(pData+3) == 2)
        {
          /* Ask Metro driver to set the new Tamper enable bit config*/
          Metro_Set_Tamper((METRO_Channel_t)*(pData+2),NO_CHANGE_TOL,(METRO_CMD_Device_t) *(pData+4));
        }
      }
      /* It is a get */
      else if (*(pData+1) == 1)
      {
        /*  it is a metro Get Channel tamper config requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel Tamper ) */
        /* get the  next param in the cmd line : *(pData+3) = tolerance requested : 1 , Tamper Requested = 2*/

        /*  tolerance requested */
        if (*(pData+3) == 1)
        {

          /* Display tolerance  */
          strcpy(text, "Metro TAMPER config Tolerance = 0x%02x \n");
          data[1] = Metro_Get_Tamper((METRO_Channel_t)*(pData+2),(METRO_Tamper_Tolerance_t*)&data[0]);
        }
        /*  Enable Tamper  requested*/
        else if (*(pData+3) == 2)
        {
          /* Display RMS voltage */
          strcpy(text, "Metro TAMPER Config Enable bit = 0x%02x \n");
          data[0] = Metro_Get_Tamper((METRO_Channel_t)*(pData+2),(METRO_Tamper_Tolerance_t*)&data[1]);
        }
        MNSH_Printf(text,data[0],data[1]);
      }
     }
    break;
      /*  Set/Get Vref */
    case (10):
    {
      /* test set or get request */
      /* It is a set */
      if (*(pData+1) == 0)
      {
        /* it is a metro set Vref config requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = Vref type selected  EXT_VREF = 0, INT_VREF = 1 */

        /* Ask Metro driver to set the new vref config */
        Metro_Set_Vref((METRO_Channel_t)*(pData+2), (METRO_Vref_t)*(pData+3));
      }
      /* It is a get */
      else if (*(pData+1) == 1)
      {
        /*  it is a metro Get Channel Vref config requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel  ) */

        strcpy(text, "Metro Get Vref config for Channel = %02x :  Vref = 0x%02x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Get_Vref((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
      }
     }
    break;
    /*  Set/Get HightPass filters config ( DC remover )  for Current and voltage Channel */
    case (11):
    {
      /* test set or get request */
      /* It is a set */
      if (*(pData+1) == 0)
      {
        /* it is a metro  HightPass filters config requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel 1 to 4 */
        /* get the  next param in the cmd line : *(pData+3) = 0 current channel request or 1 : voltage channel requested */
        /* Get the next param in the cmd line  : *(pData+4) =   DEVICE_DISABLE = 0,  DEVICE_ENABLE = 1 */

        /* Current requested */
        if ( *(pData+3) == 1)
        {
          /* Ask Metro driver to set the HightPass filters config for current channel */
          Metro_Set_Current_HP_Filter((METRO_Channel_t)(*(pData+2)),(METRO_CMD_Device_t)(*(pData+4)));
        }
        /* voltage requested */
        else if ( *(pData+3) == 2)
        {
          /* Ask Metro driver to set the HightPass filters config for Voltage channel */
          Metro_Set_Voltage_HP_Filter((METRO_Channel_t)*(pData+2), (METRO_CMD_Device_t)*(pData+4));
        }
      }
      /* It is a get */
      else if (*(pData+1) == 1)
      {
        /*  it is a metro Get Channel HightPass filters config  requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel  ) */
        /* get the  next param in the cmd line : *(pData+3) = 0 current channel requested or 1 : voltage channel requesteded */

        data[0] = *(pData+2);

        /* Current requested */
        if ( *(pData+3) == 1)
        {
          /* Ask Metro driver to Get the HightPass filters config for current channel */
          data[1] = Metro_Get_Current_HP_Filter((METRO_Channel_t)*(pData+2));
          strcpy(text, "Metro HightPass filters config for Current Channel = %02x :  Enable = 0x%02x\n");

        }
        /* voltage requested */
        else if ( *(pData+3) == 2)
        {
          /* Ask Metro driver to Get the HightPass filters config for Voltage channel */
          data[1] = Metro_Get_Voltage_HP_Filter((METRO_Channel_t)*(pData+2));
          strcpy(text, "Metro HightPass filters config for Voltage Channel = %02x :  Enable = 0x%02x\n");
        }

        MNSH_Printf(text,data[0],data[1]);
      }
     }
    break;

      /*  Set/Get  Coil integrator (Rogowski) for a  channel */
    case (13):
    {
      /* test set or get request */
      /* It is a set */
      if (*(pData+1) == 0)
      {
        /* it is a metro set Coil integrator config requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = DEVICE_DISABLE = 0,  DEVICE_ENABLE = 1, */

        /* Ask Metro driver to set the new Coil integrator config */
        Metro_Set_Coil_integrator((METRO_Channel_t)*(pData+2), (METRO_CMD_Device_t)*(pData+3));
      }
      /* It is a get */
      else if (*(pData+1) == 1)
      {
        /*  it is a metro Get Coil integrator config requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel  ) */
        strcpy(text, "Metro Get Coil integrator config for Channel = %02x :  Enable = 0x%02x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Get_Coil_integrator((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
      }
     }
    break;

    /*  Set/Get Ah_Accumulation_Down_Threshold*/
    case (14):
    {
      /* test set or get request */
      /* It is a set */
      if (*(pData+1) == 0)
      {
        /* it is a metro set Ah_Down_threshold config requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = in_Metro_Ah_Down_threshold */

        /* Ask Metro driver to set the new Ah_Down_threshold config */
        Metro_Set_Ah_Accumulation_Down_Threshold((METRO_Channel_t)*(pData+2), (uint16_t)*(pData+3));
      }
      /* It is a get */
      else if (*(pData+1) == 1)
      {
        /*  it is a metro Get Channel Ah Down Threshold config requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel  ) */

        strcpy(text, "Metro Get Ah Down Threshold  config for Channel = %02x :  Ah Down Threshold = 0x%02x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Get_Ah_Accumulation_Down_Threshold((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
      }
     }
    break;

    /*  Set/Get Ah_Accumulation_Up_Threshold*/
    case (15):
    {
      /* test set or get request */
      /* It is a set */
      if (*(pData+1) == 0)
      {
        /* it is a metro set Ah_Accumulation_Up_Threshold config requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = in_Metro_Ah_Up_threshold */

        /* Ask Metro driver to set the new Ah_Accumulation_Up_Threshold config */
        Metro_Set_Ah_Accumulation_Up_Threshold((METRO_Channel_t)*(pData+2), (uint16_t)*(pData+3));
      }
      /* It is a get */
      else if (*(pData+1) == 1)
      {
        /*  it is a metro Get Channel Ah up Threshold config requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel  ) */

        strcpy(text, "Metro Get Ah Up Acc Threshold  config for Channel = %02x :  Ah Up Acc Threshold = 0x%02x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Get_Ah_Accumulation_Up_Threshold((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
      }
     }
    break;
     /* read AH Acc for current channel service*/
  case (16):
  {
      /* test set or get request */
      /* Only Get is implemented */
      if (*(pData+1) == 1)
      {
        /* it is a metro read AH Acc service requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel 1, 2, 3 or 4 */

        strcpy(text, "Metro Read AH Acc for Channel %02x :  0x%08x\n");

        data[0] = *(pData+2);
        data[1] = Metro_Read_AH_Acc((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);

      }
   }
   break;

  }
}

static void METRO_HandleMetroCal(uint32_t *pData)
{
  /* Get Payload pointer from Payload[0] */
  /* Payload  DATA  : [0] = Cal service, [1] = set (0) or get(1)   , [2]...[N] params */
  /* Cal  service (*pData) :   */
  /*   - 0 :  Set/Get V_Calibration      */
  /*   - 1 :  Set/Get C_Calibration      */
  /*   - 2 :  Set/Get Phase_V_Calibration      */
  /*   - 3 :  Set/Get V_Calibration      */
  /*   - 4 :  Set/Get power Cal Offset compensation config */

  uint32_t data[2] = {0,0};

  switch (*pData)
  {
    /* V_Calibration service*/
    case (0):
    {
      /* test set or get request */

      /* Set */
      if (*(pData+1) == 0)
      {
        /* it is a cal  Set  V_Calibration */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = u16 V calibration value ( !!! register value under 12 bits  !!!)*/

        /* Ask Metro driver to set the new V calibration value */
        Metro_Set_V_Calibration((METRO_Channel_t)*(pData+2), (uint16_t) *(pData+3));

      }
      /* Get */
      else if (*(pData+1) == 1)
      {
        /* it is a cal  Get  V_Calibration */
        /* get the  next param in the cmd line : *(pData+2) = Channel */

        strcpy(text, "Metrology Read V Calibration from Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Get_V_Calibration((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;

    /* C Calibration service*/
    case (1):
    {
      /* test set or get request */

      /* Set */
      if (*(pData+1) == 0)
      {
        /* it is a cal  Set  C_Calibration */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = u16 C calibration value ( !!! register value under 12 bits  !!!)*/

        /* Ask Metro driver to set the new C calibration value */
        Metro_Set_C_Calibration((METRO_Channel_t)*(pData+2), (uint16_t) *(pData+3));

      }
      /* Get */
      else if (*(pData+1) == 1)
      {
        /* it is a cal  Get  C_Calibration */
        /* get the  next param in the cmd line : *(pData+2) = Channel */

        strcpy(text, "Metrology Read C Calibration from Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Get_C_Calibration((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;
    /* Phase V_Calibration service*/
    case (2):
    {
      /* test set or get request */

      /* Set */
      if (*(pData+1) == 0)
      {
        /* it is a cal  Set Phase V_Calibration */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = u8 Phase V calibration value ( !!! register value under 2 bits  !!!) */

        /* Ask Metro driver to set the new Phase V calibration value */
        Metro_Set_Phase_V_Calibration((METRO_Channel_t)*(pData+2), (uint8_t) *(pData+3));

      }
      /* Get */
      else if (*(pData+1) == 1)
      {
        /* it is a cal  Get Phase V_Calibration */
        /* get the  next param in the cmd line : *(pData+2) = Channel */

        strcpy(text, "Metrology Read Phase V Calibration from Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Get_Phase_V_Calibration((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;

    /* Phase C Calibration service*/
    case (3):
    {
      /* test set or get request */

      /* Set */
      if (*(pData+1) == 0)
      {
        /* it is a cal Set  Phase C_Calibration */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = u16 Phase C calibration value ( !!! register value under 10 bits  !!!)  */

        /* Ask Metro driver to set the new Phase C calibration value */
        Metro_Set_Phase_C_Calibration((METRO_Channel_t)*(pData+2), (uint16_t) *(pData+3));

      }
      /* Get */
      else if (*(pData+1) == 1)
      {
        /* it is a cal  Get  Phase C_Calibration */
        /* get the  next param in the cmd line : *(pData+2) = Channel */

        strcpy(text, "Metrology Read Phase C Calibration from Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Get_Phase_C_Calibration((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;
     /* Set/Get power Cal Offset compensation config */
   case (4):
    {

      /* test set or get request */
      /* It is a set */
      if (*(pData+1) == 0)
      {
        /* it is a metro set power Offset compensation config requested */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = in_Metro_Power_Selection :  W_ACTIVE = 1, F_ACTIVE = 2, REACTIVE = 3, APPARENT_RMS = 4  */
        /* get the  next param in the cmd line : *(pData+4) = in_Metro_Power_Offset : offset is coded under 10 bits */

        /* Ask Metro driver to set the new power Offset compensation config */
        Metro_Set_Power_Offset_Compensation((METRO_Channel_t)*(pData+2), (METRO_Power_selection_t)*(pData+3), (int16_t)*(pData+4));

      }

      /* It is a Get  */
      else if (*(pData+1) == 1)
      {
        /* it is a metro Get power Offset compensation config requested*/
        /* get the  next param in the cmd line : *(pData+2) = Channel 1, 2 , 3 or 4 */
        /* get the  next param in the cmd line power type  *(pData+3) : W_ACTIVE = 1, F_ACTIVE = 2, REACTIVE = 3, APPARENT_RMS = 4 */

        /* build text display according to Power type */
        switch (*(pData+3))
        {
        case (W_ACTIVE):
          {
            strcpy(text, "Get power W_ACTIVE Offset  config Channel %02x :  0x%08x\n");
          }
          break;
        case (F_ACTIVE):
          {
            strcpy(text, " Get power F_ACTIVE Offset  config Channel %02x :  0x%08x\n");
          }
          break;
        case (REACTIVE):
          {
            strcpy(text, " Get power REACTIVE Offset  config Channel %02x :  0x%08x\n");
          }
          break;
        case (APPARENT_RMS):
          {
            strcpy(text, " Get power APPARENT_RMS Offset config Channel %02x :  0x%08x\n");
          }
          break;

        }

        data[0] = *(pData+2);
        data[1] = Metro_Get_Power_Offset_Compensation((METRO_Channel_t)*(pData+2), (METRO_Power_selection_t) *(pData+3));
        MNSH_Printf(text,data[0],data[1]);

      }

     }
    break;
  }
}

static void METRO_HandleMetroIRQ(uint32_t *pData)
{
  /* Get Payload pointer from Payload[0] */
  /* Payload  DATA  : [0] = IRQ service, [1] = set (0) or get(1)   , [2]...[N] params */
  /* IRQ  service (*pData) :   */
  /*   - 0 :  Set/Get IRQ  Mask    */
  /*   - 1 :  Set/Get IRQ Status      */
  /*   - 2 :  Get DSP Event      */

  uint32_t data[2] = {0,0};

  switch (*pData)
  {
    /* Set/Get IRQ*/
    case (0):
    {
      /* test set or get request */

      /* Set IRQ mask*/
      if (*(pData+1) == 0)
      {
        /* it is a cal  Set  IRQ mask */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = u32 Metro_IT_Mask */

        /* Ask Metro driver to set the new IRQ mask value */
        Metro_Set_IRQ_Mask_for_Channel((METRO_Channel_t)*(pData+2), (uint32_t) *(pData+3));
      }
      /* Get IRQ Mask*/
      else if (*(pData+1) == 1)
      {
        /* it is a cal  Get  IRQ mask */
        /* get the  next param in the cmd line : *(pData+2) = Channel */

        strcpy(text, "Get  IRQ mask from Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Get_IRQ_Mask_for_Channel((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;

    /* Set(Clear) IRQ / Get IRQ Status*/
    case (1):
    {
      /* test set or get request */

      /* Set (Clear IRQ) */
      if (*(pData+1) == 0)
      {
        /* it is a  Clear IRQ  service */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = METRO_Status_Type_t Metro_Status_requested*/

        /* Ask Metro driver to clear the IRQ requested */
        Metro_Clear_Status_for_Channel((METRO_Channel_t)*(pData+2), (METRO_Status_Type_t) *(pData+3));

      }
      /* Get IRQ status */
      else if (*(pData+1) == 1)
      {
        /* it is a Get IRQ status service requested*/
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = METRO_Status_Type_t Metro_Status_requested*/

        strcpy(text, "Metrology Read IRQ Status from Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Read_Status_from_Channel((METRO_Channel_t)*(pData+2),(METRO_Status_Type_t) *(pData+3));
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;
    /* Get DSP event*/
    case (2):
    {
      /* test set or get request */

      /* Get Only for this service*/
      if (*(pData+1) == 1)
      {
        /* it is Get DSP event */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = METRO_Live_Event_Type_t Metro_Live_Event_requested*/

        strcpy(text, "Metrology Read DSP Event from Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Read_Live_Event_from_Channel((METRO_Channel_t)*(pData+2),(METRO_Live_Event_Type_t) *(pData+3));
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;
  }
}

static void METRO_HandleMetroSagSwell(uint32_t *pData)
{
    /* Get Payload pointer from Payload[0] */
  /* Payload  DATA  : [0] = SAG_SWELL service, [1] = set (0) or get(1)   , [2]...[N] params */
  /* SAG_SWELL  service (*pData) :   */
  /*   - 0 :  Set/Get SAG threshold    */
  /*   - 1 :  Set/Get SAG Detect time    */
  /*   - 2 :  Set/Get V SWELL config      */
  /*   - 3 :  Set/Get C SWELL config       */
  /*   - 4 :  Get SAG time      */
  /*   - 5 :  Get V SWELL Time      */
  /*   - 6 :  Get C SWELL Time       */
  /*   - 7 :  Set/Get SAG and SWELL Clear TimeOut      */
  /*   - 8 :  Set Clear SAG and SWELL Event       */

  uint32_t data[2] = {0,0};

  switch (*pData)
  {
    /* Set/Get SAG threshold*/
    case (0):
    {
      /* test set or get request */

      /* Set SAG threshold */
      if (*(pData+1) == 0)
      {
        /* it is a Set SAG config */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = u32 in_Metro_SAG_Threshold */

        /* Ask Metro driver to set the new SAG threshold */
        Metro_Set_SAG_Config((METRO_Channel_t)*(pData+2), (uint32_t) *(pData+3),0);
      }
      /* Get SAG threshold*/
      else if (*(pData+1) == 1)
      {
        /* it is a SAG Get threshold */
        /* get the  next param in the cmd line : *(pData+2) = Channel */

        strcpy(text, "Get SAG threshold: 0x%08x\n");

        Metro_Get_SAG_Config((METRO_Channel_t)*(pData+2),&data[0],&data[1]);
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;
    /* Set/Get SAG config Detect time*/
    case (1):
    {
      /* test set or get request */

      /* Set SAG Detect time */
      if (*(pData+1) == 0)
      {
        /* it is a Set SAG config */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = u32 in_Metro_SAG_detect_time */

        /* Ask Metro driver to set the new SAG Detect time */
        Metro_Set_SAG_Config((METRO_Channel_t)*(pData+2),0, (uint32_t) *(pData+3));
      }
      /* Get SAG Detect time*/
      else if (*(pData+1) == 1)
      {
        /* it is a SAG Get Detect time */
        /* get the  next param in the cmd line : *(pData+2) = Channel */

        strcpy(text, "Get SAG detect Time: 0x%08x\n");

        Metro_Get_SAG_Config((METRO_Channel_t)*(pData+2),&data[1],&data[0]);
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;

    /* Set/Get V SWELL config */
    case (2):
    {
      /* test set or get request */

      /* Set V SWELL config */
      if (*(pData+1) == 0)
      {
        /* it is a  V SWELL config  service */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = u16 in_Metro_V_SWELL_Threshold*/

        /* Ask Metro driver to set the new V SWELL config */
        Metro_Set_V_SWELL_Config((METRO_Channel_t)*(pData+2), (uint16_t) *(pData+3));

      }
      /* Get V SWELL config */
      else if (*(pData+1) == 1)
      {
        /* it is a Get V SWELL config service requested*/
        /* get the  next param in the cmd line : *(pData+2) = Channel */

        strcpy(text, "Metrology V SWELL threshold from Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Get_V_SWELL_Config((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;
    /* Set/Get C SWELL config */
    case (3):
    {
      /* test set or get request */

      /* Set C SWELL config */
      if (*(pData+1) == 0)
      {
        /* it is a  C  SWELL config  service */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = u16 in_Metro_C_SWELL_Threshold*/

        /* Ask Metro driver to set the new C SWELL config */
        Metro_Set_C_SWELL_Config((METRO_Channel_t)*(pData+2), (uint16_t) *(pData+3));

      }
      /* Get C SWELL config */
      else if (*(pData+1) == 1)
      {
        /* it is a Get C SWELL config service requested*/
        /* get the  next param in the cmd line : *(pData+2) = Channel */

        strcpy(text, "Metrology C SWELL threshold from Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Get_C_SWELL_Config((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;
    /*  Get SAG time */
    case (4):
    {
      /* Only Get SAG time */
      if (*(pData+1) == 1)
      {
        /* it is a SAG time service requested*/
        /* get the  next param in the cmd line : *(pData+2) = Channel */

        strcpy(text, "Metrology Get SAG time from Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Read_SAG_Time((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;
    /*  Get V SWELL  time */
    case (5):
    {
      /* Only Get V SWELL  time */
      if (*(pData+1) == 1)
      {
        /* it is a V SWELL  time service requested*/
        /* get the  next param in the cmd line : *(pData+2) = Channel */

        strcpy(text, "Metrology Get V SWELL  time from Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Read_V_SWELL_Time((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;
    /*  Get C SWELL time */
    case (6):
    {
      /* Only Get C SWELL time */
      if (*(pData+1) == 1)
      {
        /* it is a C SWELLtime service requested*/
        /* get the  next param in the cmd line : *(pData+2) = Channel */

        strcpy(text, "Metrology C SWELL time from Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Read_C_SWELL_Time((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;
    /* Set/Get SAG and SWELL Clear TimeOut */
    case (7):
    {
      /* test set or get request */

      /* Set SAG and SWELL Clear TimeOut */
      if (*(pData+1) == 0)
      {
        /* it Set SAG and SWELL Clear TimeOut  service */
        /* get the  next param in the cmd line : *(pData+2) = Channel */
        /* get the  next param in the cmd line : *(pData+3) = u8 in_Metro_Sag_and_Swell_Clear_Timeout*/

        /* Ask Metro driver to set the new SAG and SWELL Clear TimeOut */
        Metro_Set_SAG_and_SWELL_Clear_Timeout((METRO_Channel_t)*(pData+2), (uint8_t)*(pData+3));

      }
      /* Get SAG and SWELL Clear TimeOut  */
      else if (*(pData+1) == 1)
      {
        /* it is a SAG and SWELL Clear TimeOut service requested*/
        /* get the  next param in the cmd line : *(pData+2) = Channel */

        strcpy(text, "Metrology Get SAG and SWELL Clear TimeOut from Channel %02x :  0x%08x\n");
        data[0] = *(pData+2);
        data[1] = Metro_Get_SAG_and_SWELL_Clear_Timeout((METRO_Channel_t)*(pData+2));
        MNSH_Printf(text,data[0],data[1]);
     }
    }
    break;
    /*  Set Clear SAG and SWELL Events service */
    case (8):
    {
      /* Only Set Clear SAG and SWELL Events */
      if (*(pData+1) == 0)
      {
        /* it is a Set Clear SAG and SWELL Event service requested*/
        /* get the  next param in the cmd line : *(pData+2) = Channel */

        /* Ask Metro driver to Clear SAG and SWELL Events */
        Metro_Clear_SAG_and_SWELL_Event((METRO_Channel_t)*(pData+2));
     }
    }
    break;
  }
}

/**
  * @}
  */

/* End Of File */
