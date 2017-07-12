/*
 * readsensor.c
 *
 *  Created on: Feb 17, 2017
 *      Author: Fei.Guan
 */

#include "ch.h"
/* Sensor task includes */
#include "readstpm.h"
#include "metroTask.h"
#include "senddata.h"
#include "log.h"


/*
 * read stpm thread to read the sensor data.
 */
static WORKING_AREA(readstpmThread, 2048);
void readsensor(void *p);

char buf[1024];
uint32_t tick = 0;
UART_HandleTypeDef huart6, huart1, huart2, huart3;

BinarySemaphore readsensorSem; //binary semaphore

static VirtualTimer readsensor_vt;

void MX_USART_UART_Init(void);

/*
 * read sensor timer callback.
 */
static void readsensor_cb(void *arg) {
  METRO_Latch_Measures();
  chBSemSignal(&readsensorSem);
  chSysLockFromIsr();
  chVTSetI(&readsensor_vt, MS2ST(3000), readsensor_cb, NULL);
  chSysUnlockFromIsr();
}

void readsensor_init(BaseSequentialStream *chp) {
	MX_USART_UART_Init();

	METRO_Init();


	log_init(NULL, 0, NULL);

	chBSemInit(&readsensorSem, 0); //initialize the read sensor semaphore

	chSemInit(&sendingbufsem, 0); //initialize the sending buffer semaphore in senddata.c

	/*
	 * Creates the send data thread.
	 */
	chThdCreateStatic(wa_sendata_client, sizeof(wa_sendata_client),
			LOWPRIO, senddata_client, NULL);

	/* LED timer initialization.*/
	chVTSet(&readsensor_vt, MS2ST(1000), readsensor_cb, NULL);

	/*
	 * Creates the read sensor thread.
	 */
	chThdCreateStatic(readstpmThread, sizeof(readstpmThread), LOWPRIO, readsensor,
		 chp);
}

void readsensor(void *p) {

	//the parameter is the SD3

	BaseSequentialStream *chp = (BaseSequentialStream *)p;

	unsigned char channels_to_read = 0b00111111; // read channels  1 2 5 6

	uint32_t RMS_V = 0;
	uint32_t RMS_I = 0;
	int32_t MOM_V = 0;
	int32_t MOM_I = 0;
	int32_t FUND_V = 0;
	int32_t FUND_I = 0;
	int32_t Period = 0;
	int32_t Phase = 0;
	int32_t SAG = 0;
	int32_t VSWELL = 0;
	int32_t CSWELL = 0;
	int32_t POW_ACT, POW_REACT, POW_APP;
	int32_t EN_ACT, EN_REACT, EN_APP;
	int32_t AHACC;
	int32_t Currentgain;

//	int32_t Limit1;
//	int32_t Limit2;
	do {
		chBSemWait(&readsensorSem);
		tick++;
		/**************** Measure application ************************/

		metroData.metroInactiveTime = 0;
		METRO_Get_Measures();
		METRO_UpdateData();

		//read data

		unsigned char bit= 0b00000001;
		int len = 0;
	    Metro_Set_C_Calibration(1,2048);
	    Metro_Set_C_Calibration(2,3800);
	    Metro_Set_C_Calibration(5,3800);
		for(int i=1; i<=8; i++){
			if((bit<<(i-1))&channels_to_read){
				Metro_Read_RMS(i, &RMS_V, &RMS_I, 1);
				MOM_V = Metro_Read_Momentary_Voltage(i, 1);
				MOM_I = Metro_Read_Momentary_Current(i, 1);
				FUND_V = Metro_Read_Momentary_Voltage(i, 2);
				FUND_I = Metro_Read_Momentary_Current(i, 2);
				Phase = Metro_Read_PHI(i);
				Period = Metro_Read_Period(i);
				SAG = Metro_Read_SAG_Time(i);
				VSWELL = Metro_Read_V_SWELL_Time(i);
				CSWELL = Metro_Read_C_SWELL_Time(i);
				EN_ACT = Metro_Read_energy(i, E_W_ACTIVE);
				EN_REACT = Metro_Read_energy(i, E_REACTIVE);
				EN_APP = Metro_Read_energy(i, E_APPARENT);
				POW_ACT = Metro_Read_Power(i, W_ACTIVE);
				POW_REACT = Metro_Read_Power(i, REACTIVE);
				POW_APP = Metro_Read_Power(i, APPARENT_RMS);
				AHACC = Metro_Read_AH_Acc(i);
				Currentgain = Metro_Get_C_Calibration(i);

				len += sprintf(buf+len,
						"bt%d,c%d,v%d,i%d,e%d,p%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%dz\n",
						tick, i, RMS_V, RMS_I, EN_ACT, POW_ACT, MOM_V, MOM_I, FUND_V, FUND_I, Phase,
						Period, SAG, VSWELL, CSWELL, EN_REACT, EN_APP,
						POW_REACT, POW_APP, AHACC);
			}
		}

		_log_data(NULL, len, buf);

		chprintf(chp, buf);

		//senddata(len, buf); //send data to the network

	} while (true);
}

