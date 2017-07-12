/**
 ******************************************************************************
 * @file    metrology.c
 * @author  STMicroelectronics
 * @version V1.0
 * @date    17 May 2016
 * @brief   This file provides all the Metrology firmware functions.
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

/* Includes ------------------------------------------------------------------*/
#include "metrology.h"
#include "metrology_hal.h"
#include <stdint.h>

/** @defgroup Metrology
 * @brief Metrology driver modules
 * @{
 */

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/**
 * @brief  The assert_param macro is used for function's parameters check.
 * @param  expr If expr is false, it calls assert_failed function
 *   which reports the name of the source file and the source
 *   line number of the call that failed.
 *   If expr is true, it returns no value.
 * @retval None
 */
#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
void assert_failed(uint8_t* file, uint32_t line);
#else
#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static METRO_NB_Device_t Metro_Get_device_from_Channel(
		METRO_Channel_t in_Metro_Channel);
static METRO_internal_Channel_t Metro_Get_Internal_channel(
		METRO_Channel_t in_Metro_Channel, METRO_NB_Device_t in_Metro_device);

/* Global variables ----------------------------------------------------------*/
METRO_Device_Config_t Tab_METRO_internal_Devices_Config[NB_MAX_DEVICE];
METRO_Data_Energy_t METRO_Data;

/*===============================================================================
 Private  functions
 ===============================================================================*/
/**
 * @brief  Get device corresponding to Channel
 * @param[in]   in_Metro_Channel : Channel ID : Channel 1 to 4
 * @retval : METRO_NB_Device_t : device mapped
 */
static METRO_NB_Device_t Metro_Get_device_from_Channel(
		METRO_Channel_t in_Metro_Channel) {
	METRO_NB_Device_t device;
	uint8_t Channel_Mask = 0;

	/* Get channel mask according to Requested channel */
	switch (in_Metro_Channel) {
	case (CHANNEL_1):
		Channel_Mask = CHANNEL_MASK_CONF_CHANNEL_1;
		break;

	case (CHANNEL_2):
		Channel_Mask = CHANNEL_MASK_CONF_CHANNEL_2;
		break;

	case (CHANNEL_3):
		Channel_Mask = CHANNEL_MASK_CONF_CHANNEL_3;
		break;

	case (CHANNEL_4):
		Channel_Mask = CHANNEL_MASK_CONF_CHANNEL_4;
		break;
	case (CHANNEL_5):
		Channel_Mask = CHANNEL_MASK_CONF_CHANNEL_5;
		break;
	case (CHANNEL_6):
		Channel_Mask = CHANNEL_MASK_CONF_CHANNEL_6;
		break;
	case (CHANNEL_7):
		Channel_Mask = CHANNEL_MASK_CONF_CHANNEL_7;
		break;
	case (CHANNEL_8):
		Channel_Mask = CHANNEL_MASK_CONF_CHANNEL_8;
		break;
	}

	/* loop inside all devices configuration to get the device according to the channel requested  */
	for (device = EXT1; device < (NB_MAX_DEVICE); device++) {
		if (Channel_Mask
				& Tab_METRO_internal_Devices_Config[device].channels_mask) {
			return device;
		}
	}
	return EXT1;
}

/**
 * Changed Latest
 * @brief  Get device corresponding to Channel
 * Be careful, Device 1 is only in charge of Channel 1 and 2; Device 2: channel 3 and 4;
 * All device type is STPM34
 * Device 3: channel 5 and 6; Device 4: channel 7 and 8;
 * @param[in]   in_Metro_Channel : Channel ID : Channel 1 to 8
 * @param[in]   in_Metro_device : Device ID    : Host or EXT1 to 4
 * @retval : METRO_internal_Channel_t : device mapped
 */
static METRO_internal_Channel_t Metro_Get_Internal_channel(
		METRO_Channel_t in_Metro_Channel, METRO_NB_Device_t in_Metro_device) {
	/* Get int channel of device according to Requested user channel */
	switch (in_Metro_device) {
	/* if it is a host */
	case (HOST):
		return INT_NONE_CHANNEL;
		break;

		/* it is an external chip */
	case (EXT1):
		if (in_Metro_Channel == CHANNEL_1) {
			return INT_CHANNEL_1;
		} else if (in_Metro_Channel == CHANNEL_2) {
			return INT_CHANNEL_2;
		}
		break;
		/* it is an external chip */
	case (EXT2):
		if (in_Metro_Channel == CHANNEL_3) {
			return INT_CHANNEL_1;
		} else if (in_Metro_Channel == CHANNEL_4) {
			return INT_CHANNEL_2;
		}
		break;
		/* it is an external chip */
	case (EXT3):
		if (in_Metro_Channel == CHANNEL_5) {
			return INT_CHANNEL_1;
		} else if (in_Metro_Channel == CHANNEL_6) {
			return INT_CHANNEL_2;
		}
		break;
	case (EXT4):
		if (in_Metro_Channel == CHANNEL_7) {
			return INT_CHANNEL_1;
		} else if (in_Metro_Channel == CHANNEL_8) {
			return INT_CHANNEL_2;
		}
		break;

	}
	return INT_NONE_CHANNEL;
}

/** @defgroup Metrology_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions
 *
 @verbatim
 ===============================================================================
 Initialization and Configuration functions
 ===============================================================================

 This section provides a set of functions allowing to initialize the Metrology
 Peripherals.

 @endverbatim
 * @{
 */

/**
 * @brief  Initialize the Metrology peripheral registers to their default
 *         reset values.
 * @param[in]   None
 * @retval None
 */
void Metro_Init(void) {
	/* Ask HAL to initilisase the METROLOGY part (Metrology Block for int and ext if necessary) ( Clocks, ..) */

	for (METRO_NB_Device_t i = EXT1; i < (NB_MAX_DEVICE); i++) {
		if (Tab_METRO_internal_Devices_Config[i].device != 0) {
			Metro_HAL_init_device((METRO_NB_Device_t) i);
		}
	}
}

/**
 * @brief  Initialize the Metrology peripheral registers to their default
 *         reset values.
 * @param[in]   None
 * @retval None
 */
void Metro_power_up_device(void) {
	/* Ask HAL to enable the METROLOGY part with the right CSS level */

	for (METRO_NB_Device_t i = EXT1; i < (NB_MAX_DEVICE); i++) {
		if (Tab_METRO_internal_Devices_Config[i].device != 0) {
			Metro_HAL_power_up_device((METRO_NB_Device_t) i);
		}
	}
}

/**
 * @brief  Resets the Metrology peripherals
 * @param[in]   in_MetroResetType : Reset type  RESET_SYN_SCS = 1, RESET_SW = 2
 * @retval None
 */
void Metro_Config_Reset(METRO_ResetType_t in_MetroResetType) {

	/* Set the reset in each Metrology chip if necessary */
	for (METRO_NB_Device_t i = EXT1; i < (NB_MAX_DEVICE); i++) {
		if (Tab_METRO_internal_Devices_Config[i].device != 0) {
			Metro_HAL_reset_device(in_MetroResetType, (METRO_NB_Device_t) i);
		}
	}
}

#ifdef UART_XFER_STPM3X /* UART MODE */   
/**
 * @brief  Initialize the Metrology peripheral registers to their default
 *         reset values.
 * @param[in]   None
 * @retval None
 */
void Metro_UartSpeed(uint32_t baudrate)
{
	/* Ask HAL to initilisase the METROLOGY part (Metrology Block for int and ext if necessary) ( Clocks, ..) */

	for (METRO_NB_Device_t i=EXT1;i<(NB_MAX_DEVICE);i++)
	{
		if(Tab_METRO_internal_Devices_Config[i].device != 0)
		{
			Metro_HAL_baudrate_set(i,baudrate);
			Metro_HAL_baudrate_set(HOST,baudrate);
		}
	}
}
#endif

/**
 * @brief  Get SW revision version from Header File
 * @param[in]   None
 *
 * @retval U32
 */
uint32_t Metro_Get_SW_Rev(void) {
	return METRO_SW_REVISION;
}

/**
 * @brief  set metrology Config. Changed Latest. Remove STPM type field.
 * We assume that all devices are  STPM34. And if CH masks is 0, we set STPM type to 0, otherwise it is STPM34 (8)
 * @param[in]   in_stpm_config : STPM  topology config
 * @retval u8
 */

/******************/
/* in_stpm_config */
/******************/
/*+------------------------------------------------------------------------------------+
 |                                        U32                                         |
 |---------------------|-------------------|-------------------|----------------------|
 |     STPM EXT4       |     STPM EXT3     |     STPM EXT2     |     STPM EXT1        |
 |---------------------|-------------------|-------------------|----------------------|
 |    u4   |   u4    |   u4    |    u4   |
 |---------|-----------|--------------------------------------------------------------|
 |CH masks |CH masks |CH masks |CH masks |
 |---------|-----------|--------------------------------------------------------------|

 STPM CFG EXTx (u8):
 -----------------
 MSB u4 : Channel  Mask :  Channels affected to STPM
 0 : No Channel affected
 1 : Channel 1 affected
 2 : Channel 2 affected
 4 : Channel 3 affected
 8 : Channel 4 affected

 LSB u4 :  STPM type : 8
 0 : No STPM
 8 : STPM34

 EX : STPM EXT 1: One STPM34 with Channels 2 and 3 affected on it
 MSB u4 = 6 ( 4:Channel 3 + 2:Channel 2)

 STPM CONFIG : U32 = 0x00000006

 +------------------------------------------------------------------------------------+*/

/* set metrology Config  */
uint8_t Metro_Setup(uint32_t in_host_config, uint32_t in_stpm_config) {
	/* Get Device type and channels config  for Host and  fill the table  -> Tab[0] is the Host*/
	Tab_METRO_internal_Devices_Config[HOST].device =
			(METRO_Device_t) (in_host_config & 0x0000000F);
	Tab_METRO_internal_Devices_Config[HOST].channels_mask =
			(uint8_t) ((in_host_config & 0x000000F0) >> 4);

	/* Get STPM devices types and channels config  for each STPM , fill the table -> tab [1] to tab [4] for Ext1 to EXT4 STPM*/
	Tab_METRO_internal_Devices_Config[EXT1].channels_mask =
			(uint8_t) (0x000000C0);
	if (Tab_METRO_internal_Devices_Config[EXT1].channels_mask == 0) {
		Tab_METRO_internal_Devices_Config[EXT1].device = Device_NONE;
	} else {
		Tab_METRO_internal_Devices_Config[EXT1].device = STPM34;
	}

	Tab_METRO_internal_Devices_Config[EXT2].channels_mask =
			(uint8_t) (0x0000000C);
	if (Tab_METRO_internal_Devices_Config[EXT2].channels_mask == 0) {
		Tab_METRO_internal_Devices_Config[EXT2].device = Device_NONE;
	} else {
		Tab_METRO_internal_Devices_Config[EXT2].device = STPM34;
	}

	Tab_METRO_internal_Devices_Config[EXT3].channels_mask =
			(uint8_t) (0x00000030);
	if (Tab_METRO_internal_Devices_Config[EXT3].channels_mask == 0) {
		Tab_METRO_internal_Devices_Config[EXT3].device = Device_NONE;
	} else {
		Tab_METRO_internal_Devices_Config[EXT3].device = STPM34;
	}

	Tab_METRO_internal_Devices_Config[EXT4].channels_mask =
			(uint8_t) (0x00000000);
	if (Tab_METRO_internal_Devices_Config[EXT4].channels_mask == 0) {
		Tab_METRO_internal_Devices_Config[EXT4].device = Device_NONE;
	} else {
		Tab_METRO_internal_Devices_Config[EXT4].device = STPM34;
	}

	/* Send Config to HAL */
	Metro_HAL_Setup(
			(METRO_Device_Config_t *) &Tab_METRO_internal_Devices_Config);

	return 0;

}

/**
 * @brief  Get metrology Config
 * @param[out]   out_p_stpm_config : STPM  topology config
 * @retval u8
 */
