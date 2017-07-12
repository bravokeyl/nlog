/**
  ******************************************************************************
  * @file    metrology.h
  * @author  STMicroelectronics
  * @version V1.0
  * @date    11-March-2016
  * @brief   This file contains all the functions prototypes for the Generic Metrology
  *          firmware library, module metrology.c.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __METROLOGY_H
#define __METROLOGY_H

#ifdef __cplusplus
 extern "C" {
#endif

/* ------------------------------------------------------------------------------------------------------------------*/
/* --------------------------------------------  Includes -----------------------------------------------------------*/
/* ------------------------------------------------------------------------------------------------------------------*/


#include "stpm_metrology.h"
#include "st_device.h"
   
/* ------------------------------------------------------------------------------------------------------------------*/
/* -----------------------------------------  Exported types --------------------------------------------------------*/
/* ------------------------------------------------------------------------------------------------------------------*/


 /**
  * @brief METROLOGY Reset type
  *
  */  
   
typedef enum 
{
  RESET_SYN_SCS = 1,
  RESET_SW
}METRO_ResetType_t;  

 /**
  * @brief METROLOGY External device Enable/Disable
  *
  */  
   
typedef enum 
{
  EXT_DISABLE = 0,  
  EXT_ENABLE    
}METRO_CMD_EXT_Device_t;  


 /**
  * @brief METROLOGY generic cmd Enable/Disable
  *
  */  
   
typedef enum 
{
  DEVICE_DISABLE = 0,  
  DEVICE_ENABLE = 1,
  NO_CHANGE
}METRO_CMD_Device_t;  


 /**
  * @brief METROLOGY  Voltage Channel definition
  *
  */  
   
typedef enum 
{
  V_1 = 1,  
  V_2,
  V_3, 
  V_4  
}METRO_Voltage_Channel_t; 

 /**
  * @brief METROLOGY  Current CHANNEL definition
  *
  */  
   
typedef enum 
{
  C_1 = 1,  
  C_2,
  C_3, 
  C_4  
}METRO_Current_Channel_t; 

 /**
  * @brief METROLOGY  Current Gain definition
  *
  */  
   
typedef enum 
{
  X2 = 0,  
  X4,
  X8, 
  X16  
}METRO_Gain_t; 


/**
  * @brief METROLOGY  Vref device definition
  *
  */  
   
typedef enum 
{
  EXT_VREF =0,
  INT_VREF  
}METRO_Vref_t;


 /**
  * @brief METROLOGY  Current CHANNEL definition
  *
  */  
   
typedef enum 
{
  PRIMARY = 0,  
  SECONDARY,
  ALGEBRIC, 
  SIGMA_DELTA  
}METRO_LED_Channel_t; 


 /**
  * @brief METROLOGY  LED Slection type
  *
  */  
   
typedef enum 
{
  LED1 = 1,  
  LED2 
}METRO_LED_Selection_t; 


/**
  * @brief METROLOGY  Power selection type
  *
  */  
   
typedef enum 
{
  W_ACTIVE = 1,  
  F_ACTIVE,
  REACTIVE, 
  APPARENT_RMS,
  APPARENT_VEC,
  MOM_WIDE_ACT,
  MOM_FUND_ACT
}METRO_Power_selection_t;

typedef enum 
{
  LED_W_ACTIVE = 0,  
  LED_F_ACTIVE,
  LED_REACTIVE, 
  LED_APPARENT_RMS,

}METRO_LED_Power_selection_t;


typedef enum 
{
  E_W_ACTIVE = 1,  
  E_F_ACTIVE,
  E_REACTIVE, 
  E_APPARENT,
  NB_MAX_TYPE_NRJ
}METRO_Energy_selection_t;

/**
  * @brief METROLOGY  Calculation Power selection type
  *
  */  
   
typedef enum 
{
  FROM_RMS = 1,  
  FROM_PWIDE,
  FROM_PFUND
}METRO_Calculation_Power_selection_t;

