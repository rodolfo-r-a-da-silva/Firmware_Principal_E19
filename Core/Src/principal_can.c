/*
 * principal_can.c
 *
 *  Created on: Dec 30, 2021
 *      Author: Rodolfo
 */

#include "principal.h"

static void Tx_Analog_1_4(CAN_HandleTypeDef* hcan)
{
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
	TxHeader.StdId = FIRST_ID;
	TxHeader.DLC = 8;

	TxData[0] = ADC_Buffer[0] >> 8;
	TxData[1] = ADC_Buffer[0] & 0xff;
	TxData[2] = ADC_Buffer[1] >> 8;
	TxData[3] = ADC_Buffer[1] & 0xff;
	TxData[4] = ADC_Buffer[2] >> 8;
	TxData[5] = ADC_Buffer[2] & 0xff;
	TxData[6] = ADC_Buffer[3] >> 8;
	TxData[7] = ADC_Buffer[3] & 0xff;

	if(Flag_Datalogger == 1)
		Principal_Datalogger_Save_Buffer(TxHeader.StdId, TxHeader.DLC, TxData, &File_Struct);

	if((Acc_CAN[Analog_1_4] >= Per_CAN[Analog_1_4]) && (Per_CAN[Analog_1_4] != 0))
	{
		Acc_CAN[Analog_1_4] -= Per_CAN[Analog_1_4];
		if(HAL_CAN_AddTxMessage(hcan, &TxHeader, TxData, &TxMailbox) == HAL_OK)
			Verify_CAN |= 1;
		else
			Verify_CAN &= 0x02;

		//Wait Transmission finish
		for(uint8_t i = 0; HAL_CAN_GetTxMailboxesFreeLevel(hcan) != 3 && i < 3; i++);
	}
}

static void Tx_Analog_5_8(CAN_HandleTypeDef* hcan)
{
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
	TxHeader.StdId = FIRST_ID + 1;
	TxHeader.DLC = 8;

	TxData[0] = ADC_Buffer[4] >> 8;
	TxData[1] = ADC_Buffer[4] & 0xff;
	TxData[2] = ADC_Buffer[5] >> 8;
	TxData[3] = ADC_Buffer[5] & 0xff;
	TxData[4] = ADC_Buffer[6] >> 8;
	TxData[5] = ADC_Buffer[6] & 0xff;
	TxData[6] = ADC_Buffer[7] >> 8;
	TxData[7] = ADC_Buffer[7] & 0xff;

	if(Flag_Datalogger == 1)
		Principal_Datalogger_Save_Buffer(TxHeader.StdId, TxHeader.DLC, TxData, &File_Struct);

	if((Acc_CAN[Analog_5_8] >= Per_CAN[Analog_5_8]) && (Per_CAN[Analog_5_8] != 0))
	{
		Acc_CAN[Analog_5_8] -= Per_CAN[Analog_5_8];
		if(HAL_CAN_AddTxMessage(hcan, &TxHeader, TxData, &TxMailbox) == HAL_OK)
			Verify_CAN |= 1;
		else
			Verify_CAN &= 0x02;

		//Wait Transmission finish
		for(uint8_t i = 0; HAL_CAN_GetTxMailboxesFreeLevel(hcan) != 3 && i < 3; i++);
	}
}

static void Tx_Analog_9_12(CAN_HandleTypeDef* hcan)
{
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
	TxHeader.StdId = FIRST_ID + 2;
	TxHeader.DLC = 8;

	TxData[0] = ADC_Buffer[8] >> 8;
	TxData[1] = ADC_Buffer[8] & 0xff;
	TxData[2] = ADC_Buffer[9] >> 8;
	TxData[3] = ADC_Buffer[9] & 0xff;
	TxData[4] = ADC_Buffer[10] >> 8;
	TxData[5] = ADC_Buffer[10] & 0xff;
	TxData[6] = ADC_Buffer[11] >> 8;
	TxData[7] = ADC_Buffer[11] & 0xff;

	if(Flag_Datalogger == 1)
		Principal_Datalogger_Save_Buffer(TxHeader.StdId, TxHeader.DLC, TxData, &File_Struct);

	if((Acc_CAN[Analog_9_12] >= Per_CAN[Analog_9_12]) && (Per_CAN[Analog_9_12] != 0))
	{
		Acc_CAN[Analog_9_12] -= Per_CAN[Analog_9_12];
		if(HAL_CAN_AddTxMessage(hcan, &TxHeader, TxData, &TxMailbox) == HAL_OK)
			Verify_CAN |= 1;
		else
			Verify_CAN &= 0x02;

		//Wait Transmission finish
		for(uint8_t i = 0; HAL_CAN_GetTxMailboxesFreeLevel(hcan) != 3 && i < 3; i++);
	}
}