/* set metrology Config  */
uint8_t Metro_Get_Setup(uint32_t * out_p_host_config,
		uint32_t * out_p_stpm_config) {
	*out_p_host_config =
			(uint32_t) Tab_METRO_internal_Devices_Config[HOST].device;
	*out_p_host_config |=
			(uint32_t) (Tab_METRO_internal_Devices_Config[HOST].channels_mask
					<< 4);

	/* Get STPM devices types and channels config  for each STPM , for Ext1 to EXT4 STPM*/
	*out_p_stpm_config =
			(uint32_t) Tab_METRO_internal_Devices_Config[EXT1].device;
	*out_p_stpm_config |=
			(uint32_t) (Tab_METRO_internal_Devices_Config[EXT1].channels_mask
					<< 4);

	*out_p_stpm_config |=
			(uint32_t) (Tab_METRO_internal_Devices_Config[EXT2].device << 8);
	*out_p_stpm_config |=
			(uint32_t) (Tab_METRO_internal_Devices_Config[EXT2].channels_mask
					<< 12);

	*out_p_stpm_config |=
			(uint32_t) (Tab_METRO_internal_Devices_Config[EXT3].device << 16);
	*out_p_stpm_config |=
			(uint32_t) (Tab_METRO_internal_Devices_Config[EXT3].channels_mask
					<< 20);

	*out_p_stpm_config |=
			(uint32_t) (Tab_METRO_internal_Devices_Config[EXT4].device << 24);
	*out_p_stpm_config |=
			(uint32_t) (Tab_METRO_internal_Devices_Config[EXT4].channels_mask
					<< 28);

	return 0;
}

/**
 * @brief       set Power and NRJ factors depending of hardware for Channel 1, 2
 * @param[in]   in_Metro_Channel (Channel ID), Channel 1, 2
 * @param[in]   in_Factor_Power : Power factor depending of hardware part sensor in W/LSB
 * @param[in]   in_Factor_Nrj   : Enargy factor depending of hardware part sensor in Wh/LSB
 * @param[in]   in_Factor_Voltage : Voltage factor depending of hardware part sensor in W/LSB
 * @param[in]   in_Factor_Current   : Current factor depending of hardware part sensor in Wh/LSB
 * @retval     None

 // Parameters used for Power factor and NRJ factors calculation to help the user of this function

 #define DPOW         //  NB bits of Internal Regs
 #define CALIB_V      // Default Voltage Calibration value
 #define CALIB_I      // Default Current Calibration value
 #define KINT         // Integrator gain : used only for rogowski
 #define R1           // External resistor for ADC
 #define R2           // External Resistor for ADC
 #define AU           //( ADC Voltage Gain )
 #define AI           // (ADC Current Gain )
 #define VREF         //( ADC vref)
 #define KSI          // Sensivity of Captor
 #define F_XTAL       // frenquency of the Main Clock


 // EX  values for a shunt in Comet board
 #define DPOW        (268435456.0)   // put 2^28 for 28 bits
 #define CALIB_V      (0.875)  //default Calibration value (Middle value)
 #define CALIB_I      (0.875) //default Calibration value (Midle Value)
 #define KINT        (0.8155773)         // this value is used only for rogowski,  otherwise = 1
 #define R1          (780000.0)  // External resistor for ADC
 #define R2          (470.0)    // External Resistor for ADC
 #define AU          (2.0)   //( ADC Voltage Gain )
 #define AI          (2.0)   // (ADC Current Gain )
 #define VREF        (1.18)  //( ADC vref)
 #define KSI         (0.00218)    // Value for a shunt KSI = RShunt in ohms
 #define F_XTAL      (16000000.0)  // frenquency of the Main Clock



 KSI = RShunt  (For shunt)
 KSI =  Rb/N  (For a VAC T60404-E4626-X002) : Rb = 6.8 ohms for Comet board  and N = 2500 Nb spires of CT -> KSI = 0.00272) theoric
 KSI = K_RoCoil (For Rogowski Coil)


 // FORMULAS
 **************

 with shunt or current transformer
 ---------------------------------

 power factor = VREF * VREF * (1 + (R1 / R2)) /
 (AU * AI * KSI * CALIB_V * CALIB_I * DPOW);

 So for the factor calculation, remove dpow (2^28) to have keep  precision.
 The Read power and nrj functions will make a shift of 28 bits before to send the result.

 our case with a CT in comet board
 ----------------------------------
 KSI =  Rb/N  (For a VAC T60404-E4626-X002) : Rb = 6.8 ohms for Comet board  and N = 2500 Nb spires of CT -> KSI = 0.00272) theoric

 power factor  = (1,18 *1,18 * (1+ (780000/470)) / (2 * 2 * 0,00272  * 0,875 * 0,875) =  277573,09

 our case with a CT in STPM34 board
 ----------------------------------
 KSI =  Rb/N  (For a VAC T60404-E4626-X002) : Rb = 6.5 ohms for STPM board board  and N = 2500 Nb spires of CT -> KSI = 0.00240) theoric

 power factor  = (1,18 *1,18 * (1+ (810000/470)) / (2 * 2 * 0,00260  * 0,875 * 0,875) =  301546,05



 it is not necessary to calculate the NRJ factor when you have already calculated the power factor.
 It is always the same ration between Power and NRJ

 Our case FACTOR_POWER_ON_ENERGY      (858)   ->  (3600 * 16000000 / 0x4000000) = 858.3...


 However  there is the formula below :

 energy  = reg * VREF * VREF * (1 + R1 / R2) /
 (3600 * (1<<17) * CALIB_V * CALIB_I * AI * AU * KSI * (F_XTAL / 2048));

 With Rogowski ( KINT added inside the formulas)
 -------------------------------------------------

 power = reg * VREF * VREF * (1 + (R1 / R2)) /
 (KINT * AU * AI * KSI * DPOW * CALIB_V * CALIB_I);

 energy  = reg * VREF * VREF  *(1 + R1 / R2) /
 (3600 * (1<<17) * KINT * CALIB_V * CALIB_I * AI * AU * KSI * (F_XTAL / 2048));


 for STPM34  with CT :
 --------------------

 V factor = VREF * (1 + (R1 / R2)) / (AU * CALIB_V * DPOW)
 = (1.18 * (1+ (810000/470))) / ( 2 * 0,875) = 116274,11


 Power factor  = 301546,05

 Ratio PF / VF = 301546,05 / 116274,11 = 2,5934

 DPOW is 2^23 for Momentary values


 for STPM34  with CT :
 --------------------

 I factor = VREF  / (AI * CALIB_I * DPOW * KSI * KINT ) =
 = 1.18  / (2 * 0,875 * 0,00260 * 1)   = 25934,06

 Power factor  = 301546,05
 Ration PF /IF = 301546,05 / 25934,06 =  11,6274

 DPOW is 2^23 for Momentary values
 */
void Metro_Set_Hardware_Factors(METRO_Channel_t in_Metro_Channel,
		uint32_t in_Factor_Power, uint32_t in_Factor_Nrj,
		uint32_t in_Factor_Voltage, uint32_t in_Factor_Current) {
	METRO_NB_Device_t Device;
	METRO_internal_Channel_t int_Channel;

	/* Get Device id from Channel */
	Device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get if the channel requested is the one or the two of the device */
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, Device);

	/* Copy Factor NRJ and Factor Power inside strcut of device to correct internal Channel */
	if (int_Channel == INT_CHANNEL_1) {
		Tab_METRO_internal_Devices_Config[Device].factor_power_int_ch1 =
				in_Factor_Power;
		Tab_METRO_internal_Devices_Config[Device].factor_energy_int_ch1 =
				in_Factor_Nrj;
		Tab_METRO_internal_Devices_Config[Device].factor_voltage_int_ch1 =
				in_Factor_Voltage;
		Tab_METRO_internal_Devices_Config[Device].factor_current_int_ch1 =
				in_Factor_Current;
	} else if ((int_Channel == INT_CHANNEL_2)
			|| (int_Channel == CHANNEL_TAMPER)) {
		Tab_METRO_internal_Devices_Config[Device].factor_power_int_ch2 =
				in_Factor_Power;
		Tab_METRO_internal_Devices_Config[Device].factor_energy_int_ch2 =
				in_Factor_Nrj;
		Tab_METRO_internal_Devices_Config[Device].factor_voltage_int_ch2 =
				in_Factor_Voltage;
		Tab_METRO_internal_Devices_Config[Device].factor_current_int_ch2 =
				in_Factor_Current;
	}

}

/**
 * @brief  Set new UART baud rate for a device
 *  in_baudrate, in_baudrate
 * @param[in]   in_Metro_Device (Device ID ), HOST or EXT1 to EXT4
 * @param[in]   in_baudrate : New baudrate
 * @retval U8 : 0 : Return OK, -1 Error
 */
uint8_t Metro_Set_uart_baudrate_to_device(METRO_NB_Device_t in_Metro_Device_Id,
		uint32_t in_baudrate) {

#ifdef UART_XFER_STPM3X /* UART MODE */   
	return Metro_HAL_baudrate_set(in_Metro_Device_Id, in_baudrate);
#endif

}

/**
 * @brief  Get a block of registers from a device (Internal or external Metro Chip)
 * @param[in]   in_Metro_Device (Device ID ) EXT1 to EXT4
 * @param[in]   in_Metro_Device_Offset_Adress : Offset Address
 * @param[in]   in_Metro_Nb_of_32b_Reg : NB U32 to read
 * @param[out]   p_buffer : Output buffer filled with read data
 * @retval U8 : 0 : Return OK, -1 Error
 */
uint8_t Metro_Read_Block_From_Device(METRO_NB_Device_t in_Metro_Device_Id,
		uint8_t in_Metro_Device_Offset_Adress, uint8_t in_Metro_Nb_of_32b_Reg,
		uint32_t *p_buffer) {
	uint8_t error = 0;

	error = Metrology_HAL_ReadBlock(in_Metro_Device_Id,
			in_Metro_Device_Offset_Adress, in_Metro_Nb_of_32b_Reg, p_buffer);

	return error;
}
/**
 * @brief  Write a block of registers from a device ( Internal or external Metro Chip)
 * @param[in]   in_Metro_Device (Device ID ) EXT1 to EXT4
 * @param[in]   in_Metro_Device_Offset_Adress : Offset Address
 * @param[in]   in_Metro_Nb_of_32b_Reg : NB U32 to write
 * @param[in]   in_p_buffer : Buffer data to write
 * @param[out]  None
 * @retval U8 : 0 : Return OK, -1 Error
 */
uint8_t Metro_Write_Block_to_Device(METRO_NB_Device_t in_Metro_Device_Id,
		uint8_t in_Metro_Device_Offset_Adress, uint8_t in_Metro_Nb_of_32b_Reg,
		uint32_t *in_p_buffer) {
	uint8_t error = 0;

	error = Metrology_HAL_WriteBlock(in_Metro_Device_Id,
			in_Metro_Device_Offset_Adress, in_Metro_Nb_of_32b_Reg, in_p_buffer);

	return error;
}

/**
 * @brief  Ping Function to say : Metro Task is alive !!!!
 * @param[in]   None
 * @param[out]  None
 * @retval U8 : 0 : Return OK
 */
uint8_t Metro_Ping_Metro(void) {

	return 0;
}

/**
 * @brief  Get DSP data inside the device
 * @param[in]   in_Metro_Device (Device ID ), EXT1 to EXT4
 * @param[out]  None
 * @retval     0 : Return OK, -1 Error
 */

uint8_t Metro_Get_Data_device(METRO_NB_Device_t in_Metro_Device) {

	/* Get DSP data inside the Metrology chip requested  */
	if (in_Metro_Device < (NB_MAX_DEVICE)) {
		Metro_HAL_Get_Data_device(in_Metro_Device);
	}

	return 0;

}

