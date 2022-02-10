/*
 * principal_interrupts.c
 *
 *  Created on: 29 de dez de 2021
 *      Author: Rodolfo
 */

#include "principal.h"

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK)
	{
		verifyCAN |= 2;

		if((rxHeader.IDE == CAN_ID_STD) && ((rxHeader.StdId & CAN_DAQ_MASK) == CAN_DAQ_FILTER) && (flagDatalogger == DL_SAVE))
			Principal_Datalogger_Save_Buffer(rxHeader.StdId, rxHeader.DLC, rxData, &fileStruct);

		else if(((rxHeader.ExtId & 0x1FFFF000) == 0x1E35C000) && (rxHeader.IDE == CAN_ID_EXT))
			PDM_CAN_Process_Data(rxHeader.ExtId, rxHeader.DLC, rxData, &pdmReadings);

		else if((rxHeader.ExtId == CONFIG_ID) && (rxHeader.IDE == CAN_ID_EXT))
			Principal_Receive_Config(&hi2c1, rxData, rxHeader.DLC);

		else
			FT_CAN_ReceiveData(rxHeader.ExtId, rxHeader.DLC, rxData, &ecuData);
	}

	else
		verifyCAN &= 1;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == EXTI0_Pin)
	{
		if((inputConfig & 0x01) == 0)
			Principal_Beacon_Detect();

		if((inputConfig & 0x02) == 0)
			Principal_Datalogger_Button(&dirStruct, &fileStruct);
	}

	if(GPIO_Pin == EXTI1_Pin)
	{
		if((inputConfig & 0x01) == 1)
			Principal_Beacon_Detect();

		if((inputConfig & 0x02) == 2)
			Principal_Datalogger_Button(&dirStruct, &fileStruct);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM7)
	{
		accDatalogger[0]++;
		if(accDatalogger[1] > 0) accDatalogger[1]--;

		accCAN[ANALOG_1_4]++;
		accCAN[ANALOG_5_8]++;
		accCAN[ANALOG_9_12]++;
		accCAN[RTC_MSG]++;
		accCAN[VERIFY_MSG]++;
		accCAN[BEACON_MSG]++;

		accMsg[ANALOG_1_4]++;
		accMsg[ANALOG_5_8]++;
		accMsg[ANALOG_9_12]++;
		accMsg[RTC_MSG]++;
		accMsg[VERIFY_MSG]++;
		accMsg[BEACON_MSG]++;
		accMsg[ECU_SAVE]++;
		accMsg[PDM_SAVE]++;

		accLap++;
	}
}
