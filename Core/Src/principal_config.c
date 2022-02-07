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
	Threshold_RPM	 = buffer[12] << 8;
	Threshold_RPM	|= buffer[13] & 0xff;
	Threshold_Speed	 = buffer[14] << 8;
	Threshold_Speed |= buffer[15] & 0xff;

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
	buffer[12] = Threshold_RPM >> 8;
	buffer[13] = Threshold_RPM & 0xff;
	buffer[14] = Threshold_Speed >> 8;
	buffer[15] = Threshold_Speed & 0xff;

	return HAL_I2C_Master_Transmit(hi2c, 0xA0, buffer, EEPROM_BUFFER_SIZE, 5);
}

void Principal_Init(CAN_HandleTypeDef* hcan, I2C_HandleTypeDef* hi2c, TIM_HandleTypeDef* htim)
{
	if(Load_EEPROM(hi2c) != HAL_OK)
		Principal_Hard_Code_Config();

	if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) == RTC_STORE_DATA)
		Flag_RTC = RTC_OK;
	else
		Flag_RTC = RTC_Lost;

	Principal_Datalogger_Init(&Fatfs_Struct);

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &ADC_Buffer[6], 7);
	HAL_ADC_Start_DMA(&hadc2, (uint32_t*) &ADC_Buffer[0], 6);
	HAL_TIM_Base_Start_IT(htim);

	Principal_CAN_Start(hcan);
}

HAL_StatusTypeDef Principal_Receive_Config(I2C_HandleTypeDef* hi2c, uint8_t* data, uint32_t length)
{
	HAL_SD_StateTypeDef retVal = HAL_OK;

	switch(data[0])
	{
		case 0:
			if(length != 8)
				break;

			__LOAD_FREQ(data[1], Per_Msg[Analog_1_4]);
			__LOAD_FREQ(data[2], Per_Msg[Analog_5_8]);
			__LOAD_FREQ(data[3], Per_Msg[Analog_9_12]);
			__LOAD_FREQ(data[4], Per_Msg[RTC_Msg]);
			__LOAD_FREQ(data[5], Per_Msg[Verify_Msg]);
			__LOAD_FREQ(data[6], Per_Msg[ECU_Save]);
			__LOAD_FREQ(data[7], Per_Msg[PDM_Save]);

			retVal = Save_EEPROM(hi2c);
			break;

		case 1:
			if(length != 6)
				break;

			__LOAD_FREQ(data[1], Per_CAN[Analog_1_4]);
			__LOAD_FREQ(data[2], Per_CAN[Analog_5_8]);
			__LOAD_FREQ(data[3], Per_CAN[Analog_9_12]);
			__LOAD_FREQ(data[4], Per_CAN[RTC_Msg]);
			Input_Config = data[5];

			retVal = Save_EEPROM(hi2c);
			break;

		case 2:
			if(length != 5)
				break;

			Threshold_RPM	 = data[1] << 8;
			Threshold_RPM	|= data[2] & 0xff;
			Threshold_Speed  = data[3] << 8;
			Threshold_Speed |= data[4] & 0xff;

			retVal = Save_EEPROM(hi2c);
			break;

		case 3:
			if(length != 7)
				break;

			Date.Year	 = data[1];
			Date.Month	 = data[2];
			Date.Date	 = data[3];
			Time.Hours	 = data[4];
			Time.Minutes = data[5];
			Time.Seconds = data[6];

			HAL_RTC_SetDate(&hrtc, &Date, RTC_FORMAT_BIN);
			HAL_RTC_SetTime(&hrtc, &Time, RTC_FORMAT_BIN);
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_STORE_DATA);
			Flag_RTC = RTC_OK;
			break;

		default:
			break;
	}

	return retVal;
}

__weak void Principal_Hard_Code_Config(){
	Per_Msg[Analog_1_4] = MSG_FREQ_20HZ;
	Per_Msg[RTC_Msg] = MSG_FREQ_2HZ;
	Threshold_RPM = 7000;
	Threshold_Speed = 1;
}