static void Tx_RTC(CAN_HandleTypeDef* hcan)
{
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
	TxHeader.StdId = FIRST_ID + 5;
	TxHeader.DLC = 6;

	TxData[0] = Time.Hours;
	TxData[1] = Time.Minutes;
	TxData[2] = Time.Seconds;
	TxData[3] = Date.Date;
	TxData[4] = Date.Month;
	TxData[5] = Date.Year;

	if(Flag_Datalogger == 1)
		Principal_Datalogger_Save_Buffer(TxHeader.StdId, TxHeader.DLC, TxData, &File_Struct);

	if((Acc_CAN[RTC_Msg] >= Per_CAN[RTC_Msg]) && (Per_CAN[RTC_Msg] != 0))
	{
		Acc_CAN[RTC_Msg] -= Per_CAN[RTC_Msg];
		if(HAL_CAN_AddTxMessage(hcan, &TxHeader, TxData, &TxMailbox) == HAL_OK)
			Verify_CAN |= 1;
		else
			Verify_CAN &= 0x02;

		//Wait Transmission finish
		for(uint8_t i = 0; HAL_CAN_GetTxMailboxesFreeLevel(hcan) != 3 && i < 3; i++);
	}
}

static void Tx_Verify(CAN_HandleTypeDef* hcan)
{
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
	TxHeader.StdId = FIRST_ID + 4;
	TxHeader.DLC = 8;

	TxData[0]  = Verify_Data[0];
	TxData[0] |= Verify_Data[1] << 1;
	TxData[0] |= Verify_Data[2] << 2;
	TxData[0] |= Verify_Data[3] << 3;
	TxData[0] |= Verify_Data[4] << 4;
	TxData[0] |= Verify_Data[5] << 5;
	TxData[0] |= Verify_Data[6] << 6;
	TxData[0] |= Verify_Data[7] << 7;

	TxData[1]  = Verify_Data[8];
	TxData[1] |= Verify_Data[9] << 1;
	TxData[1] |= Verify_Data[10] << 2;
	TxData[1] |= Verify_Data[11] << 3;
	TxData[1] |= Verify_Datalogger << 4;

	if(Flag_Datalogger == 1)
		Principal_Datalogger_Save_Buffer(TxHeader.StdId, TxHeader.DLC, TxData, &File_Struct);

	if(HAL_CAN_AddTxMessage(hcan, &TxHeader, TxData, &TxMailbox) == HAL_OK)
		Verify_CAN |= 1;
	else
		Verify_CAN &= 0x02;

	//Wait Transmission finish
	for(uint8_t i = 0; HAL_CAN_GetTxMailboxesFreeLevel(hcan) != 3 && i < 3; i++);
}

static void Tx_Beacon(CAN_HandleTypeDef* hcan)
{
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
	TxHeader.StdId = BEACON_ID;
	TxHeader.DLC = 1;

	TxData[0] = Lap_Number;

	if(Flag_Datalogger == 1)
		Principal_Datalogger_Save_Buffer(TxHeader.StdId, TxHeader.DLC, TxData, &File_Struct);

//	if(HAL_CAN_AddTxMessage(hcan, &TxHeader, TxData, &TxMailbox) == HAL_OK)
//		Verify_CAN |= 1;
//	else
//		Verify_CAN &= 0x02;
//
//	//Wait Transmission finish
//	for(uint8_t i = 0; HAL_CAN_GetTxMailboxesFreeLevel(hcan) != 3 && i < 3; i++);
}