/**
 * @brief  Set Latch the device type (according to the latch type selection driving SYN pin)
 * or setting auto-latch by S/W Auto Latch bit,
 * @param[in]   in_Metro_Device (Device ID ), HOST or EXT1 to EXT4
 * @param[in]   in_Metro_Latch_Device_Type :Latch_Type : SYN_SCS, SW, AUTO
 * @param[out]  None
 * @retval     0 : Return OK, -1 Error
 */

uint8_t Metro_Set_Latch_device_type(METRO_NB_Device_t in_Metro_Device,
		METRO_Latch_Device_Type_t in_Metro_Latch_Device_Type) {

	/* Set latch type  in the Metrology chip requested  */
	if (in_Metro_Device < (NB_MAX_DEVICE)) {
		Metro_HAL_Set_Latch_device_type(in_Metro_Device,
				in_Metro_Latch_Device_Type);
	}

	return 0;

}

/**
 * @brief  Save Latch device type
 * @param[in]   in_Metro_Device (Device ID )EXT1 to EXT4
 * @param[in]   in_Metro_Latch_Device_Type :Latch_Type : SYN_SCS, SW, AUTO
 * @param[out]  None
 * @retval     0 : Return OK, -1 Error
 */
uint8_t Metro_Register_Latch_device_Config_type(
		METRO_NB_Device_t in_Metro_Device,
		METRO_Latch_Device_Type_t in_Metro_Latch_Device_Type) {

	/* Save latch config inside internal Metro struct */
	Tab_METRO_internal_Devices_Config[in_Metro_Device].latch_device_type =
			in_Metro_Latch_Device_Type;

	return 0;
}

/**
 * @brief  Set VRef
 * @param[in]   METRO_Channel_t in_Metro_Channel
 * @param[in]   in_Metro_Vref : METRO_Vref_t :  EXT_VREF = 0, INT_VREF = 1
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_Vref(METRO_Channel_t in_Metro_Channel,
		METRO_Vref_t in_Metro_Vref) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device */
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set Vref inside device according to channel requested*/
	Metro_HAL_Set_Vref(device, int_Channel, in_Metro_Vref);
}

/**
 * @brief  Get VRef  of devices
 * @param[in]   METRO_Channel_t in_Metro_Channel
 * @param[out]  None
 * @retval        METRO_Vref_t :  EXT_VREF = 0, INT_VREF = 1
 */
METRO_Vref_t Metro_Get_Vref(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device */
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	return Metro_HAL_Get_Vref(device, int_Channel);
}

/**
 * @brief  Set Temperature_Compensation of devices
 * @param[in]  METRO_Channel_t in_Metro_Channel
 * @param[in]   in_Metro_TC_Value ( 3 LSB bits used among u8)

 Table for comet (TCO is the  LSB bit):
 TC0	TC1	TC2	VBG (V)	        TC_VBG (ppm/鐧�)
 0	0	0	1.183	        -50           in_Metro_TC_Value = 0
 0	0	1	1.193	        -25           in_Metro_TC_Value = 4
 0	1	0	1.202	        0             in_Metro_TC_Value = 2
 0	1	1	1.211	        25            in_Metro_TC_Value = 6
 1	0	0	1.220	        50            in_Metro_TC_Value = 1
 1	0	1	1.229	        75            in_Metro_TC_Value = 5
 1	1	0	1.240	        100           in_Metro_TC_Value = 3
 1	1	1	1.247	        125           in_Metro_TC_Value = 7

 table for stpm3x (TCO is the LSB bit):
 TC0  TC1     TC2        VREF (V)      TC_VREF (ppm/鐧�)
 0     0       0       1.16            -50           in_Metro_TC_Value = 0
 0     0       1       1.17            -25           in_Metro_TC_Value = 4
 0     1       0       1.18             0 (default)  in_Metro_TC_Value = 2
 0     1       1       1.19            25            in_Metro_TC_Value = 6
 1     0       0       1.2             50            in_Metro_TC_Value = 1
 1     0       1       1.21            75            in_Metro_TC_Value = 5
 1     1       0       1.22            100           in_Metro_TC_Value = 3
 1     1       1       1.225           125           in_Metro_TC_Value = 7

 * @param[out]  None
 * @retval      None
 */
void Metro_Set_Temperature_Compensation(METRO_Channel_t in_Metro_Channel,
		uint8_t in_Metro_TC_Value) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device */
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set Temperature_Compensation inside device*/
	Metro_HAL_Set_Temperature_Compensation(device, int_Channel,
			in_Metro_TC_Value);
}

/**
 * @brief  Get Temperature_Compensation of devices
 * @param[in]  METRO_Channel_t in_Metro_Channel
 * @param[out]  None
 * @retval      u8 Metro_TC_Value
 */
uint8_t Metro_Get_Temperature_Compensation(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device */
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	return Metro_HAL_Get_Temperature_Compensation(device, int_Channel);
}

/**
 * @brief  Enable and set the tamper tolerance for detecting unbalanced active energy between the two current channels.
 * @param[in]   METRO_Channel_t in_Metro_Channel ( Tamper Channel should be passed only )
 * @param[in]   in_Metro_Tamper_Tolerance   TOL_12_5 = 0,  TOL_8_33 = 1, TOL_6_25 = 2, TOL_3_125 = 3 or NO_CHANGE_TOL = 4 ( if no change of tolerance is requested)
 TMP_TOL[1:0] Tamper tolerance
 0x00 TOL = 12.5%
 0x01 TOL = 8.33%
 0x10 TOL = 6.25%
 0x11 TOL = 3.125%
 * @param[in]   in_Metro_CMD :   DEVICE_DISABLE = 0,  DEVICE_ENABLE = 1   No_CHANGE = 2 ( if No change is requested about enable bit)
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_Tamper(METRO_Channel_t in_Metro_Channel,
		METRO_Tamper_Tolerance_t in_Metro_Tamper_Tolerance,
		METRO_CMD_Device_t in_Metro_CMD) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set tamper tolerance and enable or disable tamper */
	Metro_HAL_Set_Tamper(device, int_Channel, in_Metro_Tamper_Tolerance,
			in_Metro_CMD);
}

/**
 * @brief  Get Tamper tolerance
 * @param[in]   None
 * @param[out]  Metro_Tamper_Tolerance  : TOL_12_5 = 1,  TOL_8_33 = 2, TOL_6_25 = 3, TOL_3_125 = 4
 * @retval     in_Metro_CMD :   DEVICE_DISABLE = 0,  DEVICE_ENABLE = 1
 */
METRO_CMD_Device_t Metro_Get_Tamper(METRO_Channel_t in_Metro_Channel,
		METRO_Tamper_Tolerance_t * out_p_Tamper_Tolerance) {

	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set tamper tolerance and enable or disable tamper */
	return (Metro_HAL_Get_Tamper(device, int_Channel, out_p_Tamper_Tolerance));
}

/**
 * @brief  Enable/Disable and set the ZCR config.
 * @param[in]   METRO_NB_Device_t in_Metro_Device (Device ID ), HOST or EXT1 to EXT4
 * @param[in]   METRO_ZCR_Sel_t in_Metro_ZCR_Sel_config  :   ZCR_SEL_V1 = 0, ZCR_SEL_C1, ZCR_SEL_V2, ZCR_SEL_C2
 * @param[in]   in_Metro_CMD :   DEVICE_DISABLE = 0,  DEVICE_ENABLE = 1   No_CHANGE = 2 ( if No change is requested about enable bit)
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_ZCR(METRO_NB_Device_t in_Metro_Device,
		METRO_ZCR_Sel_t in_Metro_ZCR_Sel_config,
		METRO_CMD_Device_t in_Metro_CMD) {

	/* Enable/Disable and set the ZCR config */
	Metro_HAL_Set_ZCR(in_Metro_Device, in_Metro_ZCR_Sel_config, in_Metro_CMD);
}

/**
 * @brief       Get Zero Crossing config
 * @param[in]   METRO_NB_Device_t in_Metro_Device (Device ID ), HOST or EXT1 to EXT4
 * @param[out]  METRO_ZCR_Sel_t * out_p_ZCR_Sel_config  :   ZCR_SEL_V1 = 0,  ZCR_SEL_C1, ZCR_SEL_V2, ZCR_SEL_C2
 * @retval      Metro_CMD :   DEVICE_DISABLE = 0,  DEVICE_ENABLE = 1
 */
METRO_CMD_Device_t Metro_Get_ZCR(METRO_NB_Device_t in_Metro_Device,
		METRO_ZCR_Sel_t * out_p_ZCR_Sel_config) {

	/* Get Zero Crossing config*/
	return (Metro_HAL_Get_ZCR(in_Metro_Device, out_p_ZCR_Sel_config));
}

/**
 * @brief  Enable/Disable and set the ZCR config.
 * @param[in]   METRO_NB_Device_t in_Metro_Device (Device ID ), HOST or EXT1 to EXT4
 * @param[in]   METRO_CLK_Sel_t in_Metro_CLK_Sel_config :   CLK_SEL_7KHz = 0,  CLK_SEL_4MHz, CLK_SEL_4MHz_50,CLK_SEL_16MHz
 * @param[in]   in_Metro_CMD :   DEVICE_DISABLE = 0,  DEVICE_ENABLE = 1   No_CHANGE = 2 ( if No change is requested about enable bit)
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_CLK(METRO_NB_Device_t in_Metro_Device,
		METRO_CLK_Sel_t in_Metro_CLK_Sel_config,
		METRO_CMD_Device_t in_Metro_CMD) {
	/* Enable/Disable and set the CLK config */
	Metro_HAL_Set_CLK(in_Metro_Device, in_Metro_CLK_Sel_config, in_Metro_CMD);
}

/**
 * @brief       Get CLK config
 * @param[in]   METRO_NB_Device_t in_Metro_Device (Device ID ), HOST or EXT1 to EXT4
 * @param[out]  METRO_CLK_Sel_t * out_p_CLK_Sel_config :   CLK_SEL_7KHz = 0,  CLK_SEL_4MHz, CLK_SEL_4MHz_50,CLK_SEL_16MHz
 * @retval      Metro_CMD :   DEVICE_DISABLE = 0,  DEVICE_ENABLE = 1
 */
METRO_CMD_Device_t Metro_Get_CLK(METRO_NB_Device_t in_Metro_Device,
		METRO_CLK_Sel_t * out_p_CLK_Sel_config) {
	/* Get CLK config*/
	return (Metro_HAL_Get_CLK(in_Metro_Device, out_p_CLK_Sel_config));
}

/**
 * @brief       Set Led Power Config.
 * @param[in]   METRO_NB_Device_t in_Metro_Device (Device ID ), HOST or EXT1 to EXT4
 * @param[in]   METRO_LED_Selection_t in_Metro_LED_Selection  :     LED1 = 1,  LED2 = 2
 * @param[in]   METRO_LED_Power_selection_t in_Metro_Power_Selection :   LED_W_ACTIVE = 0,  LED_F_ACTIVE, LED_REACTIVE, LED_APPARENT_RMS
 * @param[Out]  None
 * @retval      None
 */
void Metro_Set_Led_Power_Config(METRO_NB_Device_t in_Metro_Device,
		METRO_LED_Selection_t in_Metro_LED_Selection,
		METRO_LED_Power_selection_t in_Metro_Power_Selection) {
	/* Set Led Power config  */
	Metro_HAL_Set_Led_Power_Config(in_Metro_Device, in_Metro_LED_Selection,
			in_Metro_Power_Selection);
}

/**
 * @brief       Set Led Channel Config.
 * @param[in]   METRO_NB_Device_t in_Metro_Device (Device ID ), HOST or EXT1 to EXT4
 * @param[in]   METRO_LED_Selection_t in_Metro_LED_Selection  :     LED1 = 1,  LED2 = 2
 * @param[in]   METRO_LED_Channel_t in_Metro_LED_Channel :    PRIMARY = 0,  SECONDARY, ALGEBRIC, SIGMA_DELTA
 * @param[Out]  None
 * @retval      None
 */
