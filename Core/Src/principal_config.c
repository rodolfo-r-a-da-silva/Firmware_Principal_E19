/*
 * principal_config.c
 *
 *  Created on: 22 de jan de 2022
 *      Author: Rodolfo
 */

#include "principal.h"

static HAL_StatusTypeDef Load_EEPROM(I2C_HandleTypeDef* hi2c)
{
	uint8_t buffer[EEPROM_BUFFER_SIZE];
	HAL_StatusTypeDef retVal;

	retVal = HAL_I2C_Master_Transmit(hi2c, 0xA0, 0x00, 1, 5);

	if(retVal == HAL_OK)
		retVal = HAL_I2C_Master_Receive(hi2c, 0xA1, buffer, EEPROM_BUFFER_SIZE, 5);
	else
		return retVal;

	__LOAD_FREQ(buffer[0], Per_Msg[Analog_1_4]);
	__LOAD_FREQ(buffer[1], Per_Msg[Analog_5_8]);
	__LOAD_FREQ(buffer[2], Per_Msg[Analog_9_12]);
	__LOAD_FREQ(buffer[3], Per_Msg[RTC_Msg]);
	__LOAD_FREQ(buffer[4], Per_Msg[Verify_Msg]);
	__LOAD_FREQ(buffer[5], Per_Msg[ECU_Save]);
	__LOAD_FREQ(buffer[6], Per_Msg[PDM_Save]);

	__LOAD_FREQ(buffer[7], Per_CAN[Analog_1_4]);
	__LOAD_FREQ(buffer[8], Per_CAN[Analog_5_8]);
	__LOAD_FREQ(buffer[9], Per_CAN[Analog_9_12]);
	__LOAD_FREQ(buffer[10], Per_CAN[RTC_Msg]);

	Input_Config = buffer[11];

	return retVal;
}

static HAL_StatusTypeDef Save_EEPROM(I2C_HandleTypeDef* hi2c)
{
	uint8_t buffer[EEPROM_BUFFER_SIZE];

	__SAVE_FREQ(buffer[0], Per_Msg[Analog_1_4]);
	__SAVE_FREQ(buffer[1], Per_Msg[Analog_5_8]);
	__SAVE_FREQ(buffer[2], Per_Msg[Analog_9_12]);
	__SAVE_FREQ(buffer[3], Per_Msg[RTC_Msg]);
	__SAVE_FREQ(buffer[4], Per_Msg[Verify_Msg]);
	__SAVE_FREQ(buffer[5], Per_Msg[ECU_Save]);
	__SAVE_FREQ(buffer[6], Per_Msg[PDM_Save]);

	__SAVE_FREQ(buffer[7], Per_CAN[Analog_1_4]);
	__SAVE_FREQ(buffer[8], Per_CAN[Analog_5_8]);
	__SAVE_FREQ(buffer[9], Per_CAN[Analog_9_12]);
	__SAVE_FREQ(buffer[10], Per_CAN[RTC_Msg]);

	buffer[11] = Input_Config;

	return HAL_I2C_Master_Transmit(hi2c, 0xA0, buffer, EEPROM_BUFFER_SIZE, 5);
}

void Principal_Init(CAN_HandleTypeDef* hcan, I2C_HandleTypeDef* hi2c, TIM_HandleTypeDef* htim)
{
	if(Load_EEPROM(hi2c) != HAL_OK)
		Principal_Hard_Code_Config();

	Principal_RTC_Reg_Check(&Date, &Time);

	Principal_Datalogger_Init(&Fatfs_Struct);

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &ADC_Buffer[6], 7);
	HAL_ADC_Start_DMA(&hadc2, (uint32_t*) &ADC_Buffer[0], 6);
	HAL_TIM_Base_Start_IT(htim);

	Principal_CAN_Start(hcan);
}

HAL_StatusTypeDef Principal_Receive_Config(I2C_HandleTypeDef* hi2c, uint8_t* data)
{
	switch(data[0])
	{
		case 0:
			__LOAD_FREQ(data[1], Per_Msg[Analog_1_4]);
			__LOAD_FREQ(data[2], Per_Msg[Analog_5_8]);
			__LOAD_FREQ(data[3], Per_Msg[Analog_9_12]);
			__LOAD_FREQ(data[4], Per_Msg[RTC_Msg]);
			__LOAD_FREQ(data[5], Per_Msg[Verify_Msg]);
			__LOAD_FREQ(data[6], Per_Msg[ECU_Save]);
			__LOAD_FREQ(data[7], Per_Msg[PDM_Save]);
			break;

		case 1:
			__LOAD_FREQ(data[1], Per_CAN[Analog_1_4]);
			__LOAD_FREQ(data[2], Per_CAN[Analog_5_8]);
			__LOAD_FREQ(data[3], Per_CAN[Analog_9_12]);
			__LOAD_FREQ(data[4], Per_CAN[RTC_Msg]);
			Input_Config = data[5];
			break;

		case 2:
			Date.Year = data[1];
			Date.Month = data[2];
			Date.Date = data[3];
			Time.Hours = data[4];
			Time.Minutes = data[5];
			Time.Seconds = data[6];
			Principal_RTC_Get_Date(&Date, &Time);
			break;

		default:
			return HAL_OK;
	}

	return Save_EEPROM(hi2c);
}

__weak void Principal_Hard_Code_Config(){
	Per_Msg[Analog_1_4] = MSG_FREQ_20HZ;
	Threshold_RPM = 7000;
	Threshold_Speed = 1;
}
