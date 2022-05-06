/*
 * pdm_can.h
 *
 *  Created on: 12 de jan de 2022
 *      Author: Rodolfo
 */

#ifndef INC_PDM_CAN_H_
#define INC_PDM_CAN_H_

#include "main.h"

typedef struct{
	uint16_t Input_Voltage;
	uint16_t Output_Verify;
	uint16_t Current_Buffer[16];
	uint16_t Duty_Cycle_Buffer[4];
	uint16_t Tempetature_Buffer[9];
}PDM_Data;

HAL_StatusTypeDef PDM_CAN_FilterConfig(CAN_HandleTypeDef* hcan, uint32_t bank_position, uint32_t fifo_assignment);

void PDM_CAN_Process_Data(CAN_RxHeaderTypeDef* rx_header, uint8_t* data_buffer, PDM_Data* data_struct);

#endif /* INC_PDM_CAN_H_ */