void Metro_Set_Led_Channel_Config(METRO_NB_Device_t in_Metro_Device,
		METRO_LED_Selection_t in_Metro_LED_Selection,
		METRO_LED_Channel_t in_Metro_LED_Channel) {
	/* Set Led Channel config  */
	Metro_HAL_Set_Led_Channel_Config(in_Metro_Device, in_Metro_LED_Selection,
			in_Metro_LED_Channel);
}

/**
 * @brief       Get Led Power Config.
 * @param[in]   METRO_NB_Device_t in_Metro_Device (Device ID ), HOST or EXT1 to EXT4
 * @param[in]   METRO_LED_Selection_t in_Metro_LED_Selection  :     LED1 = 1,  LED2 = 2
 * @param[out]  METRO_LED_Power_selection_t * out_p_Metro_Power_Selection :   LED_W_ACTIVE = 0,  LED_F_ACTIVE, LED_REACTIVE, LED_APPARENT_RMS
 * @retval      None
 */
void Metro_Get_Led_Power_Config(METRO_NB_Device_t in_Metro_Device,
		METRO_LED_Selection_t in_Metro_LED_Selection,
		METRO_LED_Power_selection_t * out_p_Metro_Power_Selection) {
	/* Get Led config  */
	Metro_HAL_Get_Led_Power_Config(in_Metro_Device, in_Metro_LED_Selection,
			out_p_Metro_Power_Selection);
}

/**
 * @brief       Get Led Channel  Config.
 * @param[in]   METRO_NB_Device_t in_Metro_Device (Device ID ), HOST or EXT1 to EXT4
 * @param[in]   METRO_LED_Selection_t in_Metro_LED_Selection  :     LED1 = 1,  LED2 = 2
 * @param[out]  METRO_LED_Channel_t * out_p_Metro_LED_Channel :    PRIMARY = 0,  SECONDARY, ALGEBRIC, SIGMA_DELTA
 * @retval      None
 */
void Metro_Get_Led_Channel_Config(METRO_NB_Device_t in_Metro_Device,
		METRO_LED_Selection_t in_Metro_LED_Selection,
		METRO_LED_Channel_t * out_p_Metro_LED_Channel) {
	/* Get Led Channel config  */
	Metro_HAL_Get_Led_Channel_Config(in_Metro_Device, in_Metro_LED_Selection,
			out_p_Metro_LED_Channel);
}

/**
 * @brief       Set Led Speed divisor.
 * @param[in]   METRO_NB_Device_t in_Metro_Device (Device ID ), HOST or EXT1 to EXT4
 * @param[in]   METRO_LED_Selection_t in_Metro_LED_Selection  :     LED1 = 1,  LED2 = 2
 * @param[in]   u8 in_Metro_LED_Speed_divisor  :     0 to 15 ( 4 bits only used amoung the U8)
 * @param[Out]  None
 * @retval      None
 */
void Metro_Set_Led_Speed_divisor(METRO_NB_Device_t in_Metro_Device,
		METRO_LED_Selection_t in_Metro_LED_Selection,
		uint8_t in_Metro_LED_Speed_divisor) {
	/* Set Led Speed divisor  */
	Metro_HAL_Set_Led_Speed_divisor(in_Metro_Device, in_Metro_LED_Selection,
			in_Metro_LED_Speed_divisor);

}

/**
 * @brief       Get Led Speed divisor..
 * @param[in]   METRO_NB_Device_t in_Metro_Device (Device ID ), HOST or EXT1 to EXT4
 * @param[in]   METRO_LED_Selection_t in_Metro_LED_Selection  :     LED1 = 1,  LED2 = 2
 * @retval      u8 Metro_LED_Speed_divisor  :     0 to 15 ( 4 bits only used amoung the U8)
 */
uint8_t Metro_Get_Led_Speed_divisor(METRO_NB_Device_t in_Metro_Device,
		METRO_LED_Selection_t in_Metro_LED_Selection) {
	uint8_t Led_speed_divisor = 0;

	/* Get Led Spped divisor  */
	Led_speed_divisor = Metro_HAL_Get_Led_Speed_divisor(in_Metro_Device,
			in_Metro_LED_Selection);

	return Led_speed_divisor;
}

/**
 * @brief       Set Led On Off.
 * @param[in]   METRO_NB_Device_t in_Metro_Device (Device ID ), HOST or EXT1 to EXT4
 * @param[in]   METRO_LED_Selection_t in_Metro_LED_Selection  :     LED1 = 1,  LED2 = 2
 * @param[in]   METRO_CMD_Device_t in_Metro_CMD  :  DEVICE_DISABLE = 0,  DEVICE_ENABLE = 1
 * @param[Out]  None
 * @retval      None
 */
void Metro_Set_Led_On_Off(METRO_NB_Device_t in_Metro_Device,
		METRO_LED_Selection_t in_Metro_LED_Selection,
		METRO_CMD_Device_t in_Metro_CMD) {
	/* Set Led On Off  */
	Metro_HAL_Set_Led_On_Off(in_Metro_Device, in_Metro_LED_Selection,
			in_Metro_CMD);

}

/**
 * @brief       Get Led On Off.
 * @param[in]   METRO_NB_Device_t in_Metro_Device (Device ID ), HOST or EXT1 to EXT4
 * @param[in]   METRO_LED_Selection_t in_Metro_LED_Selection  :     LED1 = 1,  LED2 = 2
 * @retval      METRO_CMD_Device_t in_Metro_CMD  :  DEVICE_DISABLE = 0,  DEVICE_ENABLE = 1
 */
METRO_CMD_Device_t Metro_Get_Led_On_Off(METRO_NB_Device_t in_Metro_Device,
		METRO_LED_Selection_t in_Metro_LED_Selection) {
	METRO_CMD_Device_t Led_On_Off = DEVICE_DISABLE;

	/* Get Led On Off  status*/
	Led_On_Off = Metro_HAL_Get_Led_On_Off(in_Metro_Device,
			in_Metro_LED_Selection);

	return Led_On_Off;
}

/**
 * @brief  : Read period for the selected channel.
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to to CHANNEL_4
 * @param[out]  None
 * @retval U16 :  :Return Period in 绁�
 */

/* Read period for the selected channel. LSB is 8uSec. */
uint16_t Metro_Read_Period(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t Device;
	uint16_t period = 0;

	/* Get Device id from Channel to request the value from the good device */
	Device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get if the channel requested is the one or the two */
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, Device);

	/* Get period according to device and channel */
	period = Metro_HAL_read_period(Device, int_Channel);

	/* LSB is 8 绁�, so multiply by 8 to have the value in 绁� */
	period = period * 8;

	return period;
}

/**
 * @brief      This function Read Power according to the selected type for the given channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   in_Metro_Power_Selection : W_ACTIVE , F_ACTIVE, REACTIVE, APPARENT_RMS, APPARENT_VEC, MOM_WIDE_ACT, MOM_FUND_ACT
 * @param[out]  None
 * @retval     Return power value in  in mW  , mVAR  or mVA ...
 */
int32_t Metro_Read_Power(METRO_Channel_t in_Metro_Channel,
		METRO_Power_selection_t in_Metro_Power_Selection) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t Device;
	int64_t calc_power = 0;
	int32_t raw_power = 0;

	/* Get Device id from Channel to request the value from the good device */
	Device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get if the channel requested is the one or the two */
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, Device);

	/* Get raw power according to device and channel */
	raw_power = Metro_HAL_read_power(Device, int_Channel,
			in_Metro_Power_Selection);

	/* Calc real power  */
	if (int_Channel == INT_CHANNEL_1) {
		calc_power =
				(int64_t) raw_power
						* Tab_METRO_internal_Devices_Config[Device].factor_power_int_ch1;
	} else if (int_Channel == INT_CHANNEL_2) {
		calc_power =
				(int64_t) raw_power
						* Tab_METRO_internal_Devices_Config[Device].factor_power_int_ch2;
	}

	/* multiply by 10 to have in milli- */
	calc_power *= 10;

	/* Shift calcul result to 28 bits ( resolution of Reg inside metrology block)*/
	calc_power >>= 28;

	/* return power selection calculated with Factor Power */
	return ((int32_t) calc_power);
}

/**
 * @brief      This function Read Energy according to the selected type for the given channel
 * @param[in]   in_Metro_Channel (Channel ID), CHANNEL_1 to CHANNEL_2
 * @param[in]   in_Metro_energy_Selection : W_ACTIVE , F_ACTIVE, REACTIVE, APPARENT
 * @param[out]  None
 * @retval     Return NRJ value in mWh , mVARh  or mVAh ...
 */
int32_t Metro_Read_energy(METRO_Channel_t in_Metro_Channel,METRO_Energy_selection_t in_Metro_Energy_Selection)
{
	METRO_internal_Channel_t int_Channel;
	  METRO_NB_Device_t        Device;
	  int64_t calc_nrj = 0;
	  int32_t raw_nrj = 0;
      int32_t extension = 0;
      int64_t factor1 =0;
      int64_t factor2 =0;
	  /* Get Device id from Channel to request the value from the good device */
	  Device = Metro_Get_device_from_Channel(in_Metro_Channel);

	  /* Get if the channel requested is the one or the two */
	  int_Channel =  Metro_Get_Internal_channel(in_Metro_Channel,Device);


	  /* Get raw nrj according to device, channel and NRJ type */
	  raw_nrj = Metro_HAL_read_energy(Device,int_Channel,in_Metro_Energy_Selection);

    	if ( METRO_Data.energy[in_Metro_Channel][in_Metro_Energy_Selection] > raw_nrj && raw_nrj <0 )
    		 METRO_Data.energy_extension[in_Metro_Channel][in_Metro_Energy_Selection] ++;

	  /* manage the 2 U32 to have enougth place to save energy cumulated */
	  /* Make sure between two reads inside hardware registers if we have to add carry inside ext U32 */


	  /* save the new result cumulated come from register inside internal structure */
	  METRO_Data.energy[in_Metro_Channel][in_Metro_Energy_Selection] = raw_nrj;
      extension = METRO_Data.energy_extension[in_Metro_Channel][in_Metro_Energy_Selection];
      factor1 = (int64_t)Tab_METRO_internal_Devices_Config[Device].factor_energy_int_ch1;
      factor2 = (int64_t)Tab_METRO_internal_Devices_Config[Device].factor_energy_int_ch2;
	  /* calculate the nrj value and add the 32 bits extension */
	  calc_nrj = (int64_t)raw_nrj + ((int64_t)METRO_Data.energy_extension[in_Metro_Channel][in_Metro_Energy_Selection] << 32);

	   /* Apply Energy factors  */
	  if (int_Channel == INT_CHANNEL_1)
	  {
	   calc_nrj *= (int64_t)Tab_METRO_internal_Devices_Config[Device].factor_energy_int_ch1;
	  }
	  else if (int_Channel == INT_CHANNEL_2)
	  {
	    calc_nrj *= (int64_t)Tab_METRO_internal_Devices_Config[Device].factor_energy_int_ch2;
	  }

	  /* multiply by 10 to have in milli- */
	  calc_nrj *= 10;

	  /* Put the result in 32 bits format */
	  calc_nrj >>= 32;

	  /* return the nrj value */
	  return((int32_t)calc_nrj);


}

/**
 * @brief      This function Read momentary voltage data for the selected channel.
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to to CHANNEL_44
 * @param[in]   in_Metro_V_type :   V_WIDE = 1,  V_FUND = 2
 * @param[out]  None
 * @retval     Return Momentary Voltage value in mV

 for STPM34  with CT :
 --------------------

 V factor = VREF * (1 + (R1 / R2)) / (AU * CALIB_V * DPOW)
 = (1.18 * (1+ (810000/470))) / ( 2 * 0,875) = 116274,11


 Power factor  = 301546,05

 Ratio PF / VF = 301546,05 / 116274,11 = 2,5934

 DPOW is 2^23 for Momentary values
 */