/**
  * @brief METROLOGY  Latch device type
  *
  */  
typedef enum 
{
  LATCH_SYN_SCS = 1,  
  LATCH_SW,
  LATCH_AUTO
 }METRO_Latch_Device_Type_t;

/**
  * @brief METROLOGY  Voltage read type
  *
  */  
typedef enum 
{
  V_WIDE = 1,  
  V_FUND
 }METRO_Voltage_type_t;

/**
  * @brief METROLOGY  Current read type
  *
  */  
typedef enum 
{
  C_WIDE = 1,  
  C_FUND
 }METRO_Current_type_t;



/**
  * @brief METROLOGY  Tamper Tolerance type
  *
  */  
typedef enum 
{
  TOL_12_5 = 0,  
  TOL_8_33,
  TOL_6_25,
  TOL_3_125,
  NO_CHANGE_TOL
 }METRO_Tamper_Tolerance_t;


/**
  * @brief METROLOGY  ZCR Signal Selection
  *
  */  
typedef enum 
{
  ZCR_SEL_V1 = 0,  
  ZCR_SEL_C1,
  ZCR_SEL_V2,
  ZCR_SEL_C2,
  NO_CHANGE_ZCR
 }METRO_ZCR_Sel_t;

 
 /**
  * @brief METROLOGY  CLK  Selection
  *
  */  
typedef enum 
{
  CLK_SEL_7KHz = 0,  
  CLK_SEL_4MHz,
  CLK_SEL_4MHz_50,
  CLK_SEL_16MHz,
  NO_CHANGE_CLK
 }METRO_CLK_Sel_t;
 
   
  /**
  * @brief METROLOGY  Live Event type
  *
  */  
typedef enum 
{
  ALL_LIVE_EVENTS =0,
  LIVE_EVENT_REFRESHED,
  LIVE_EVENT_WRONG_INSERTION,
  LIVE_EVENT_VOLTAGE_SAG,  
  LIVE_EVENT_VOLTAGE_SWELL,
  LIVE_EVENT_CURRENT_SWELL,
  LIVE_EVENT_VOLTAGE_ZCR,
  LIVE_EVENT_CURRENT_ZCR,  
  LIVE_EVENT_VOLTAGE_PERIOD_STATUS,
  LIVE_EVENT_VOLTAGE_SIGNAL_STUCK,
  LIVE_EVENT_CURRENT_SIGNAL_STUCK,
  LIVE_EVENT_CURRENT_TAMPER,
  LIVE_EVENT_CURRENT_SIGN_CHANGE_APPARENT_POWER,
  LIVE_EVENT_CURRENT_SIGN_CHANGE_REACTIVE_POWER,
  LIVE_EVENT_CURRENT_SIGN_CHANGE_FUNDAMENTAL_POWER,
  LIVE_EVENT_CURRENT_SIGN_CHANGE_ACTIVE_POWER,
  LIVE_EVENT_CURRENT_OVERFLOW_APPARENT_NRJ,
  LIVE_EVENT_CURRENT_OVERFLOW_REACTIVE_NRJ,
  LIVE_EVENT_CURRENT_OVERFLOW_FUNDAMENTAL_NRJ,
  LIVE_EVENT_CURRENT_OVERFLOW_ACTIVE_NRJ,
  LIVE_EVENT_CURRENT_NAH
 }METRO_Live_Event_Type_t;

  /**
  * @brief METROLOGY Status type
  *
  */  
