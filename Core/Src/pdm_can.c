/*
 * pdm_can.c
 *
 *  Created on: 12 de jan de 2022
 *      Author: Rodolfo
 */

#include "pdm_can.h"

static void Process_Data(uint16_t id, uint16_t data, PDM_Data* data_struct)
{
	switch(id >> 1){
		case 0x0001:
			data_struct->Current_Buffer[0] = data;
			if((id & 0x0001) == 1)
				data_struct->Output_Verify |= 1;
			else
				data_struct->Output_Verify &= ~1;
			break;

		case 0x0002:
			data_struct->Current_Buffer[1] = data;
			if((id & 0x0001) == 1)
				data_struct->Output_Verify |= (1 << 1);
			else
				data_struct->Output_Verify &= ~(1 << 1);
			break;

		case 0x0003:
			data_struct->Current_Buffer[2] = data;
			if((id & 0x0001) == 1)
				data_struct->Output_Verify |= (1 << 2);
			else
				data_struct->Output_Verify &= ~(1 << 2);
			break;

		case 0x0004:
			data_struct->Current_Buffer[3] = data;
			if((id & 0x0001) == 1)
				data_struct->Output_Verify |= (1 << 3);
			else
				data_struct->Output_Verify &= ~(1 << 3);
			break;

		case 0x0005:
			data_struct->Current_Buffer[4] = data;
			if((id & 0x0001) == 1)
				data_struct->Output_Verify |= (1 << 4);
			else
				data_struct->Output_Verify &= ~(1 << 4);
			break;

		case 0x0006:
			data_struct->Current_Buffer[5] = data;
			if((id & 0x0001) == 1)
				data_struct->Output_Verify |= (1 << 5);
			else
				data_struct->Output_Verify &= ~(1 << 5);
			break;

		case 0x0007:
			data_struct->Current_Buffer[6] = data;
			if((id & 0x0001) == 1)
				data_struct->Output_Verify |= (1 << 6);
			else
				data_struct->Output_Verify &= ~(1 << 6);
			break;

		case 0x0008:
			data_struct->Current_Buffer[7] = data;
			if((id & 0x0001) == 1)
				data_struct->Output_Verify |= (1 << 7);
			else
				data_struct->Output_Verify &= ~(1 << 7);
			break;

		case 0x0009:
			data_struct->Current_Buffer[8] = data;
			if((id & 0x0001) == 1)
				data_struct->Output_Verify |= (1 << 8);
			else
				data_struct->Output_Verify &= ~(1 << 8);
			break;

		case 0x000A:
			data_struct->Current_Buffer[9] = data;
			if((id & 0x0001) == 1)
				data_struct->Output_Verify |= (1 << 9);
			else
				data_struct->Output_Verify &= ~(1 << 9);
			break;

		case 0x000B:
			data_struct->Current_Buffer[10] = data;
			if((id & 0x0001) == 1)
				data_struct->Output_Verify |= (1 << 10);
			else
				data_struct->Output_Verify &= ~(1 << 10);
			break;

		case 0x000C:
			data_struct->Current_Buffer[11] = data;
			if((id & 0x0001) == 1)
				data_struct->Output_Verify |= (1 << 11);
			else
				data_struct->Output_Verify &= ~(1 << 11);
			break;

		case 0x000D:
			data_struct->Current_Buffer[12] = data;
			if((id & 0x0001) == 1)
				data_struct->Output_Verify |= (1 << 12);
			else
				data_struct->Output_Verify &= ~(1 << 12);
			break;

		case 0x000E:
			data_struct->Current_Buffer[13] = data;
			if((id & 0x0001) == 1)
				data_struct->Output_Verify |= (1 << 13);
			else
				data_struct->Output_Verify &= ~(1 << 13);
			break;

		case 0x000F:
			data_struct->Current_Buffer[14] = data;
			if((id & 0x0001) == 1)
				data_struct->Output_Verify |= (1 << 14);
			else
				data_struct->Output_Verify &= ~(1 << 14);
			break;

		case 0x0010:
			data_struct->Current_Buffer[15] = data;
			if((id & 0x0001) == 1)
				data_struct->Output_Verify |= (1 << 15);
			else
				data_struct->Output_Verify &= ~(1 << 15);
			break;

		case 0x0011:
			data_struct->Tempetature_Buffer[0] = data;
			break;

		case 0x0012:
			data_struct->Tempetature_Buffer[1] = data;
			break;

		case 0x0013:
			data_struct->Tempetature_Buffer[2] = data;
			break;

		case 0x0014:
			data_struct->Tempetature_Buffer[3] = data;
			break;

		case 0x0015:
			data_struct->Tempetature_Buffer[4] = data;
			break;

		case 0x0016:
			data_struct->Tempetature_Buffer[5] = data;
			break;

		case 0x0017:
			data_struct->Tempetature_Buffer[6] = data;
			break;

		case 0x0018:
			data_struct->Tempetature_Buffer[7] = data;
			break;

		case 0x0019:
			data_struct->Tempetature_Buffer[8] = data;
			break;

		case 0x001A:
			data_struct->Input_Voltage = data;
			break;

		case 0x001B:
			data_struct->Duty_Cycle_Buffer[0] = data;
			break;

		case 0x001C:
			data_struct->Duty_Cycle_Buffer[1] = data;
			break;

		case 0x001D:
			data_struct->Duty_Cycle_Buffer[2] = data;
			break;

		case 0x001E:
			data_struct->Duty_Cycle_Buffer[3] = data;
			break;

		default:
			break;
	}

	return;
}

HAL_StatusTypeDef PDM_CAN_FilterConfig(CAN_HandleTypeDef* hcan, uint32_t bank_position, uint32_t fifo_assignment)
{
	CAN_FilterTypeDef sFilterConfig;
	uint32_t filterID = 0x1E35C000, filterMask = 0x1FFFF000;

	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = filterID >> 13;
	sFilterConfig.FilterIdLow = (filterID << 3) & 0xFFF8;
	sFilterConfig.FilterMaskIdHigh = filterMask >> 13;
	sFilterConfig.FilterMaskIdLow = (filterMask << 3) & 0xFFF8;
	sFilterConfig.FilterBank = bank_position;
	sFilterConfig.FilterFIFOAssignment = fifo_assignment;
	sFilterConfig.FilterActivation = ENABLE;

	return HAL_CAN_ConfigFilter(hcan, &sFilterConfig);
}

void PDM_CAN_Process_Data(CAN_RxHeaderTypeDef* rx_header, uint8_t* data_buffer, PDM_Data* data_struct)
{
	uint8_t length = 0;
	uint8_t buffer[8];
	uint16_t id = 0;
	uint16_t data = 0;

	if((rx_header->DLC != CAN_ID_EXT) && ((rx_header->ExtId & 0x1FFFF000) != 0x1E35C000))
		return;

	length = rx_header->DLC;

	for(uint8_t i = 0; i < length; i++)
		buffer[i] = data_buffer[i];

	id  = buffer[0] << 8;
	id |= buffer[1] & 0xff;
	data  = buffer[2] << 8;
	data |= buffer[3] & 0xff;

	Process_Data(id, data, data_struct);

	if(length == 8)
	{
		id  = buffer[4] << 8;
		id |= buffer[5] & 0xff;
		data  = buffer[6] << 8;
		data |= buffer[7] & 0xff;

		Process_Data(id, data, data_struct);
	}
}