void MX_USART_UART_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
//	__GPIOA_CLK_ENABLE();
//	__GPIOB_CLK_ENABLE();
//	__GPIOC_CLK_ENABLE();
//	__GPIOE_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, EN_B_Pin|SCS_B_Pin|SYN_B_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, EN_A_Pin|SCS_A_Pin|SYN_A_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOE, EN_C_Pin|SCS_C_Pin|SYN_C_Pin|
			EN_D_Pin|SCS_D_Pin|SYN_D_Pin, GPIO_PIN_RESET);

//	/*Configure GPIO pin Output Level */
//	HAL_GPIO_WritePin(GPIOD, LED5_Pin|LED6_Pin, GPIO_PIN_SET);


	/*Configure GPIO pins : EN_B_Pin (PA8)|SCS_B_Pin (PA9)|SYN_B_Pin (PA10) */
	GPIO_InitStruct.Pin = EN_B_Pin|SCS_B_Pin|SYN_B_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : EN_A_Pin (PB0)|SCS_A_Pin (PB1)|SYN_A_Pin (PB2) */
	GPIO_InitStruct.Pin = EN_A_Pin|SCS_A_Pin|SYN_A_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : EN_C_Pin (PE3)|SCS_C_Pin (PE4)|SYN_C_Pin (PE5)|EN_D_Pin (PE13)|SCS_D_Pin (PE14)|SYN_D_Pin  (PE15)*/
	GPIO_InitStruct.Pin = EN_C_Pin|SCS_C_Pin|SYN_C_Pin|EN_D_Pin|SCS_D_Pin|SYN_D_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

//	/*Configure GPIO pins : LED5_Pin LED6_Pin */
//	GPIO_InitStruct.Pin = LED5_Pin|LED6_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
//	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	huart6.Instance = USART6;
	huart6.Init.BaudRate = 9600;
	huart6.Init.WordLength = UART_WORDLENGTH_8B;
	huart6.Init.StopBits = UART_STOPBITS_1;
	huart6.Init.Parity = UART_PARITY_NONE;
	huart6.Init.Mode = UART_MODE_TX_RX;
	huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart6.Init.OverSampling = UART_OVERSAMPLING_8;
	HAL_UART_Init(&huart6);

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 9600;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_8;
	HAL_UART_Init(&huart1);

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 9600;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_8;
	HAL_UART_Init(&huart2);

//	huart3.Instance = USART3;
//	huart3.Init.BaudRate = 9600;
//	huart3.Init.WordLength = UART_WORDLENGTH_8B;
//	huart3.Init.StopBits = UART_STOPBITS_1;
//	huart3.Init.Parity = UART_PARITY_NONE;
//	huart3.Init.Mode = UART_MODE_TX_RX;
//	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//	huart3.Init.OverSampling = UART_OVERSAMPLING_8;
//	HAL_UART_Init(&huart3);
}