typedef enum 
{
  ALL_STATUS = 0,
  STATUS_REFRESHED,
  STATUS_TAMPER_DETECTED,
  STATUS_TAMPER_OR_WRONG,
  STATUS_VOLTAGE_SWELL_DOWN,
  STATUS_VOLTAGE_SWELL_UP,
  STATUS_VOLTAGE_SAG_DOWN,
  STATUS_VOLTAGE_SAG_UP,    
  STATUS_VOLTAGE_PERIOD_STATUS,
  STATUS_VOLTAGE_SIGNAL_STUCK,
  STATUS_CURRENT_OVERFLOW_APPARENT_NRJ,
  STATUS_CURRENT_OVERFLOW_REACTIVE_NRJ,
  STATUS_CURRENT_OVERFLOW_FUNDAMENTAL_NRJ,
  STATUS_CURRENT_OVERFLOW_ACTIVE_NRJ,
  STATUS_CURRENT_SIGN_APPARENT_POWER,
  STATUS_CURRENT_SIGN_CHANGE_REACTIVE_POWER,
  STATUS_CURRENT_SIGN_CHANGE_FUNDAMENTAL_POWER,
  STATUS_CURRENT_SIGN_CHANGE_ACTIVE_POWER,
  STATUS_CURRENT_SWELL_DOWN,
  STATUS_CURRENT_SWELL_UP,
  STATUS_CURRENT_NAH_TMP,
  STATUS_CURRENT_SIGNAL_STUCK
 }METRO_Status_Type_t;
 
  /**
  * @brief METROLOGY Status type
  *
  */  
typedef enum 
{
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
  STATUS_STPM_SPI_LINK_OVERRRUN,
 }METRO_STPM_LINK_IRQ_Status_Type_t;
  
 /**
  * @brief METROLOGY  Boolean  type
  *
  */  
typedef enum 
{
  BOOL_FALSE = 0,
  BOOL_TRUE  
}METRO_Bool_Type_t;
  
 /**
  * @brief METROLOGY External device Number
  *
  */  
   
typedef enum 
{
  HOST=0,
  EXT1,  
  EXT2,
  EXT3,
  EXT4,
  NB_MAX_DEVICE 
}METRO_NB_Device_t;  

 /**
  * @brief METROLOGY  CHANNEL definition
  *
  */  
   
typedef enum 
{
  CHANNEL_NONE=0,
  CHANNEL_1,  
  CHANNEL_2,
  CHANNEL_3,  
  CHANNEL_4,
  CHANNEL_5,
  CHANNEL_6,
  CHANNEL_7,
  CHANNEL_8,
  NB_MAX_CHANNEL  
}METRO_Channel_t; 

typedef enum 
{
  INT_NONE_CHANNEL=0,
  INT_CHANNEL_1,  
  INT_CHANNEL_2,
  CHANNEL_TAMPER
}METRO_internal_Channel_t; 



 /**
  * @brief METROLOGY hardware Device type
  *
  */
     
typedef enum 
{
  Device_NONE=0,
  STM32 = 5,
  STPM32 = 6,                           
  STPM33,                            
  STPM34,
  NB_MAX_STPM
}METRO_Device_t;


/* Struct to define communication between STM32 and STPMs chips */
typedef struct
{
  uint8_t            rxData;
  uint8_t            txData;
  uint8_t            txValid;
  uint8_t            rxValid;
  uint8_t            txOngoing;
  uint8_t            rxOngoing;  
  uint8_t            *pTxReadBuf;
  uint8_t            *pTxWriteBuf;
  uint8_t            *pRxReadBuf;
  uint8_t            *pRxWriteBuf;
} STPM_Com_t;

/* Struct to define communication pin and  port between STM32 and STPMs chips */

typedef struct
{
#ifdef UART_XFER_STPM3X   
  USART_TypeDef*  uart;
#endif 
#ifdef SPI_XFER_STPM3X
  SPI_TypeDef*    spi;
#endif  
  GPIO_TypeDef*   cs_port;
  uint16_t           cs_pin;
  GPIO_TypeDef*   syn_port;
  uint16_t           syn_pin;
  GPIO_TypeDef*   en_port;
  uint16_t           en_pin;
} STPM_Com_port_t;


/**
  * @brief METROLOGY Mapping Channels ( 1 to 4 ) to real V and C chip channels 
  * according to the Device
  *  Put NONE_CHANNEL if the channel is not mapped oterhwise  CHANNEL_1, CHANNEL_2, CHANNEL_3, CHANNEL_4   */