int32_t Metro_Read_Momentary_Voltage(METRO_Channel_t in_Metro_Channel,
		METRO_Voltage_type_t in_Metro_V_type) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t Device;
	int32_t raw_M_Voltage = 0;
	int32_t calc_M_Voltage = 0;
	uint32_t Factor_Voltage = 0;

	/* Get Device id from Channel */
	Device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get if the channel requested is the one or the two of the device */
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, Device);

	/* Get raw power according to device and channel */
	raw_M_Voltage = Metro_HAL_read_Momentary_Voltage(Device, int_Channel,
			in_Metro_V_type);

	/* gat Voltage  factor to calculate the Momentary values */
	if (int_Channel == INT_CHANNEL_1) {
		Factor_Voltage =
				Tab_METRO_internal_Devices_Config[Device].factor_voltage_int_ch1;
	} else {
		Factor_Voltage =
				Tab_METRO_internal_Devices_Config[Device].factor_voltage_int_ch2;
	}

	/* Calculate real values with factors */
	calc_M_Voltage = (int64_t) raw_M_Voltage * Factor_Voltage;

	/* Multiply  by 10 to have in milli  */
	calc_M_Voltage *= 10;

	/* Shift calcul result to 23 bits ( resolution of Reg inside metrology block)*/
	calc_M_Voltage >>= 23;

	/* return Voltage selection calculated with Factor Voltage in mV */
	return ((int32_t) calc_M_Voltage);

}

/**
 * @brief      This function Read momentary current data for the selected channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to to CHANNEL_4
 * @param[in]   METRO_Current_type_t :     C_WIDE = 1,  C_FUND = 2
 * @param[out]  None
 * @retval     Return Momentary Current value in mA

 for STPM34  with CT :
 --------------------

 I factor = VREF  / (AI * CALIB_I * DPOW * KSI * KINT ) =
 = 1.18  / (2 * 0,875 * 0,00260 * 1)   = 25934,06

 Power factor  = 301546,05
 Ration PF /IF = 301546,05 / 25934,06 =  11,6274

 DPOW is 2^23 for Momentary values
 */
int32_t Metro_Read_Momentary_Current(METRO_Channel_t in_Metro_Channel,
		METRO_Current_type_t in_Metro_C_type) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t Device;
	int32_t raw_M_Current = 0;
	int32_t calc_M_Current = 0;
	int32_t Factor_Current = 0;

	/* Get Device id from Channel */
	Device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get if the channel requested is the one or the two of the device */
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, Device);

	/* Get raw Momentary Current according to device and channel */
	raw_M_Current = Metro_HAL_read_Momentary_Current(Device, int_Channel,
			in_Metro_C_type);

	/* gat  current factor to calculate the Momentary values */
	if (int_Channel == INT_CHANNEL_1) {
		Factor_Current =
				Tab_METRO_internal_Devices_Config[Device].factor_current_int_ch1;
	} else {
		Factor_Current =
				Tab_METRO_internal_Devices_Config[Device].factor_current_int_ch2;
	}

	/* Calculate real values with factors */
	calc_M_Current = (int64_t) raw_M_Current * Factor_Current;

	/* Multiply  by 10 to have in milli  */
	calc_M_Current *= 10;

	/* Shift calcul result to 23 bits ( resolution of Reg inside metrology block)*/
	calc_M_Current >>= 23;

	/* return Cuurent selection calculated with Factor Current in mA */
	return ((int32_t) calc_M_Current);
}

/**
 * @brief      This function Set the current gain in  the given channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to to CHANNEL_4
 * @param[in]   in_Metro_Gain : X2 , X4 ,X8 , X16
 * @param[out]  None
 * @retval     Return Status
 */
uint8_t Metro_Set_Current_gain(METRO_Channel_t in_Metro_Channel,
		METRO_Gain_t in_Metro_Gain) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device */
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set Gain */
	Metro_HAL_Set_Gain(device, int_Channel, in_Metro_Gain);

	return 0;
}

/**
 * @brief      This function Get the current gain from the given channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to to CHANNEL_4
 * @param[out]  None
 * @retval     Return METRO_Gain_t Gain of channel requested : X2 , X4 ,X8 , X16
 */
METRO_Gain_t Metro_Get_Current_gain(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device */
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Get Gain  and return it */
	return Metro_HAL_Get_Gain(device, int_Channel);

}

/**
 * @brief      This function read RMS values of both voltage and current for the selected channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_3 ( Channel 4 is excluded because is TAMPER )
 * @param[in]   in_RAW_vs_RMS : 0 : Raw values from registers requestest at output, 1 : RMS values in mV or mA requested at output
 * @param[out]  out_Metro_RMS_voltage , out_Metro_RMS_current ( in mV and mA) or RAW values from registers
 * @retval     None


 for STPM34  with CT :
 --------------------

 V factor = VREF * (1 + (R1 / R2)) / (AU * CALIB_V * DPOW)
 = (1.18 * (1+ (810000/470))) / ( 2 * 0,875) = 116274,11

 I factor = VREF  / (AI * CALIB_I * DPOW * KSI * KINT ) =
 = 1.18  / (2 * 0,875 * 0,00260 * 1)   = 25934,06

 Power factor  = 301546,05
 DPOW is 2^15 for RMS values
 */

void Metro_Read_RMS(METRO_Channel_t in_Metro_Channel,
		uint32_t * out_Metro_RMS_voltage, uint32_t * out_Metro_RMS_current,
		uint8_t in_RAW_vs_RMS) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t Device;
	uint32_t raw_RMS_Voltage = 0;
	uint64_t calc_RMS_Voltage = 0;
	uint32_t raw_RMS_Current = 0;
	uint64_t calc_RMS_Current = 0;
	uint32_t Factor_Voltage = 0;
	uint32_t Factor_Current = 0;

	/* Get Device id from Channel */
	Device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get if the channel requested is the one or the two of the device */
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, Device);

	/* Get raw RMS voltage according to device and channel */
	raw_RMS_Voltage = Metro_HAL_read_RMS_Voltage(Device, int_Channel);

	/* Get RAW RMS current according to device and channel */
	raw_RMS_Current = Metro_HAL_read_RMS_Current(Device, int_Channel);

	/* gat Voltage and current factors to calculate the RMS values */
	if (int_Channel == INT_CHANNEL_1) {
		Factor_Voltage =
				Tab_METRO_internal_Devices_Config[Device].factor_voltage_int_ch1;
		Factor_Current =
				Tab_METRO_internal_Devices_Config[Device].factor_current_int_ch1;
	} else {
		Factor_Voltage =
				Tab_METRO_internal_Devices_Config[Device].factor_voltage_int_ch2;
		Factor_Current =
				Tab_METRO_internal_Devices_Config[Device].factor_current_int_ch2;
	}

	/* Calculate real values with factors */
	calc_RMS_Voltage = (uint64_t) raw_RMS_Voltage * Factor_Voltage;
	calc_RMS_Current = (uint64_t) raw_RMS_Current * Factor_Current;

	/* Multiply  by 10 to have in milli  */
	calc_RMS_Voltage *= 10;
	calc_RMS_Current *= 10;

	/* Shift calcul result to 15 bits ( resolution of Reg inside metrology block)*/
	calc_RMS_Voltage >>= 15;

	calc_RMS_Current >>= 17; // 17 bits resolution revBC

	/* if Raw type requested , return registers otherwise return calc values in mV and mA RMS */
	if (in_RAW_vs_RMS == 0) {
		/* Return RAW values from registers to ouput  */
		*out_Metro_RMS_voltage = raw_RMS_Voltage;
		*out_Metro_RMS_current = raw_RMS_Current;
	} else if (in_RAW_vs_RMS == 1) {
		/* Return values to ouput params in mV and mA */
		*out_Metro_RMS_voltage = (uint32_t) calc_RMS_Voltage;
		*out_Metro_RMS_current = (uint32_t) calc_RMS_Current;
	}

}

/**
 * @brief      This function Read the phase angle between voltage and current for the selected channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_3 ( Channel 4 is excluded =  TAMPER )
 * @param[out]  None
 * @retval     Return S32 , PHI angle in degree for given channel
 */
int32_t Metro_Read_PHI(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t Device;
	int32_t raw_phi = 0;
	int32_t Calc_phi = 0;
	uint16_t period = 0;

	/* Get period of signal , return is in 绁� */
	period = Metro_Read_Period(in_Metro_Channel);

	/* Get Device id from Channel */
	Device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get if the channel requested is the one or the two of the device */
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, Device);

	/* Get raw PHI according to device and channel */
	raw_phi = Metro_HAL_read_PHI(Device, int_Channel);

	/* Calc real PHI according to signal period and frequency of metro block  */
	/* phase angle = Calc_phi *f*360/Fclk */
	Calc_phi = (int64_t) ((raw_phi * 2880) / period);

	/* return Phase angle in degree calculated  */
	return ((int32_t) Calc_phi);
}

/**
 * @brief      This function Read AH accumulation for the selected channel.
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_3 ( Channel 4 is excluded =  TAMPER )
 * @param[out]  None
 * @retval     Return S32 , AH accumulation for the selected channel
 */
int32_t Metro_Read_AH_Acc(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t Device;
	int32_t raw_AG_ACC = 0;

	/* Get Device id from Channel */
	Device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get if the channel requested is the one or the two of the device */
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, Device);

	/* Get AH accumulation according to device and channel */
	raw_AG_ACC = Metro_HAL_Read_AH_Acc(Device, int_Channel);

	/* return AH accumulation   */
	return ((int32_t) raw_AG_ACC);
}

/**
 * @brief  enable/disable HightPass filters ( DC remover )  for Current  Channel (  C )
 * @param[in]   METRO_Channel_t in_Metro_Channel  (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   in_Metro_CMD :   DEVICE_DISABLE = 0,  DEVICE_ENABLE = 1
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_Current_HP_Filter(METRO_Channel_t in_Metro_Channel,
		METRO_CMD_Device_t in_Metro_CMD) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set Current HP Filter */
	Metro_HAL_Set_Current_HP_Filter(device, int_Channel, in_Metro_CMD);
}

/**
 * @brief  enable/disable HightPass filters ( DC remover )  for Voltage Channel (  V )
 * @param[in]   METRO_Channel_t in_Metro_Channel  (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   in_Metro_CMD :   DEVICE_DISABLE = 0,  DEVICE_ENABLE = 1
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_Voltage_HP_Filter(METRO_Channel_t in_Metro_Channel,
		METRO_CMD_Device_t in_Metro_CMD) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set Set Voltage HP Filter */
	Metro_HAL_Set_Voltage_HP_Filter(device, int_Channel, in_Metro_CMD);
}

/**
 * @brief      Get configs for HP filters for Current Channel requested
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[out]  None
 * @retval     Return METRO_CMD_Device_t :   DEVICE_DISABLE = 0,  DEVICE_ENABLE = 1,
 */
METRO_CMD_Device_t Metro_Get_Current_HP_Filter(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  return Current HP Filter config */
	return (Metro_HAL_Get_Current_HP_Filter(device, int_Channel));
}

/**
 * @brief      Get configs for HP filters for Voltage Channel requested
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[out]  None
 * @retval     Return METRO_CMD_Device_t :   DEVICE_DISABLE = 0,  DEVICE_ENABLE = 1,
 */
METRO_CMD_Device_t Metro_Get_Voltage_HP_Filter(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  return Voltage HP Filter config */
	return (Metro_HAL_Get_Voltage_HP_Filter(device, int_Channel));
}

/**
 * @brief       Enable or disable Coil integrator (Rogowski) for a  channel
 * @param[in]   METRO_Channel_t in_Metro_Channel  (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   in_Metro_CMD :   DEVICE_DISABLE = 0,  DEVICE_ENABLE = 1
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_Coil_integrator(METRO_Channel_t in_Metro_Channel,
		METRO_CMD_Device_t in_Metro_CMD) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set Enable or disable Coil integrator */
	Metro_HAL_Set_Coil_integrator(device, int_Channel, in_Metro_CMD);
}