static void Save_PDM()
{
	uint8_t id = 0, length = 0, buffer[8];

	if(Flag_Datalogger == 0)
		return;

	id = PDM_FIRST_ID;
	length = 8;

	buffer[0] = PDM_Readings.Current_Buffer[0] << 8;
	buffer[1] = PDM_Readings.Current_Buffer[0] & 0xff;
	buffer[2] = PDM_Readings.Current_Buffer[1] << 8;
	buffer[3] = PDM_Readings.Current_Buffer[1] & 0xff;
	buffer[4] = PDM_Readings.Current_Buffer[2] << 8;
	buffer[5] = PDM_Readings.Current_Buffer[2] & 0xff;
	buffer[6] = PDM_Readings.Current_Buffer[3] << 8;
	buffer[7] = PDM_Readings.Current_Buffer[3] & 0xff;

	Principal_Datalogger_Save_Buffer(id, length, buffer, &File_Struct);

	id = PDM_FIRST_ID + 1;
	length = 8;

	buffer[0] = PDM_Readings.Current_Buffer[4] << 8;
	buffer[1] = PDM_Readings.Current_Buffer[4] & 0xff;
	buffer[2] = PDM_Readings.Current_Buffer[5] << 8;
	buffer[3] = PDM_Readings.Current_Buffer[5] & 0xff;
	buffer[4] = PDM_Readings.Current_Buffer[6] << 8;
	buffer[5] = PDM_Readings.Current_Buffer[6] & 0xff;
	buffer[6] = PDM_Readings.Current_Buffer[7] << 8;
	buffer[7] = PDM_Readings.Current_Buffer[7] & 0xff;

	Principal_Datalogger_Save_Buffer(id, length, buffer, &File_Struct);

	id = PDM_FIRST_ID + 2;
	length = 8;

	buffer[0] = PDM_Readings.Current_Buffer[8] << 8;
	buffer[1] = PDM_Readings.Current_Buffer[8] & 0xff;
	buffer[2] = PDM_Readings.Current_Buffer[9] << 8;
	buffer[3] = PDM_Readings.Current_Buffer[9] & 0xff;
	buffer[4] = PDM_Readings.Current_Buffer[10] << 8;
	buffer[5] = PDM_Readings.Current_Buffer[10] & 0xff;
	buffer[6] = PDM_Readings.Current_Buffer[11] << 8;
	buffer[7] = PDM_Readings.Current_Buffer[11] & 0xff;

	Principal_Datalogger_Save_Buffer(id, length, buffer, &File_Struct);

	id = PDM_FIRST_ID + 3;
	length = 8;

	buffer[0] = PDM_Readings.Current_Buffer[12] << 8;
	buffer[1] = PDM_Readings.Current_Buffer[12] & 0xff;
	buffer[2] = PDM_Readings.Current_Buffer[13] << 8;
	buffer[3] = PDM_Readings.Current_Buffer[13] & 0xff;
	buffer[4] = PDM_Readings.Current_Buffer[14] << 8;
	buffer[5] = PDM_Readings.Current_Buffer[14] & 0xff;
	buffer[6] = PDM_Readings.Current_Buffer[15] << 8;
	buffer[7] = PDM_Readings.Current_Buffer[15] & 0xff;

	Principal_Datalogger_Save_Buffer(id, length, buffer, &File_Struct);

	id = PDM_FIRST_ID + 4;
	length = 8;

	buffer[0] = PDM_Readings.Tempetature_Buffer[0] << 8;
	buffer[1] = PDM_Readings.Tempetature_Buffer[0] & 0xff;
	buffer[2] = PDM_Readings.Tempetature_Buffer[1] << 8;
	buffer[3] = PDM_Readings.Tempetature_Buffer[1] & 0xff;
	buffer[4] = PDM_Readings.Tempetature_Buffer[2] << 8;
	buffer[5] = PDM_Readings.Tempetature_Buffer[2] & 0xff;
	buffer[6] = PDM_Readings.Tempetature_Buffer[3] << 8;
	buffer[7] = PDM_Readings.Tempetature_Buffer[3] & 0xff;

	Principal_Datalogger_Save_Buffer(id, length, buffer, &File_Struct);

	id = PDM_FIRST_ID + 5;
	length = 8;

	buffer[0] = PDM_Readings.Tempetature_Buffer[4] << 8;
	buffer[1] = PDM_Readings.Tempetature_Buffer[4] & 0xff;
	buffer[2] = PDM_Readings.Tempetature_Buffer[5] << 8;
	buffer[3] = PDM_Readings.Tempetature_Buffer[5] & 0xff;
	buffer[4] = PDM_Readings.Tempetature_Buffer[6] << 8;
	buffer[5] = PDM_Readings.Tempetature_Buffer[6] & 0xff;
	buffer[6] = PDM_Readings.Tempetature_Buffer[7] << 8;
	buffer[7] = PDM_Readings.Tempetature_Buffer[7] & 0xff;

	Principal_Datalogger_Save_Buffer(id, length, buffer, &File_Struct);

	id = PDM_FIRST_ID + 6;
	length = 8;

	buffer[0] = PDM_Readings.Duty_Cycle_Buffer[0] << 8;
	buffer[1] = PDM_Readings.Duty_Cycle_Buffer[0] & 0xff;
	buffer[2] = PDM_Readings.Duty_Cycle_Buffer[1] << 8;
	buffer[3] = PDM_Readings.Duty_Cycle_Buffer[1] & 0xff;
	buffer[4] = PDM_Readings.Duty_Cycle_Buffer[2] << 8;
	buffer[5] = PDM_Readings.Duty_Cycle_Buffer[2] & 0xff;
	buffer[6] = PDM_Readings.Duty_Cycle_Buffer[3] << 8;
	buffer[7] = PDM_Readings.Duty_Cycle_Buffer[3] & 0xff;

	Principal_Datalogger_Save_Buffer(id, length, buffer, &File_Struct);

	id = PDM_FIRST_ID + 7;
	length = 4;

	buffer[0] = PDM_Readings.Input_Voltage << 8;
	buffer[1] = PDM_Readings.Input_Voltage & 0xff;
	buffer[2] = PDM_Readings.Output_Verify << 8;
	buffer[3] = PDM_Readings.Output_Verify & 0xff;

	Principal_Datalogger_Save_Buffer(id, length, buffer, &File_Struct);
}