typedef struct
{
  METRO_Device_t              device;      
  uint8_t                     channels_mask;
  uint32_t                    factor_power_int_ch1;
  uint32_t                    factor_energy_int_ch1;
  uint32_t                    factor_power_int_ch2;
  uint32_t                    factor_energy_int_ch2;
  uint32_t                    factor_voltage_int_ch1;
  uint32_t                    factor_current_int_ch1;
  uint32_t                    factor_voltage_int_ch2;
  uint32_t                    factor_current_int_ch2;
  METRO_Latch_Device_Type_t   latch_device_type;  
  STPM_Com_t                  STPM_com;
  STPM_Com_port_t             STPM_com_port;
  METRO_STPM_TypeDef          metro_stpm_reg;
}METRO_Device_Config_t;



typedef struct
{
  int32_t       energy[NB_MAX_CHANNEL][NB_MAX_TYPE_NRJ];
  int32_t       energy_extension[NB_MAX_CHANNEL][NB_MAX_TYPE_NRJ];
}METRO_Data_Energy_t;

/* Define SW MEtro revision V1.2 Version */
#define METRO_SW_REVISION     0x00010002


#define    CHANNEL_MASK_CONF_CHANNEL_1     0x01
#define    CHANNEL_MASK_CONF_CHANNEL_2     0x02
#define    CHANNEL_MASK_CONF_CHANNEL_3     0x04
#define    CHANNEL_MASK_CONF_CHANNEL_4     0x08
#define    CHANNEL_MASK_CONF_CHANNEL_5     0x10
#define    CHANNEL_MASK_CONF_CHANNEL_6     0x20
#define    CHANNEL_MASK_CONF_CHANNEL_7     0x40
#define    CHANNEL_MASK_CONF_CHANNEL_8     0x80
  
#define    NB_NAX_CHANNEL                   9
 
#define    DEVICE_MASK_CONF                0x0F
#define    CHANNEL_MASK_CONF               0xF0

/* ------------------------------------------------------------------------------------------------------------------*/
/* -------------------------------------  Exported functions --------------------------------------------------------*/
/* ------------------------------------------------------------------------------------------------------------------*/

/****************/
/* Device Level */
/****************/

/* Initialization and Setup functions *********************************/
void Metro_Init(void);
void Metro_power_up_device(void);

#ifdef UART_XFER_STPM3X /* UART MODE */   
void Metro_UartSpeed(uint32_t baudrate);
#endif

void Metro_Config_Reset(METRO_ResetType_t in_MetroResetType);

  /* set metrology Config */
uint8_t Metro_Setup(uint32_t in_host_config,uint32_t in_stpm_config);

/* Set new UART baudrate */
uint8_t Metro_Set_uart_baudrate_to_device(METRO_NB_Device_t in_Metro_Device_Id, uint32_t in_baudrate);

/* Get setup Metrology */
uint8_t Metro_Get_Setup(uint32_t * out_p_host_config,uint32_t * out_p_stpm_config);

/*****************/
/* Channel Level */
/*****************/

/* Analog and digital Front End functions**************************************/

/* Hardware factors power, NRJ, Voltage, Current */
void Metro_Set_Hardware_Factors(METRO_Channel_t in_Metro_Channel, uint32_t in_Factor_Power,uint32_t in_Factor_Nrj,uint32_t in_Factor_Voltage,uint32_t in_Factor_Current);
/* Set and Get  VRef and Temperature Compensation of devices */
void Metro_Set_Vref(METRO_Channel_t in_Metro_Channel, METRO_Vref_t in_Metro_Vref);
METRO_Vref_t Metro_Get_Vref(METRO_Channel_t in_Metro_Channel);

void Metro_Set_Temperature_Compensation(METRO_Channel_t in_Metro_Channel, uint8_t in_Metro_TC_Value);
uint8_t Metro_Get_Temperature_Compensation(METRO_Channel_t in_Metro_Channel);

