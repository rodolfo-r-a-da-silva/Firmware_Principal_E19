/*
 * principal_config.c
 *
 *  Created on: 22 de jan de 2022
 *      Author: Rodolfo
 */

#include "principal.h"

static HAL_StatusTypeDef Load_EEPROM(I2C_HandleTypeDef* hi2c);
static HAL_StatusTypeDef Save_EEPROM(I2C_HandleTypeDef* hi2c);

void Principal_Init(CAN_HandleTypeDef* hcan, I2C_HandleTypeDef* hi2c, TIM_HandleTypeDef* htim)
{
	if(Load_EEPROM(hi2c) != HAL_OK)
		Principal_Hard_Code_Config();

//	Load_EEPROM(hi2c);

//	rtcDate.Year = 22;
//	rtcDate.Month = 07;
//	rtcDate.Date = 06;
//	rtcDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
//	rtcTime.Hours = 20;
//	rtcTime.Minutes = 22;
//	rtcTime.Seconds = 0;
//	HAL_RTCEx_SetCoarseCalib(&hrtc, RTC_CALIBSIGN_NEGATIVE, 55);
//	HAL_RTC_SetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);
//	HAL_RTC_SetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
//	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_STORE_DATA);

	if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) == RTC_STORE_DATA)
		flagRTC = RTC_OK;
	else
		flagRTC = RTC_LOST;

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &adcBuffer[6], 6);
	HAL_ADC_Start_DMA(&hadc2, (uint32_t*) &adcBuffer[0], 6);
	HAL_TIM_Base_Start_IT(htim);

	Principal_Datalogger_Init();

	Principal_CAN_Start(hcan);
}

HAL_StatusTypeDef Principal_Receive_Config(I2C_HandleTypeDef* hi2c, uint8_t* data, uint32_t length)
{
	HAL_StatusTypeDef retVal = HAL_OK;

	switch(data[0])
	{
		case CAN_COMMAND_PER_MSG:
			if(length != 8)
				break;

			__BUFFER_TO_FREQ(data[1], perMsg[ANALOG_1_4]);
			__BUFFER_TO_FREQ(data[2], perMsg[ANALOG_5_8]);
			__BUFFER_TO_FREQ(data[3], perMsg[ANALOG_9_12]);
			__BUFFER_TO_FREQ(data[4], perMsg[RTC_MSG]);
			__BUFFER_TO_FREQ(data[5], perMsg[VERIFY_MSG]);
			__BUFFER_TO_FREQ(data[6], perMsg[ECU_SAVE]);
			__BUFFER_TO_FREQ(data[7], perMsg[PDM_SAVE]);

			retVal = Save_EEPROM(hi2c);
			break;

		case CAN_COMMAND_PER_CAN:
			if(length != 6)
				break;

			__BUFFER_TO_FREQ(data[1], perCAN[ANALOG_1_4]);
			__BUFFER_TO_FREQ(data[2], perCAN[ANALOG_5_8]);
			__BUFFER_TO_FREQ(data[3], perCAN[ANALOG_9_12]);
			__BUFFER_TO_FREQ(data[4], perCAN[RTC_MSG]);
			__BUFFER_TO_FREQ(data[5], perCAN[VERIFY_MSG]);

			retVal = Save_EEPROM(hi2c);
			break;

		case CAN_COMMAND_THR:
			if(length != 8)
				break;

			inputConfig		 = data[1];
			thresholdBeacon  = data[2] << 8;
			thresholdBeacon |= data[3] & 0xff;
			thresholdRPM	 = data[4] << 8;
			thresholdRPM	|= data[5] & 0xff;
			thresholdSpeed	 = data[6] << 8;
			thresholdSpeed	|= data[7] & 0xff;

			retVal = Save_EEPROM(hi2c);
			break;

		case CAN_COMMAND_RTC:
			if(length != 7)
				break;

			rtcDate.Year	= data[1];
			rtcDate.Month	= data[2];
			rtcDate.Date	= data[3];
			rtcTime.Hours	= data[4];
			rtcTime.Minutes = data[5];
			rtcTime.Seconds = data[6];

			if((HAL_RTC_SetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN) == HAL_OK)
				&& (HAL_RTC_SetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN) == HAL_OK))
			{
				HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_STORE_DATA);
				flagRTC = RTC_OK;
				retVal = HAL_OK;
			}
			else
				retVal = HAL_ERROR;

			break;

		case CAN_COMMAND_FMT:
			if(length != 1)
				break;

			break;

		default:
			break;
	}

	return retVal;
}

__weak void Principal_Hard_Code_Config(){
	perMsg[ANALOG_1_4] = MSG_FREQ_100HZ;
	perMsg[ANALOG_5_8] = MSG_FREQ_100HZ;
	perMsg[ANALOG_9_12] = MSG_DISABLED;
	perMsg[RTC_MSG] = MSG_FREQ_2HZ;
	perMsg[VERIFY_MSG] = MSG_FREQ_5HZ;
	perMsg[BEACON_MSG] = MSG_FREQ_10HZ;
	perMsg[ECU_SAVE] = MSG_FREQ_100HZ;
	perMsg[PDM_SAVE] = MSG_FREQ_100HZ;

	perCAN[ANALOG_1_4] = MSG_FREQ_10HZ;
	perCAN[ANALOG_5_8] = MSG_FREQ_10HZ;
	perCAN[ANALOG_9_12] = MSG_DISABLED;
	perCAN[RTC_MSG] = MSG_FREQ_2HZ;
	perCAN[VERIFY_MSG] = MSG_FREQ_5HZ;
	perCAN[BEACON_MSG] = MSG_FREQ_5HZ;

	inputConfig = INPUT_BEACON_PIN_1 | INPUT_DATALOGGER_PIN_0 | INPUT_BEACON_FALLING_EDGE | INPUT_DATALOGGER_FALLING_EDGE;

	thresholdBeacon = 1000;
	thresholdRPM = 2000;
	thresholdSpeed = 1;

	Save_EEPROM(&hi2c1);
}