static void Save_ECU()
{
	uint8_t id = 0, length = 0, buffer[8];

	if(Flag_Datalogger == 0)
		return;

	id = ECU_FIRST_ID;
	length = 8;

	buffer[0] = ECU_Data.rpm >> 8;
	buffer[1] = ECU_Data.rpm & 0xff;
	buffer[2] = ECU_Data.tps >> 8;
	buffer[3] = ECU_Data.tps & 0xff;
	buffer[4] = ECU_Data.iat >> 8;
	buffer[5] = ECU_Data.iat & 0xff;
	buffer[6] = ECU_Data.ect >> 8;
	buffer[7] = ECU_Data.ect & 0xff;

	Principal_Datalogger_Save_Buffer(id, length, buffer, &File_Struct);

	id = ECU_FIRST_ID + 1;
	length = 8;

	buffer[0] = ECU_Data.map >> 8;
	buffer[1] = ECU_Data.map & 0xff;
	buffer[2] = ECU_Data.fuel_pressure >> 8;
	buffer[3] = ECU_Data.fuel_pressure & 0xff;
	buffer[4] = ECU_Data.oil_pressure >> 8;
	buffer[5] = ECU_Data.oil_pressure & 0xff;
	buffer[6] = ECU_Data.coolant_pressure >> 8;
	buffer[7] = ECU_Data.coolant_pressure & 0xff;

	Principal_Datalogger_Save_Buffer(id, length, buffer, &File_Struct);

	id = ECU_FIRST_ID + 2;
	length = 8;

	buffer[0] = ECU_Data.lambda >> 8;
	buffer[1] = ECU_Data.lambda & 0xff;
	buffer[2] = ECU_Data.oil_temperature >> 8;
	buffer[3] = ECU_Data.oil_temperature & 0xff;
	buffer[4] = ECU_Data.wheel_speed_fl;
	buffer[5] = ECU_Data.wheel_speed_fr;
	buffer[6] = ECU_Data.wheel_speed_rl;
	buffer[7] = ECU_Data.wheel_speed_rr;

	Principal_Datalogger_Save_Buffer(id, length, buffer, &File_Struct);

	id = ECU_FIRST_ID + 3;
	length = 6;

	buffer[0] = ECU_Data.battery_voltage >> 8;
	buffer[1] = ECU_Data.battery_voltage & 0xff;
	buffer[2] = ECU_Data.total_fuel_flow >> 8;
	buffer[3] = ECU_Data.total_fuel_flow & 0xff;
	buffer[4] = ECU_Data.gear & 0xff;
	buffer[5] = ECU_Data.electro_fan & 0xff;

	Principal_Datalogger_Save_Buffer(id, length, buffer, &File_Struct);

	return;
}

void Principal_CAN_Start(CAN_HandleTypeDef* hcan)
{
	CAN_FilterTypeDef sFilterConfig;
	uint32_t filter_id = CAN_DAQ_FILTER << 13, mask_id = CAN_DAQ_MASK;

	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = filter_id >> 13;
	sFilterConfig.FilterIdLow = (filter_id << 3) & 0xFFF8;
	sFilterConfig.FilterMaskIdHigh = mask_id >> 13;
	sFilterConfig.FilterMaskIdLow = (mask_id << 3) & 0xFFF8;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.FilterBank = 0;
	sFilterConfig.SlaveStartFilterBank = 14;

	HAL_CAN_ConfigFilter(hcan, &sFilterConfig);
	FT_CAN_FilterConfig(hcan, FT600, 1, CAN_RX_FIFO0);
	PDM_CAN_FilterConfig(hcan, 2, CAN_RX_FIFO0);

	HAL_CAN_Start(hcan);

	HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void Principal_Transmit_Msg(CAN_HandleTypeDef* hcan, uint8_t msg_number)
{
	switch(msg_number)
	{
		case Analog_1_4:
			Tx_Analog_1_4(hcan);
			break;

		case Analog_5_8:
			Tx_Analog_5_8(hcan);
			break;

		case Analog_9_12:
			Tx_Analog_9_12(hcan);
			break;

		case Verify_Msg:
			Tx_Verify(hcan);
			break;

		case RTC_Msg:
			Tx_RTC(hcan);
			break;

		case Beacon_Msg:
			Tx_Beacon(hcan);
			break;

		case ECU_Save:
			Save_ECU();
			break;

		case PDM_Save:
			Save_PDM();
			break;

		default:
			return;
	}
}
