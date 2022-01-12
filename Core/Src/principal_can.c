/*
 * principal_can.c
 *
 *  Created on: Dec 30, 2021
 *      Author: Rodolfo
 */

#include "principal.h"

static void Msg_0(CAN_HandleTypeDef* hcan)
{
	CAN_Tx_Header.StdId = FIRST_ID;
	CAN_Tx_Header.DLC = 8;
	CAN_Tx_Header.IDE = CAN_ID_STD;
	CAN_Tx_Header.RTR = CAN_RTR_DATA;
	CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	CAN_Tx_Data[0] = ADC_Buffer[0] >> 8;
	CAN_Tx_Data[1] = ADC_Buffer[0] & 0xff;
	CAN_Tx_Data[2] = ADC_Buffer[1] >> 8;
	CAN_Tx_Data[3] = ADC_Buffer[1] & 0xff;
	CAN_Tx_Data[4] = ADC_Buffer[2] >> 8;
	CAN_Tx_Data[5] = ADC_Buffer[2] & 0xff;
	CAN_Tx_Data[6] = ADC_Buffer[3] >> 8;
	CAN_Tx_Data[7] = ADC_Buffer[3] & 0xff;

	if(Flag_Datalogger == 1)
		Principal_Datalogger_Save_Buffer(CAN_Tx_Header.StdId, CAN_Tx_Header.DLC, CAN_Tx_Data, &File_Struct);

	if(Acc_CAN[0] >= Per_CAN[0])
	{
		Acc_CAN[0] = 0;
		if(HAL_CAN_AddTxMessage(hcan, &CAN_Tx_Header, CAN_Tx_Data, &pTxMailbox) == HAL_OK)
			Verify_CAN[0] = 1;

		//Wait Transmission finish
		for(uint8_t i = 0; HAL_CAN_GetTxMailboxesFreeLevel(hcan) != 3 && i < 3; i++);
	}
}

static void Msg_1(CAN_HandleTypeDef* hcan)
{
	CAN_Tx_Header.StdId = FIRST_ID + 1;
	CAN_Tx_Header.DLC = 8;
	CAN_Tx_Header.IDE = CAN_ID_STD;
	CAN_Tx_Header.RTR = CAN_RTR_DATA;
	CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	CAN_Tx_Data[0] = ADC_Buffer[4] >> 8;
	CAN_Tx_Data[1] = ADC_Buffer[4] & 0xff;
	CAN_Tx_Data[2] = ADC_Buffer[5] >> 8;
	CAN_Tx_Data[3] = ADC_Buffer[5] & 0xff;
	CAN_Tx_Data[4] = ADC_Buffer[6] >> 8;
	CAN_Tx_Data[5] = ADC_Buffer[6] & 0xff;
	CAN_Tx_Data[6] = ADC_Buffer[7] >> 8;
	CAN_Tx_Data[7] = ADC_Buffer[7] & 0xff;

	if(Flag_Datalogger == 1)
		Principal_Datalogger_Save_Buffer(CAN_Tx_Header.StdId, CAN_Tx_Header.DLC, CAN_Tx_Data, &File_Struct);

	if(Acc_CAN[1] >= Per_CAN[1])
	{
		Acc_CAN[1] = 0;
		if(HAL_CAN_AddTxMessage(hcan, &CAN_Tx_Header, CAN_Tx_Data, &pTxMailbox) == HAL_OK)
			Verify_CAN[0] = 1;

		//Wait Transmission finish
		for(uint8_t i = 0; HAL_CAN_GetTxMailboxesFreeLevel(hcan) != 3 && i < 3; i++);
	}
}

static void Msg_2(CAN_HandleTypeDef* hcan)
{
	CAN_Tx_Header.StdId = FIRST_ID + 2;
	CAN_Tx_Header.DLC = 8;
	CAN_Tx_Header.IDE = CAN_ID_STD;
	CAN_Tx_Header.RTR = CAN_RTR_DATA;
	CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	CAN_Tx_Data[0] = ADC_Buffer[8] >> 8;
	CAN_Tx_Data[1] = ADC_Buffer[8] & 0xff;
	CAN_Tx_Data[2] = ADC_Buffer[9] >> 8;
	CAN_Tx_Data[3] = ADC_Buffer[9] & 0xff;
	CAN_Tx_Data[4] = ADC_Buffer[10] >> 8;
	CAN_Tx_Data[5] = ADC_Buffer[10] & 0xff;
	CAN_Tx_Data[6] = ADC_Buffer[11] >> 8;
	CAN_Tx_Data[7] = ADC_Buffer[11] & 0xff;

	if(Flag_Datalogger == 1)
		Principal_Datalogger_Save_Buffer(CAN_Tx_Header.StdId, CAN_Tx_Header.DLC, CAN_Tx_Data, &File_Struct);

	if(Acc_CAN[2] >= Per_CAN[2])
	{
		Acc_CAN[2] = 0;
		if(HAL_CAN_AddTxMessage(hcan, &CAN_Tx_Header, CAN_Tx_Data, &pTxMailbox) == HAL_OK)
			Verify_CAN[0] = 1;
		//Wait Transmission finish
		for(uint8_t i = 0; HAL_CAN_GetTxMailboxesFreeLevel(hcan) != 3 && i < 3; i++);
	}
}

static void Msg_Verify(CAN_HandleTypeDef* hcan)
{
	CAN_Tx_Header.StdId = FIRST_ID + 4;
	CAN_Tx_Header.DLC = 8;
	CAN_Tx_Header.IDE = CAN_ID_STD;
	CAN_Tx_Header.RTR = CAN_RTR_DATA;
	CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	if(Flag_Datalogger == 1)
		Principal_Datalogger_Save_Buffer(CAN_Tx_Header.StdId, CAN_Tx_Header.DLC, CAN_Tx_Data, &File_Struct);

	if(HAL_CAN_AddTxMessage(hcan, &CAN_Tx_Header, CAN_Tx_Data, &pTxMailbox) == HAL_OK)
		Verify_CAN[0] = 1;

	//Wait Transmission finish
	for(uint8_t i = 0; HAL_CAN_GetTxMailboxesFreeLevel(hcan) != 3 && i < 3; i++);
}

static void Msg_PDM()
{
	uint8_t buffer[8];
}

static void Msg_ECU()
{
	uint8_t buffer[8];
}

void Principal_Transmit_Msg(CAN_HandleTypeDef* hcan, uint8_t msg_number)
{
	switch(msg_number)
	{
		case 0:
			Msg_0(hcan);
			break;

		case 1:
			Msg_1(hcan);
			break;

		case 2:
			Msg_2(hcan);
			break;

		case 3:
			Msg_Verify(hcan);
			break;

		case 4:
			Msg_PDM();
			break;

		case 5:
			Msg_ECU();
			break;

		default:
			return;
	}
}
