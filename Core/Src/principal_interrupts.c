/*
 * principal_interrupts.c
 *
 *  Created on: 29 de dez de 2021
 *      Author: Rodolfo
 */

#include "principal.h"

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_Rx_Header, CAN_Rx_Data) == HAL_OK)
	{
		Verify_CAN[1] = 1;

		if(CAN_Rx_Header.IDE == CAN_ID_STD)
		{
			Principal_Datalogger_Save_Buffer(CAN_Rx_Header.StdId, CAN_Rx_Header.DLC, CAN_Rx_Data, &File_Struct);
		}

		else if(((CAN_Rx_Header.ExtId & 0x1FFFF000) == 0x1E35C000) && (CAN_Rx_Header.IDE == CAN_ID_EXT))
		{
			PDM_CAN_Process_Data(CAN_Rx_Header.ExtId, CAN_Rx_Header.DLC, CAN_Rx_Data, &PDM_Readings);
		}

		else
		{
			FT_CAN_ReceiveData(CAN_Rx_Header.ExtId, CAN_Rx_Header.DLC, CAN_Rx_Data, &ECU_Data);
		}
	}

	else
		Verify_CAN[1] = 0;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == EXTI0_Pin)
	{

	}

	if(GPIO_Pin == EXTI1_Pin)
	{

	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{
		for(uint8_t i = 0; i < NBR_OF_MSGS; i++)
		{
			Acc_CAN[i]++;
		}

		for(uint8_t i = 0; i < (NBR_OF_MSGS + NBR_OF_LOCAL_MSGS); i++)
		{
			Acc_Msg[i]++;
		}
	}
}