/**
 * @brief      Get configs for Coil integrator Rogowski  for Channel requested
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[out]  None
 * @retval     Return METRO_CMD_Device_t :   DEVICE_DISABLE = 0,  DEVICE_ENABLE = 1,
 */
METRO_CMD_Device_t Metro_Get_Coil_integrator(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  return Get Coil integrator config (Rogowski)*/
	return (Metro_HAL_Get_Coil_integrator(device, int_Channel));
}

/**
 * @brief       Set Ah Accumulation Down Threshold for a  channel
 * @param[in]   METRO_Channel_t in_Metro_Channel  (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   u16  :   Ah Accumulation Down Threshold
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_Ah_Accumulation_Down_Threshold(METRO_Channel_t in_Metro_Channel,
		uint16_t in_Metro_Ah_Down_threshold) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set Ah Accumulation Down Threshold */
	Metro_HAL_Set_Ah_Accumulation_Down_Threshold(device, int_Channel,
			in_Metro_Ah_Down_threshold);
}
/**
 * @brief       Set Ah Accumulation up Threshold for a  channel
 * @param[in]   METRO_Channel_t in_Metro_Channel  (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   u16  :   Ah Accumulation Up Threshold
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_Ah_Accumulation_Up_Threshold(METRO_Channel_t in_Metro_Channel,
		uint16_t in_Metro_Ah_Up_Threshold) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set Ah Accumulation Up Threshold */
	Metro_HAL_Set_Ah_Accumulation_Up_Threshold(device, int_Channel,
			in_Metro_Ah_Up_Threshold);
}

/**
 * @brief      Get Ah Accumulation Down Threshold
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[out]  None
 * @retval     Return u16 :   Ah Accumulation Down Threshold
 */
uint16_t Metro_Get_Ah_Accumulation_Down_Threshold(
		METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  return Ah Accumulation Down Threshold*/
	return (Metro_HAL_Get_Ah_Accumulation_Down_Threshold(device, int_Channel));
}

/**
 * @brief      Get Ah Accumulation up Threshold
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[out]  None
 * @retval     Return u16 :   Ah Accumulation Up Threshold
 */
uint16_t Metro_Get_Ah_Accumulation_Up_Threshold(
		METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  return Ah Accumulation Up Threshold*/
	return (Metro_HAL_Get_Ah_Accumulation_Up_Threshold(device, int_Channel));
}

/**
 * @brief       Set offset Power compensation for each power type and Channel
 * @param[in]   METRO_Channel_t in_Metro_Channel  (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   METRO_Power_selection_t in_Metro_Power_Selection
 * @param[in]   s16 in_Metro_Power_Offset : offset is coded under 10 bits
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_Power_Offset_Compensation(METRO_Channel_t in_Metro_Channel,
		METRO_Power_selection_t in_Metro_Power_Selection,
		int16_t in_Metro_Power_Offset) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set offset Power compensation according to power type and Channel*/
	Metro_HAL_Set_Power_Offset_Compensation(device, int_Channel,
			in_Metro_Power_Selection, in_Metro_Power_Offset);
}

/**
 * @brief       Get offset Power compensation from power type and Channel
 * @param[in]   METRO_Channel_t in_Metro_Channel  (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   METRO_Power_selection_t in_Metro_Power_Selection
 * @param[out]  None
 * @retval     Return s16 : Metro_Power_Offset : offset is coded under 10 bits
 */
int16_t Metro_Get_Power_Offset_Compensation(METRO_Channel_t in_Metro_Channel,
		METRO_Power_selection_t in_Metro_Power_Selection) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  return offset Power compensation from power type and Channel*/
	return (Metro_HAL_Get_Power_Offset_Compensation(device, int_Channel,
			in_Metro_Power_Selection));
}

/**
 * @brief       Set the calibration for the selected voltage channel
 * @param[in]   METRO_Channel_t in_Metro_Channel  (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   u16 in_Metro_V_calibrator_value (Calibrator value under 12 bits)
 * @retval      None
 */
void Metro_Set_V_Calibration(METRO_Channel_t in_Metro_Channel,
		uint16_t in_Metro_V_calibrator_value) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set the calibration for the selected voltage channel */
	Metro_HAL_Set_V_Calibration(device, int_Channel,
			in_Metro_V_calibrator_value);
}

/**
 * @brief        Get the calibration for the selected voltage channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[out]  None
 * @retval     Return u16 in_Metro_V_calibrator_value  (Calibrator value under 12 bits)
 */
uint16_t Metro_Get_V_Calibration(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  return he calibration for the selected voltage channel*/
	return (Metro_HAL_Get_V_Calibration(device, int_Channel));
}

/**
 * @brief       Set the calibration for the selected current channel
 * @param[in]   METRO_Channel_t in_Metro_Channel  (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   u16 in_Metro_C_calibrator_value (Calibrator value under 12 bits )
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_C_Calibration(METRO_Channel_t in_Metro_Channel,
		uint16_t in_Metro_C_calibrator_value) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set the calibration for the selected current channel  */
	Metro_HAL_Set_C_Calibration(device, int_Channel,
			in_Metro_C_calibrator_value);
}

/**
 * @brief      Get the calibration for the selected current channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[out]  None
 * @retval     Return u16 in_Metro_C_calibrator_value (Calibrator value under 12 bits )
 */
uint16_t Metro_Get_C_Calibration(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  return the calibration for the selected current channel*/
	return (Metro_HAL_Get_C_Calibration(device, int_Channel));
}

/**
 * @brief       Set the phase calibration for the selected voltage channel
 * @param[in]   METRO_Channel_t in_Metro_Channel  (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   u8 in_Metro_Phase_V_calibrator_value :   Calibrator value under 2 bits
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_Phase_V_Calibration(METRO_Channel_t in_Metro_Channel,
		uint8_t in_Metro_Phase_V_calibrator_value) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set the phase calibration for the selected voltage channel */
	Metro_HAL_Set_Phase_V_Calibration(device, int_Channel,
			in_Metro_Phase_V_calibrator_value);
}

/**
 * @brief        Get the phase calibration for the selected voltage channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[out]  None
 * @retval     Return u8 in_Metro_Phase_V_calibrator_value (Calibrator value under 2 bits)
 */
uint8_t Metro_Get_Phase_V_Calibration(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  return the phase calibration for the selected voltage channel*/
	return (Metro_HAL_Get_Phase_V_Calibration(device, int_Channel));
}

/**
 * @brief       Set the phase calibration value for the selected current channel
 * @param[in]   METRO_Channel_t in_Metro_Channel  (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   in_Metro_Phase_C_calibrator_value (Calibrator value under 10 bits)
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_Phase_C_Calibration(METRO_Channel_t in_Metro_Channel,
		uint16_t in_Metro_Phase_C_calibrator_value) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set the phase calibration value for the selected  channel  */
	Metro_HAL_Set_Phase_C_Calibration(device, int_Channel,
			in_Metro_Phase_C_calibrator_value);
}

/**
 * @brief      Get the phase calibration value for the selected current channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[out]  None
 * @retval     in_Metro_Phase_C_calibrator_value (Calibrator value under 10 bits)
 */
uint16_t Metro_Get_Phase_C_Calibration(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  return the phase calibration value for the selected current channel*/
	return (Metro_HAL_Get_Phase_C_Calibration(device, int_Channel));
}

/******************************************************************************/
/*********             IRQ - STATUS - DSP LIVE EVENTS                   *******/
/******************************************************************************/

/*
 The device detects and monitors events like sag and swell, tamper, energy register
 overflow, power sign change and errors, generating an interrupt signal on INTx pins when
 the masked event is triggered.

 When the event is triggered, the correspondent bit is set in two registers:
 Live event register DSP_EV1,2
 Status (also called interrupt) register DSP_SR1,2

 To output the interrupt on INTx pins, the correspondent bit should be set in the interrupt
 control mask register DSP_IRQ1,2

 Live event register
 ------------------
 In live event registers (DSP_EV1 and DSP_EV2), events are set and cleared by DSP at the
 sampling rate DCLK = 7,8125 kHz.


 Interrupt control mask register
 ---------------------------------

 Each bit in the status register has a correspondent bit in DSP_IRQ1, DSP_IRQ2 interrupt
 mask registers. For each bit set, the relative event detection is output on INT1, INT2 pins
 respectively. Concerning negative sign bits, interrupt raises every sign change (0 to 1 and 1
 to 0 transitions).

 In the STPM32, DSP_IRQ1 is mapped on INT1 pin only.
 Status bits can be monitored by an external application, in fact when INTx
 pin triggers, the application reads the relative status register content and clears it.

 Status interrupt register
 -------------------------

 When an event is detected, DSP sets the status register (DSP_SR1 and DSP_SR2) bits
 that remain latched, even if the event ceases, until they are cleared to zero by a write
 operation.

 */
/* STPM chip*/
/* The DSP_IRQ1 and DSP_IRQ2 registers are used */
/* DSPIRQ1 is hardware mapped to  channel V1 and C1 */
/* DSPIRQ2 is hardware mapped to  channel V2 and C2 */
/* Interrupts status mapped in DSP_IRQ1 and DSP_IRQ2 drive INT1 and INT2 pins respectively when enabled. */

/***********************************************/
/* STPM 34    DSP_IRQ1  Control Mask REG       */
/***********************************************/

/* 

 PH1+PH2 IRQ CR
 Bit 0 Sign change total active power
 Bit 1 Sign change total reactive power
 Bit 2 Overflow total active energy
 Bit 3 Overflow total reactive energy

 PH2 IRQ CR
 Bit 4 Sign change secondary channel active power
 Bit 5 Sign change secondary channel active fundamental power
 Bit 6 Sign change secondary channel reactive power
 Bit 7 Sign change secondary channel apparent power
 Bit 8 Overflow secondary channel active energy
 Bit 9 Overflow secondary channel active fundamental energy
 Bit 10 Overflow secondary channel reactive energy
 Bit 11 Overflow secondary channel apparent energy

 PH1 IRQ CR
 Bit 12 Sign change primary channel active power
 Bit 13 Sign change primary channel active fundamental power
 Bit 14 Sign change primary channel reactive power
 Bit 15 Sign change primary channel apparent power
 Bit 16 Overflow primary channel active energy
 Bit 17 Overflow primary channel active fundamental energy
 Bit 18 Overflow primary channel reactive energy
 Bit 19 Overflow primary channel apparent energy

 C1 IRQ CR
 Bit 20 Primary current sigma-delta bitstream stuck
 Bit 21 AH1 - accumulation of primary channel current
 Bit 22 Primary current swell start
 Bit 23 Primary current swell end

 V1 IRQ CR
 Bit 24 Primary voltage sigma-delta bitstream stuck
 Bit 25 Primary voltage period error
 Bit 26 Primary voltage sag start
 Bit 27 Primary voltage sag end
 Bit 28 Primary voltage swell start
 Bit 29 Primary voltage swell end

 Tamper
 Bit 30 Tamper on primary
 Bit 31 Tamper or wrong connection

 ***********************************************
 * STPM 34    DSP_IRQ2  Control Mask REG       *
 ***********************************************


 PH1+PH2 IRQ CR
 Bit 0 Sign change total active power
 Bit 1 Sign change total reactive power
 Bit 2 Overflow total active energy
 Bit 3 Overflow total reactive energy

 PH2 IRQ CR
 Bit 4 Sign change secondary channel active power
 Bit 5 Sign change secondary channel active fundamental power
 Bit 6 Sign change secondary channel reactive power
 Bit 7 Sign change secondary channel apparent power
 Bit 8 Overflow secondary channel active energy
 Bit 9 Overflow secondary channel active fundamental energy
 Bit 10 Overflow secondary channel reactive energy
 Bit 11 Overflow secondary channel apparent energy

 PH1 IRQ CR
 Bit 12 Sign change primary channel active power
 Bit 13 Sign change primary channel active fundamental power
 Bit 14 Sign change primary channel reactive power
 Bit 15 Sign change primary channel apparent power
 Bit 16 Overflow primary channel active energy
 Bit 17 Overflow primary channel active fundamental energy
 Bit 18 Overflow primary channel reactive energy
 Bit 19 Overflow primary channel apparent energy

 C2 IRQ CR
 Bit 20 Secondary current sigma-delta bitstream stuck
 Bit 21 AH1 - accumulation of primary channel current
 Bit 22 Secondary current swell start
 Bit 23 Secondary current swell end

 V2 IRQ CR
 Bit 24 Secondary voltage sigma-delta bitstream stuck
 Bit 25 Secondary voltage period error
 Bit 26 Secondary voltage sag start
 Bit 27 Secondary voltage sag end
 Bit 28 Secondary voltage swell start
 Bit 29 Secondary voltage swell end

 Tamper
 Bit 30 Tamper on Secondary
 Bit 31 Tamper or wrong connection

 */