/*  set  and get the tamper tolerance for detecting unbalanced active energy between the two current channels. */
void Metro_Set_Tamper(METRO_Channel_t in_Metro_Channel,METRO_Tamper_Tolerance_t in_Metro_Tamper_Tolerance,METRO_CMD_Device_t in_Metro_CMD);
METRO_CMD_Device_t  Metro_Get_Tamper(METRO_Channel_t in_Metro_Channel,METRO_Tamper_Tolerance_t * out_p_Tamper_Tolerance);

/* Set Current Channel Gain */
uint8_t Metro_Set_Current_gain(METRO_Channel_t in_Metro_Channel, METRO_Gain_t in_Metro_Gain);

/* Get Current Channel Gain */
METRO_Gain_t Metro_Get_Current_gain(METRO_Channel_t in_Metro_Channel);

/* DSP Functions ****************************************************************/

/* enable/disable HightPass filters ( DC remover )  for Current or Voltage Channels ( V or C ) */
void Metro_Set_Current_HP_Filter(METRO_Channel_t in_Metro_Channel, METRO_CMD_Device_t in_Metro_CMD);
void Metro_Set_Voltage_HP_Filter(METRO_Channel_t in_Metro_Channel, METRO_CMD_Device_t in_Metro_CMD);

/* Get configs for HP filters */
METRO_CMD_Device_t Metro_Get_Current_HP_Filter(METRO_Channel_t in_Metro_Channel);
METRO_CMD_Device_t Metro_Get_Voltage_HP_Filter(METRO_Channel_t in_Metro_Channel);

/* Enable or disable Coil integrator (Rogowski) for a  channel */
void Metro_Set_Coil_integrator(METRO_Channel_t in_Metro_Channel, METRO_CMD_Device_t in_Metro_CMD);

/* Get config for Coil integrator Rogowski  */
METRO_CMD_Device_t Metro_Get_Coil_integrator(METRO_Channel_t in_Metro_Channel);

/* Set/Get offset Power compensation for each power type and calculation path (V+C) */
/* offset is coded under 10 bits */
void Metro_Set_Power_Offset_Compensation(METRO_Channel_t in_Metro_Channel, METRO_Power_selection_t in_Metro_Power_Selection, int16_t in_Metro_Power_Offset);
int16_t Metro_Get_Power_Offset_Compensation(METRO_Channel_t in_Metro_Channel, METRO_Power_selection_t in_Metro_Power_Selection);

/* Set the Ah accumulation thresholds for each calculation path. */
void Metro_Set_Ah_Accumulation_Down_Threshold(METRO_Channel_t in_Metro_Channel, uint16_t in_Metro_Ah_Down_threshold);
void Metro_Set_Ah_Accumulation_Up_Threshold(METRO_Channel_t in_Metro_Channel, uint16_t in_Metro_Ah_Up_Threshold);
uint16_t Metro_Get_Ah_Accumulation_Down_Threshold(METRO_Channel_t in_Metro_Channel);
uint16_t Metro_Get_Ah_Accumulation_Up_Threshold(METRO_Channel_t in_Metro_Channel);

/* Sag and Swell functions */
/****************************/
/*  Set timeout value for clearing sag and swell events bits after setting events clear bits for each voltage channel.Timout Value under 4 bits */
void Metro_Set_SAG_and_SWELL_Clear_Timeout(METRO_Channel_t in_Metro_Channel,uint8_t in_Metro_Sag_and_Swell_Clear_Timeout);

/*  Get timeout value for clearing sag and swell events bits after setting events clear bits for each voltage channel.Timout Value under 4 bits */
uint8_t Metro_Get_SAG_and_SWELL_Clear_Timeout(METRO_Channel_t in_Metro_Channel);

/* Clear the sag and swell event bits for each power type and calculation path (Vx-Cx couple). */
void Metro_Clear_SAG_and_SWELL_Event(METRO_Channel_t in_Metro_Channel);

