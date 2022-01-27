/*
 * principal_interrupts.c
 *
 *  Created on: 29 de dez de 2021
 *      Author: Rodolfo
 */

#include "principal.h"

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
	{
		Verify_CAN |= 2;

		if((RxHeader.IDE == CAN_ID_STD) && (Flag_Datalogger == DL_Save))
			Principal_Datalogger_Save_Buffer(RxHeader.StdId, RxHeader.DLC, RxData, &File_Struct);

		else if(((RxHeader.ExtId & 0x1FFFF000) == 0x1E35C000) && (RxHeader.IDE == CAN_ID_EXT))
			PDM_CAN_Process_Data(RxHeader.ExtId, RxHeader.DLC, RxData, &PDM_Readings);

		else if((RxHeader.ExtId == CONFIG_ID) && (RxHeader.IDE == CAN_ID_EXT))
			Principal_Receive_Config(&hi2c1, RxData);

		else
			FT_CAN_ReceiveData(RxHeader.ExtId, RxHeader.DLC, RxData, &ECU_Data);
	}

	else
		Verify_CAN &= 1;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == EXTI0_Pin)
	{
		if((Input_Config & 0x01) == 0)
			Principal_Beacon_Detect();

		if((Input_Config & 0x02) == 0)
			Principal_Datalogger_Button(&Date, &Time, Dir_String, Log_String, &Dir_Struct, &File_Struct);
	}

	if(GPIO_Pin == EXTI1_Pin)
	{
		if((Input_Config & 0x01) == 1)
			Principal_Beacon_Detect();

		if((Input_Config & 0x02) == 2)
			Principal_Datalogger_Button(&Date, &Time, Dir_String, Log_String, &Dir_Struct, &File_Struct);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM7)
	{
		Acc_Datalogger[0]++;
		if(Acc_Datalogger[1] > 0) Acc_Datalogger[1]--;

		Acc_CAN[Analog_1_4]++;
		Acc_CAN[Analog_5_8]++;
		Acc_CAN[Analog_9_12]++;
		Acc_CAN[RTC_Msg]++;

		Acc_Msg[Analog_1_4]++;
		Acc_Msg[Analog_5_8]++;
		Acc_Msg[Analog_9_12]++;
		Acc_Msg[RTC_Msg]++;
		Acc_Msg[Verify_Msg]++;
		Acc_Msg[ECU_Save]++;
		Acc_Msg[PDM_Save]++;
	}
}