/**
 * @brief       Set IRQ Mask for a channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   in_Metro_IT_Mask :  Mask of interruption according to the channel
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_IRQ_Mask_for_Channel(METRO_Channel_t in_Metro_Channel,
		uint32_t in_Metro_IT_Mask) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set the IRQ Mask for the selected channel according to device */
	Metro_HAL_Set_IRQ_Mask_for_Channel(device, int_Channel, in_Metro_IT_Mask);
}

/**
 * @brief      Get IRQ Mask for a channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[out]  None
 * @retval     u32 Metro_IT_Mask
 */
uint32_t Metro_Get_IRQ_Mask_for_Channel(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  return the IRQ Mask for the selected channel according to device*/
	return (Metro_HAL_Get_IRQ_Mask_for_Channel(device, int_Channel));
}

/******************************************************************************/
/*********             DSP LIVE EVENTS                                  *******/
/******************************************************************************/

/*
 ****************************************
 * STPM34 LIVE EVENT REGISTER  DSP_EV1 *
 ***************************************


 PH1+PH2 events
 Bit 0 Sign total active power
 Bit 1 Sign total reactive power
 Bit 2 Overflow total active energy
 Bit 3 Overflow total reactive energy

 PH1 events
 Bit 4 Sign primary channel active power
 Bit 5 Sign primary channel active fundamental power
 Bit 6 Sign primary channel reactive power
 Bit 7 Sign primary channel apparent power
 Bit 8 Overflow primary channel active energy
 Bit 9 Overflow primary channel active fundamental energy
 Bit 10 Overflow primary channel reactive energy
 Bit 11 Overflow primary channel apparent energy

 C1 events
 Bit 12 Primary current zero-crossing
 Bit 13 Primary current sigma-delta bitstream stuck
 Bit 14 Primary current AH accumulation
 Bits 15:18  Primary current swell event history

 V1 events
 Bit 19 Primary voltage zero-crossing
 Bit 20 Primary voltage sigma-delta bitstream stuck
 Bit 21 Primary voltage period error (out of range)
 Bit 22:25 Primary voltage swell event history
 Bit 26:29 Primary voltage sag event history
 
 Bit 30 Reserved
 Bit 31 Reserved

 ****************************************
 * STPM34 LIVE EVENT REGISTER  DSP_EV2 *
 ***************************************


 PH1+PH2 events
 Bit 0 Sign total active power
 Bit 1 Sign total reactive power
 Bit 2 Overflow total active energy
 Bit 3 Overflow total reactive energy

 PH2 events
 Bit 4 Sign secondary channel active power
 Bit 5 Sign secondary channel active fundamental power
 Bit 6 Sign secondary channel reactive power
 Bit 7 Sign secondary channel apparent power
 Bit 8 Overflow secondary channel active energy
 Bit 9 Overflow secondary channel active fundamental energy
 Bit 10 Overflow secondary channel reactive energy
 Bit 11 Overflow secondary channel apparent energy

 C2 events
 Bit 12 Secondary current zero-crossing
 Bit 13 Secondary current sigma-delta bitstream stuck
 Bit 14 Secondary current AH accumulation
 Bits 15:18  Secondary current swell event history

 V2 events
 Bit 19 Secondary voltage zero-crossing
 Bit 20 Secondary voltage sigma-delta bitstream stuck
 Bit 21 Secondary voltage period error (out of range)
 Bit 22:25 Secondary voltage swell event history
 Bit 26:29 Secondary voltage sag event history
 
 Bit 30 Reserved
 Bit 31 Reserved


 */

/**
 * @brief       Get the specified live event for a channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   METRO_Live_Event_Type_t type ( Live events are generated by DSP)
 * @param[out]  None
 * @retval     u32 : Live event Register  if in_Metro_Live_Event_requested == ALL_LIVE_EVENTS
 *             otherwise : 0 if Live event is NOT occured,  1 if occured
 */
uint32_t Metro_Read_Live_Event_from_Channel(METRO_Channel_t in_Metro_Channel,
		METRO_Live_Event_Type_t in_Metro_Live_Event_requested) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  return the Live event status requested for the selected channel according to device*/
	return (Metro_HAL_Read_Live_Event_from_Channel(device, int_Channel,
			in_Metro_Live_Event_requested));
}

/******************************************************************************/
/*********             STATUS                                  *******/
/******************************************************************************/

/***********************************/
/* STPM3X  STATUS REGISTER DSP_SR1 */
/************************************/
/* 
 PH1+PH2 status
 Bit 0 Sign change total active power
 Bit 1 Sign change total reactive power
 Bit 2 Overflow total active energy
 Bit 3 Overflow total reactive energy

 PH2 IRQ status
 Bit 4 Sign change secondary channel active power
 Bit 5 Sign change secondary channel active fundamental power
 Bit 6 Sign change secondary channel reactive power
 Bit 7 Sign change secondary channel apparent power
 Bit 8 Overflow secondary channel active energy
 Bit 9 Overflow secondary channel active fundamental energy
 Bit 10 Overflow secondary channel reactive energy
 Bit 11 Overflow secondary channel apparent energy

 PH1 IRQ status
 Bit 12 Sign change primary channel active power
 Bit 13 Sign change primary channel active fundamental power
 Bit 14 Sign change primary channel reactive power
 Bit 15 Sign change primary channel apparent power
 Bit 16 Overflow primary channel active energy
 Bit 17 Overflow primary channel active fundamental energy
 Bit 18 Overflow primary channel reactive energy
 Bit 19 Overflow primary channel apparent energy

 C2 IRQ status
 Bit 20 Secondary current sigma-delta bitstream stuck
 Bit 21 AH1 - accumulation of secondary channel current
 Bit 22 Secondary current swell start
 Bit 23 Secondary current swell end

 V2 IRQ status
 Bit 24 Secondary voltage sigma-delta bitstream stuck
 Bit 25 Secondary voltage period error
 Bit 26 Secondary voltage sag start
 Bit 27 Secondary voltage sag end
 Bit 28 Secondary voltage swell start
 Bit 29 Secondary voltage swell end
 
 Tamper
 Bit 30  Tamper on secondary
 Bit 31  Tamper or wrong connection
 */

/***********************************/
/* STPM3X  STATUS REGISTER DSP_SR2 */
/************************************/

/*

 PH1+PH2 status
 Bit 0 Sign change total active power 0
 Bit 1 Sign change total reactive power 0
 Bit 2 Overflow total active energy 0
 Bit 3 Overflow total reactive energy 0

 PH2 IRQ status
 Bit 4 Sign change secondary channel active power 0
 Bit 5 Sign change secondary channel active fundamental power 0
 Bit 6 Sign change secondary channel reactive power 0
 Bit 7 Sign change secondary channel apparent power 0
 Bit 8 Overflow secondary channel active energy 0
 Bit 9 Overflow secondary channel active fundamental energy 0
 Bit 10 Overflow secondary channel reactive energy 0
 Bit 11 Overflow secondary channel apparent energy 0

 PH1 IRQ status
 Bit 12 Sign change primary channel active power 0
 Bit 13 Sign change primary channel active fundamental power 0
 Bit 14 Sign change primary channel reactive power 0
 Bit 15 Sign change primary channel apparent power 0
 Bit 16 Overflow primary channel active energy 0
 Bit 17 Overflow primary channel active fundamental energy 0
 Bit 18 Overflow primary channel reactive energy 0
 Bit 19 Overflow primary channel apparent energy 0

 C1 IRQ status
 Bit 20 Secondary current sigma-delta bitstream stuck 0
 Bit 21 AH1 - accumulation of secondary channel current 0
 Bit 22 Secondary current swell start 0
 Bit 23 Secondary current swell end 0

 V1 IRQ status
 Bit 24 Secondary voltage sigma-delta bitstream stuck 0
 Bit 25 Secondary voltage period error 0
 Bit 26 Secondary voltage sag start 0
 Bit 27 Secondary voltage sag end 0
 Bit 28 Secondary voltage swell start 0
 Bit 29 Secondary voltage swell end 0

 Tamper
 Bit 30 Tamper on secondary 0
 Bit 31 Tamper or wrong connection 0

 */

/**
 * @brief       Get the specified IRQ status for a channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   METRO_Status_Type_t type ( Status registers are generated by IRQ  )
 * @param[out]  None
 * @retval     u32 : Full Status  Register  if in_Metro_Status_requested == ALL_STATUS
 *             otherwise : 0 if Status requested (IRQ) is NOT occured,  1 if Status requested IRQ occured
 */
uint32_t Metro_Read_Status_from_Channel(METRO_Channel_t in_Metro_Channel,
		METRO_Status_Type_t in_Metro_Status_requested) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  return the Status requested for the selected channel according to device*/
	return (Metro_HAL_Read_Status_from_Channel(device, int_Channel,
			in_Metro_Status_requested));
}

/**
 * @brief       Clear the specified IRQ status for a channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   METRO_Status_Type_t type ( Status registers are generated by IRQ  )
 * @param[out]  None
 * @retval      None
 */
void Metro_Clear_Status_for_Channel(METRO_Channel_t in_Metro_Channel,
		METRO_Status_Type_t in_Metro_Status_requested) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  Clear the Status requested for the selected channel according to device*/
	Metro_HAL_Clear_Status_for_Channel(device, int_Channel,
			in_Metro_Status_requested);
}

/*************************************************************************************************/
/* IRQ and Status for STPM only ( IRQ and status for UART/SPI link between Host and STPM chips) */
/***************************************************************************************************/

/*

 Register US_REG3  Bit position Description

 Status bits
 ----------------
 30 SPI RX overrun 0 RW
 29 SPI TX underrun 0 RW
 28 SPI CRC error 0 RW
 27 UART/SPI write address error 0 RW
 26 UART/SPI read address error 0 RW
 25 SPI TX empty 0 RO
 24 SPI RX full 0 RO
 22 UART TX overrun 0 RW
 21 UART RX overrun 0 RW
 20 UART noise error 0 RW
 19 UART frame error 0 RW
 18 UART timeout error 0 RW
 17 UART CRC error 0 RW
 16 UART break 0 RW
 IRQ CR Bits
 -----------
 14 mask for SPI RX overrun error status bit 0 RW
 13 mask for SPI TX underrun error status bit 0 RW
 12 mask for SPI CRC error status bit 0 RW
 11 mask for write address error status bit 0 RW
 10 mask for read address error status bit 0 RW
 6 mask for UART TX overrun 0 RW
 5 mask for UART RX overrun 0 RW
 4 mask for UART noise error 0 RW
 3 mask for UART frame error 0 RW
 2 mask for UART timeout error 0 RW
 1 mask for UART CRC error 0 RW

 */

/**
 * @brief       Set IRQ Mask for a Ext device
 * @param[in]   METRO_NB_Device_t in_Metro_Device_Id : EXT1 to EXT4 ( HOST forbidden )
 * @param[in]   in_Metro_IT_Mask :  Mask of interruption according to the device
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_IRQ_Mask_for_STPM_device(METRO_NB_Device_t in_Metro_Device_Id,
		uint16_t in_Metro_IT_Mask) {
	/* Set the IRQ Mask for the selected ext device */
	Metro_HAL_Set_IRQ_Mask_for_STPM_device(in_Metro_Device_Id,
			in_Metro_IT_Mask);
}