/* Set/Get  the thresholds and the time to detect the sag event for each voltage channel. */
/* The time value is shared between the voltage channels. Time LSB is 8uSec. */
/* Setting threshold and time to zero disables the sag event detection */
/* Threshold value under 10 bits, Time value under 14 bits */
void Metro_Set_SAG_Config(METRO_Channel_t in_Metro_Channel,uint32_t in_Metro_SAG_Threshold,uint32_t in_Metro_SAG_detect_time);
void Metro_Get_SAG_Config(METRO_Channel_t in_Metro_Channel,uint32_t * out_p_Metro_SAG_Threshold,uint32_t * out_p_Metro_SAG_detect_time);

/* Set/Get voltage swell threshold for each voltage channel */
/* Threshold value under 10 bits */
void Metro_Set_V_SWELL_Config(METRO_Channel_t in_Metro_Channel,uint16_t in_Metro_V_SWELL_Threshold);
uint16_t  Metro_Get_V_SWELL_Config(METRO_Channel_t in_Metro_Channel);

/* Set/Get Current swell threshold for each current channel */
/* Threshold value under 10 bits */
void Metro_Set_C_SWELL_Config(METRO_Channel_t in_Metro_Channel,uint16_t in_Metro_C_SWELL_Threshold);
uint16_t Metro_Get_C_SWELL_Config(METRO_Channel_t in_Metro_Channel);

/* Read the  sag time counter for each voltage channel. */
uint16_t Metro_Read_SAG_Time(METRO_Channel_t in_Metro_Channel);

/* Read Current swell time counter for Voltage or current channel */
uint16_t Metro_Read_V_SWELL_Time(METRO_Channel_t in_Metro_Channel);
uint16_t Metro_Read_C_SWELL_Time(METRO_Channel_t in_Metro_Channel);

/* Set / Get Latch the device registers according to the latch type selection driving SYN pin  */
/* or setting auto-latch by S/W Auto Latch bit */
/* Latch_Type : SYN_SCS, SW, AUTO */

uint8_t Metro_Set_Latch_device_type(METRO_NB_Device_t in_Metro_Device, METRO_Latch_Device_Type_t in_Metro_Latch_Device_Type);
uint8_t Metro_Register_Latch_device_Config_type(METRO_NB_Device_t in_Metro_Device, METRO_Latch_Device_Type_t in_Metro_Latch_Device_Type);

/* ZCR management */
METRO_CMD_Device_t Metro_Get_ZCR(METRO_NB_Device_t in_Metro_Device, METRO_ZCR_Sel_t * out_p_ZCR_Sel_config);
void Metro_Set_ZCR(METRO_NB_Device_t in_Metro_Device, METRO_ZCR_Sel_t in_Metro_ZCR_Sel_config,METRO_CMD_Device_t in_Metro_CMD);

/* CLK management */
METRO_CMD_Device_t Metro_Get_CLK(METRO_NB_Device_t in_Metro_Device, METRO_CLK_Sel_t * out_p_CLK_Sel_config);
void Metro_Set_CLK(METRO_NB_Device_t in_Metro_Device, METRO_CLK_Sel_t in_Metro_CLK_Sel_config,METRO_CMD_Device_t in_Metro_CMD);

/* DSP Functions ****************************************************************/

/* Led config and speed ( the LEds are only used with stand alone chip -> STMET_only or STCOMET_only configuration)*/
/* With externals chip and multiple phases, the Leds will be managed by GPIOs with specific firmware calculation */
/* the hardware in this case can't manage the Leds output itself */
/* the speed divisor ( 4 bits) = 0 to 15 */