static HAL_StatusTypeDef Load_EEPROM(I2C_HandleTypeDef* hi2c)
{
	uint8_t buffer[EEPROM_BUFFER_SIZE_READ];
	HAL_StatusTypeDef retVal = HAL_OK;

	HAL_I2C_Master_Transmit(hi2c, EEPROM_ADDRESS_WRITE, 0x00, 1, EEPROM_TIMEOUT_WRITE);

	retVal = HAL_I2C_Master_Receive(hi2c, EEPROM_ADDRESS_READ, buffer, EEPROM_BUFFER_SIZE_READ, EEPROM_TIMEOUT_READ);

	if(retVal != HAL_OK)
		return retVal;

	__BUFFER_TO_FREQ(buffer[0], perMsg[ANALOG_1_4]);
	__BUFFER_TO_FREQ(buffer[1], perMsg[ANALOG_5_8]);
	__BUFFER_TO_FREQ(buffer[2], perMsg[ANALOG_9_12]);
	__BUFFER_TO_FREQ(buffer[3], perMsg[RTC_MSG]);
	__BUFFER_TO_FREQ(buffer[4], perMsg[VERIFY_MSG]);
	__BUFFER_TO_FREQ(buffer[5], perMsg[ECU_SAVE]);
	__BUFFER_TO_FREQ(buffer[6], perMsg[PDM_SAVE]);
	__BUFFER_TO_FREQ(buffer[7], perCAN[ANALOG_1_4]);
	__BUFFER_TO_FREQ(buffer[8], perCAN[ANALOG_5_8]);
	__BUFFER_TO_FREQ(buffer[9], perCAN[ANALOG_9_12]);
	__BUFFER_TO_FREQ(buffer[10], perCAN[RTC_MSG]);
	__BUFFER_TO_FREQ(buffer[11], perCAN[VERIFY_MSG]);

	inputConfig 	 = buffer[12];
	thresholdBeacon	 = buffer[13] << 8;
	thresholdBeacon |= buffer[14] & 0xff;
	thresholdRPM	 = buffer[15] << 8;
	thresholdRPM	|= buffer[16] & 0xff;
	thresholdSpeed	 = buffer[17] << 8;
	thresholdSpeed	|= buffer[18] & 0xff;

	return retVal;
}

static HAL_StatusTypeDef Save_EEPROM(I2C_HandleTypeDef* hi2c)
{
	uint8_t buffer[EEPROM_BUFFER_SIZE_WRITE];

	buffer[0] = 0x00;

	__FREQ_TO_BUFFER(buffer[1], perMsg[ANALOG_1_4]);
	__FREQ_TO_BUFFER(buffer[2], perMsg[ANALOG_5_8]);
	__FREQ_TO_BUFFER(buffer[3], perMsg[ANALOG_9_12]);
	__FREQ_TO_BUFFER(buffer[4], perMsg[RTC_MSG]);
	__FREQ_TO_BUFFER(buffer[5], perMsg[VERIFY_MSG]);
	__FREQ_TO_BUFFER(buffer[6], perMsg[ECU_SAVE]);
	__FREQ_TO_BUFFER(buffer[7], perMsg[PDM_SAVE]);
	__FREQ_TO_BUFFER(buffer[8], perCAN[ANALOG_1_4]);

	HAL_I2C_Master_Transmit(hi2c, EEPROM_ADDRESS_WRITE, buffer, EEPROM_BUFFER_SIZE_WRITE, EEPROM_TIMEOUT_WRITE);
	HAL_Delay(10);

	buffer[0] = 0x08;

	__FREQ_TO_BUFFER(buffer[1], perCAN[ANALOG_5_8]);
	__FREQ_TO_BUFFER(buffer[2], perCAN[ANALOG_9_12]);
	__FREQ_TO_BUFFER(buffer[3], perCAN[RTC_MSG]);
	__FREQ_TO_BUFFER(buffer[4], perCAN[VERIFY_MSG]);

	buffer[5] = inputConfig;
	buffer[6] = thresholdBeacon >> 8;
	buffer[7] = thresholdBeacon & 0xff;
	buffer[8] = thresholdRPM >> 8;

	HAL_I2C_Master_Transmit(hi2c, EEPROM_ADDRESS_WRITE, buffer, EEPROM_BUFFER_SIZE_WRITE, EEPROM_TIMEOUT_WRITE);
	HAL_Delay(10);

	buffer[0] = 0x10;
	buffer[1] = thresholdRPM & 0xff;
	buffer[2] = thresholdSpeed >> 8;
	buffer[3] = thresholdSpeed & 0xff;

	return HAL_I2C_Master_Transmit(hi2c, EEPROM_ADDRESS_WRITE, buffer, 4, EEPROM_TIMEOUT_WRITE);
}