/**
 * @brief       Get IRQ Mask for a Ext Device
 * @param[in]   METRO_NB_Device_t in_Metro_Device_Id : EXT1 to EXT4 ( HOST forbidden )
 * @param[out]  None
 * @retval     u16 Metro_IT_Mask
 */
uint16_t Metro_Get_IRQ_Mask_from_STPM_device(
		METRO_NB_Device_t in_Metro_Device_Id) {

	/*  return the IRQ Mask for the selected EXT device*/
	return (Metro_HAL_Get_IRQ_Mask_from_STPM_device(in_Metro_Device_Id));
}

/**
 * @brief       Get the specified IRQ Link  status for STPM device
 * @param[in]   METRO_NB_Device_t in_Metro_Device_Id : EXT1 to EXT4 ( HOST forbidden )
 * @param[in]   METRO_STPM_LINK_IRQ_Status_Type_t type ( Status registers are generated by IRQ  )
 * @param[out]  None
 * @retval     u16 : Full Status  Register  if in_Metro_Status_requested == ALL_STATUS
 *             otherwise : 0 if Status requested (IRQ) is NOT occured,  1 if Status requested IRQ occured
 */
uint16_t Metro_Read_Status_from_STPM_device(
		METRO_NB_Device_t in_Metro_Device_Id,
		METRO_STPM_LINK_IRQ_Status_Type_t in_Metro_Status_requested) {
	/*  return the IRQStatus requested for the ext STPM device*/
	return (Metro_HAL_Read_Status_from_STPM_device(in_Metro_Device_Id,
			in_Metro_Status_requested));
}

/**
 * @brief       Clear the specified IRQ status for a channel
 * @param[in]   METRO_NB_Device_t in_Metro_Device_Id : EXT1 to EXT4 ( HOST forbidden )
 * @param[in]   METRO_STPM_LINK_IRQ_Status_Type_t type ( Status registers are generated by IRQ  )
 * @param[out]  None
 * @retval      None
 */
void Metro_Clear_Status_for_STPM_device(METRO_NB_Device_t in_Metro_Device_Id,
		METRO_STPM_LINK_IRQ_Status_Type_t in_Metro_Status_requested) {
	/*  Clear the IRQ Status requested for the selected EXT STPM  device*/
	Metro_HAL_Clear_Status_for_STPM_device(in_Metro_Device_Id,
			in_Metro_Status_requested);
}

/***********************************/
/*        SAG and SWELL            */
/************************************/

/*
 SAG is the detection of too low voltage. This detection works absolutely.
 -----
 To define the voltage threshold level for SAG, 10 bits register SagValue[9:0] are used.
 To define the time when SAG is set, if voltage signal doesn't reach value SagValue, 12 bits register TimeValue[13:0] are used.
 4 bits shift register SAG[3:0], detection of too low voltage after time the TimeValue.
 15 bits register SagTime[14:0], accumulation of time, when bit SAG[0] is set. After overflow in SagTime the event is notes in SAG.

 TimeofSag = TimeSag[14:0]/FClk,   Delta_time=8绁�

 Overflow in the register SagTime after time:
 --------------------------------------------
 2^15/FClk=262 ms

 With bit Clear_SS (0 --> 1) the value in registers SagTime and SAG are clear to 0.

 SagValue[9:0]=鐩玱ltageForSag锟�2^10

 Max VoltageForSag:
 -----------------
 VoltageForSag=(鐩╝gValue[9:0])/2^10 =锟�0.999023,   Delta_value=锟�976锟�10^(-6)

 TimeValue[13:0]=TimeForSag绋lk=10 ms锟�125kHz=1250= 14' h04E2

 Min TimeValue should be 14'h0001, never 14'h0000!!!

 Max TimeForSag:
 --------------
 MaxTimeForSag=TimeValue[13:0]/FClk=2^14/125 kHz=131 ms,      delta_time=8绁�



 SWELL is the detection of too high voltage from the voltage signal (Voltage SWELL) and detection of too high current (Current SWELL) for both current channels. The detection works absolutely.
 ------
 To define the voltage level for SWELL, 10 bits register SwellValue[9:0] are used.
 4 bits shift register SWELL[3:0], detection of too high voltage.
 15 bits register SwellTime[14:0], accumulation of time, when bit SWELL[0] is set. After overflow in SwellTime the event is notes in SWELL.

 TimeofSwell=TimeSwell[14:0]/FClk,   lsb_time=8绁�

 Overflow in the register SwellTime after time:
 2^15/FClk=262 ms

 With bit Clear_SS (0 --> 1) the value in registers SagTime, SwellTime, SAG and SWELL are clear to 0.*/

/**
 * @brief       Set the thresholds and the time to detect the sag event for each channel ( Voltage )
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_3 ( CHANNEL 4 is for Current Tamper only, not used here)
 * @param[in]   u16 in_Metro_SAG_Threshold (Reg value under 10 bits)
 * @param[in]   u16 in_Metro_SAG_detect_time (Reg value under 14 bits)
 *              (Setting threshold and time to zero disables the sag event detection)
 * @param[out]  None
 * @retval      None
 */

void Metro_Set_SAG_Config(METRO_Channel_t in_Metro_Channel,
		uint32_t in_Metro_SAG_Threshold, uint32_t in_Metro_SAG_detect_time) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  Set the thresholds and the time to detect the sag event for the channel ( Voltage )*/
	Metro_HAL_Set_SAG_Config(device, int_Channel, in_Metro_SAG_Threshold,
			in_Metro_SAG_detect_time);
}
/**
 * @brief       Set voltage swell threshold for each voltage channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_3 ( CHANNEL 4 is for Current Tamper only, not used here)
 * @param[in]   u16 in_Metro_V_SWELL_Threshold ( Threshold value under 10 bits)
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_V_SWELL_Config(METRO_Channel_t in_Metro_Channel,
		uint16_t in_Metro_V_SWELL_Threshold) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  Set voltage swell threshold for the voltage channel according to device*/
	Metro_HAL_Set_V_SWELL_Config(device, int_Channel,
			in_Metro_V_SWELL_Threshold);
}

/**
 * @brief       Set Current swell threshold for each current channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[in]   u16 in_Metro_C_SWELL_Threshold ( Threshold value under 10 bits)
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_C_SWELL_Config(METRO_Channel_t in_Metro_Channel,
		uint16_t in_Metro_C_SWELL_Threshold) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set Current swell threshold for current channel according to device*/
	Metro_HAL_Set_C_SWELL_Config(device, int_Channel,
			in_Metro_C_SWELL_Threshold);
}

/**
 * @brief       Get the thresholds and the time to detect the sag event for each channel ( Voltage )
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_3 ( CHANNEL 4 is for Current Tamper only, not used here)
 * @param[out]   u32 * out_p_Metro_SAG_Threshold  (Reg value under 10 bits)
 * @param[in]     u32 * out_p_Metro_SAG_detect_time  (Reg value under 14 bits)
 * @retval      None
 */

void Metro_Get_SAG_Config(METRO_Channel_t in_Metro_Channel,
		uint32_t * out_p_Metro_SAG_Threshold,
		uint32_t * out_p_Metro_SAG_detect_time) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;
	uint16_t time_Value = 0;
	uint16_t sag_Value = 0;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  Get the thresholds and the time to detect the sag event for the channel ( Voltage )*/
	Metro_HAL_Get_SAG_Config(device, int_Channel, &sag_Value, &time_Value);

	/* return values */
	*out_p_Metro_SAG_detect_time = (uint16_t) (time_Value);
	*out_p_Metro_SAG_Threshold = (uint16_t) (sag_Value);
}
/**
 * @brief       Get voltage swell threshold for each voltage channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_3 ( CHANNEL 4 is for Current Tamper only, not used here)
 * @param[out]  None
 * @retval      u16 Metro_V_SWELL_Threshold
 */
uint16_t Metro_Get_V_SWELL_Config(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  Get voltage swell threshold for the voltage channel according to device*/
	return Metro_HAL_Get_V_SWELL_Config(device, int_Channel);
}

/**
 * @brief       Get Current swell threshold for each current channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_4
 * @param[out]  None
 * @retval      u16 Metro_C_SWELL_Threshold
 */
uint16_t Metro_Get_C_SWELL_Config(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Get Current swell threshold for current channel according to device*/
	return Metro_HAL_Get_C_SWELL_Config(device, int_Channel);
}

/**
 * @brief       Get SAG time counter for each voltage channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_3 ( CHANNEL 4 is for Current Tamper only, not used here)
 * @param[out]  None
 * @retval      u16 Metro_V_SAG_Time
 */

uint16_t Metro_Read_SAG_Time(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  Get SAG time counter for each voltage channel according to device*/
	return Metro_HAL_Read_SAG_Time(device, int_Channel);
}

/**
 * @brief       Get  Swell time counter from Voltage Channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_3 ( CHANNEL 4 is for Current Tamper only, not used here)
 * @param[out]  None
 * @retval      u16 Metro_V_SWELL_Time
 */
//*  */
uint16_t Metro_Read_V_SWELL_Time(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Get  swell time  counter for  channel according to device*/
	return Metro_HAL_Read_V_SWELL_Time(device, int_Channel);
}

/**
 * @brief       Get Swell time counter for each Current channel
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_3 ( CHANNEL 4 is for Current Tamper only, not used here)
 * @param[out]  None
 * @retval      u16 Metro_C_SWELL_Time
 */
uint16_t Metro_Read_C_SWELL_Time(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Get swell time counter for current channel according to device*/
	return Metro_HAL_Read_C_SWELL_Time(device, int_Channel);
}

/**
 * @brief       Set timeout value for clearing sag and swell events bits after setting events clear bits for each voltage channel.
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_3 ( CHANNEL 4 is for Current Tamper only, not used here)
 * @param[in]   u8 in_Metro_Sag_and_Swell_Clear_Timeout ( Time out Value under 4 bits)
 * @param[out]  None
 * @retval      None
 */
void Metro_Set_SAG_and_SWELL_Clear_Timeout(METRO_Channel_t in_Metro_Channel,
		uint8_t in_Metro_Sag_and_Swell_Clear_Timeout) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Set Sag and swell Timeout for channel according to device*/
	Metro_HAL_Set_SAG_and_SWELL_Clear_Timeout(device, int_Channel,
			in_Metro_Sag_and_Swell_Clear_Timeout);
}

/**
 * @brief       Get timeout value for clearing sag and swell events bits after setting events clear bits for each voltage channel.
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_3 ( CHANNEL 4 is for Current Tamper only, not used here)
 * @param[out]  None
 * @retval      u8 Time out (Value under 4 bits)
 */
uint8_t Metro_Get_SAG_and_SWELL_Clear_Timeout(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/* Get Clear Sag and Swell time out channel according to device*/
	return Metro_HAL_Get_SAG_and_SWELL_Clear_Timeout(device, int_Channel);
}

/**
 * @brief       Clear the sag and swell event bits for each power type and calculation path (Vx-Cx couple).
 * @param[in]   in_Metro_Channel (Channel ID ), CHANNEL_1 to CHANNEL_3 ( CHANNEL 4 is for Current Tamper only, not used here)
 * @param[out]  None
 * @retval      None
 */
void Metro_Clear_SAG_and_SWELL_Event(METRO_Channel_t in_Metro_Channel) {
	METRO_internal_Channel_t int_Channel;
	METRO_NB_Device_t device;

	/* Get Device id from Channel */
	device = Metro_Get_device_from_Channel(in_Metro_Channel);

	/* Get internal channel to get if the channel requested is the one or the two of the device or tamper*/
	int_Channel = Metro_Get_Internal_channel(in_Metro_Channel, device);

	/*  Clear Sag and Swell events to  channel according to device*/
	Metro_HAL_Clear_SAG_and_SWELL_Events(device, int_Channel);
}

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