void Metro_Set_Led_Power_Config(METRO_NB_Device_t in_Metro_Device, METRO_LED_Selection_t in_Metro_LED_Selection,METRO_LED_Power_selection_t in_Metro_Power_Selection);
void Metro_Set_Led_Channel_Config(METRO_NB_Device_t in_Metro_Device, METRO_LED_Selection_t in_Metro_LED_Selection,METRO_LED_Channel_t in_Metro_LED_Channel);
void Metro_Set_Led_Speed_divisor(METRO_NB_Device_t in_Metro_Device, METRO_LED_Selection_t in_Metro_LED_Selection,uint8_t in_Metro_LED_Speed_divisor);
void Metro_Set_Led_On_Off(METRO_NB_Device_t in_Metro_Device, METRO_LED_Selection_t in_Metro_LED_Selection,METRO_CMD_Device_t in_Metro_CMD);


void Metro_Get_Led_Power_Config(METRO_NB_Device_t in_Metro_Device, METRO_LED_Selection_t in_Metro_LED_Selection,METRO_LED_Power_selection_t * out_p_Metro_Power_Selection);
void Metro_Get_Led_Channel_Config(METRO_NB_Device_t in_Metro_Device, METRO_LED_Selection_t in_Metro_LED_Selection,METRO_LED_Channel_t * out_p_Metro_LED_Channel);
uint8_t Metro_Get_Led_Speed_divisor(METRO_NB_Device_t in_Metro_Device,METRO_LED_Selection_t in_Metro_LED_Selection);
METRO_CMD_Device_t Metro_Get_Led_On_Off(METRO_NB_Device_t in_Metro_Device, METRO_LED_Selection_t in_Metro_LED_Selection);

/* Calibration function */
/************************/

/* Set/Get the calibration for the selected voltage channel. */
/* Calibrator value under 12 bits */
void Metro_Set_V_Calibration(METRO_Channel_t in_Metro_Channel, uint16_t in_Metro_V_calibrator_value);
uint16_t Metro_Get_V_Calibration(METRO_Channel_t in_Metro_Channel);

/* Set/Get the calibration for the selected current channel. */
/* Calibrator value under 12 bits */
void Metro_Set_C_Calibration(METRO_Channel_t in_Metro_Channel, uint16_t in_Metro_C_calibrator_value);
uint16_t Metro_Get_C_Calibration(METRO_Channel_t in_Metro_Channel);

/* Set/Get the phase calibration for the selected voltage channel. */
/* Calibrator value under 2 bits */
void Metro_Set_Phase_V_Calibration(METRO_Channel_t in_Metro_Channel, uint8_t in_Metro_Phase_V_calibrator_value);
uint8_t Metro_Get_Phase_V_Calibration(METRO_Channel_t in_Metro_Channel);

/* Set/Get the phase calibration for the selected current channel. */
/* Calibrator value under 10 bits */
void Metro_Set_Phase_C_Calibration(METRO_Channel_t in_Metro_Channel, uint16_t in_Metro_Phase_C_calibrator_value);
uint16_t Metro_Get_Phase_C_Calibration(METRO_Channel_t in_Metro_Channel);

/* READ functions for period, power, energy, voltage, current, phase for each channel */
/***************************************************************************************/

/* Read period for the selected channel */
uint16_t Metro_Read_Period(METRO_Channel_t in_Metro_Channel);

/* Read momentary voltage data for the selected channel. LSB according to the datasheet formula */
int32_t Metro_Read_Momentary_Voltage(METRO_Channel_t in_Metro_Channel, METRO_Voltage_type_t in_Metro_V_type);

/* Read momentary current data for the selected channel. LSB according to the datasheet formula */
int32_t Metro_Read_Momentary_Current(METRO_Channel_t in_Metro_Channel, METRO_Current_type_t in_Metro_C_type);

/* Read energy */
/* in_Metro_energy_Selection : W_ACTIVE , F_ACTIVE, REACTIVE, APPARENT */
int32_t Metro_Read_energy(METRO_Channel_t in_Metro_Channel,METRO_Energy_selection_t in_Metro_Energy_Selection);

/* Read Power */
/* in_Metro_Power_Selection : W_ACTIVE , F_ACTIVE, REACTIVE, APPARENT_RMS, APPARENT_VEC, MOM_WIDE_ACT, MOM_FUND_ACT */
int32_t Metro_Read_Power(METRO_Channel_t in_Metro_Channel,METRO_Power_selection_t in_Metro_Power_Selection);

/* Read RMS */
/* in_RAW_vs_RMS : 0 : Raw values from registers requestest at output, 1 : RMS values in mV or mA requested at output */
void Metro_Read_RMS(METRO_Channel_t in_Metro_Channel,uint32_t * out_Metro_RMS_voltage,uint32_t * out_Metro_RMS_current, uint8_t in_RAW_vs_RMS);

/* Read the phase angle between voltage and current for the selected channel. */
int32_t Metro_Read_PHI(METRO_Channel_t in_Metro_Channel);

/* Read AH accumulation for the selected channel. */
int32_t Metro_Read_AH_Acc(METRO_Channel_t in_Metro_Channel);

/* IRQ, STATUS, EVENTS for  each channel */
/***************************************************************************************/

/* STPM chip*/
/* The DSP_IRQ1 and DSP_IRQ2 registers are used */
/* DSPIRQ1 is hardware mapped to  channel V1 and C1 */
/* DSPIRQ2 is hardware mapped to  channel V2 and C2 */
/* Interrupts status mapped in DSP_IRQ1 and DSP_IRQ2 drive INT1 and INT2 pins respectively when enabled. */

/* IRQ Set/Get Mask for a channel */
void Metro_Set_IRQ_Mask_for_Channel(METRO_Channel_t in_Metro_Channel, uint32_t in_Metro_IT_Mask);
uint32_t Metro_Get_IRQ_Mask_for_Channel(METRO_Channel_t in_Metro_Channel);

/* Read the specified live event for a channel   */
uint32_t Metro_Read_Live_Event_from_Channel(METRO_Channel_t in_Metro_Channel, METRO_Live_Event_Type_t in_Metro_Live_Event_requested);

 /* Read the specified Status for a channel   */
uint32_t Metro_Read_Status_from_Channel(METRO_Channel_t in_Metro_Channel, METRO_Status_Type_t in_Metro_Status_requested);

 /* Clear the specified Status for a channel   */
void Metro_Clear_Status_for_Channel(METRO_Channel_t in_Metro_Channel, METRO_Status_Type_t in_Metro_Status_requested);

/* IRQ and status for STPM only ( IRQ and status for UART/SPI link between Host and STPM chips) */
void Metro_Set_IRQ_Mask_for_STPM_device( METRO_NB_Device_t in_Metro_Device_Id, uint16_t in_Metro_IT_Mask);
uint16_t Metro_Get_IRQ_Mask_from_STPM_device( METRO_NB_Device_t in_Metro_Device_Id);
uint16_t Metro_Read_Status_from_STPM_device(METRO_NB_Device_t in_Metro_Device_Id, METRO_STPM_LINK_IRQ_Status_Type_t in_Metro_Status_requested);
void Metro_Clear_Status_for_STPM_device(METRO_NB_Device_t in_Metro_Device_Id, METRO_STPM_LINK_IRQ_Status_Type_t in_Metro_Status_requested);


/* Read / Write data block from Device ( Reg access to External STPM from UART/SPI */
/*********************************************************************************************************/
uint8_t Metro_Get_Data_device(METRO_NB_Device_t in_Metro_Device);
uint8_t Metro_Read_Block_From_Device ( METRO_NB_Device_t in_Metro_Device_Id, uint8_t in_Metro_Device_Offset_Adress, uint8_t in_Metro_Nb_of_32b_Reg, uint32_t *p_buffer );
uint8_t Metro_Write_Block_to_Device ( METRO_NB_Device_t in_Metro_Device_Id, uint8_t in_Metro_Device_Offset_Adress, uint8_t in_Metro_Nb_of_32b_Reg, uint32_t *in_p_buffer );

uint8_t Metro_Ping_Metro(void);
uint32_t Metro_Get_SW_Rev(void);

#ifdef __cplusplus
}
#endif

#endif /* __METROLOGY_H */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
