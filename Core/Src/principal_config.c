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
//	if(Load_EEPROM(hi2c) != HAL_OK)
		Principal_Hard_Code_Config();

//	rtcDate.Year = 22;
//	rtcDate.Month = 5;
//	rtcDate.Date = 17;
//	rtcDate.WeekDay = RTC_WEEKDAY_TUESDAY;
//	rtcTime.Hours = 23;
//	rtcTime.Minutes = 9;
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

	Principal_Datalogger_Init(&fatfsStruct);

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

			__BUFFER_TO_FREQ(data[1], perMsg[ANALOG_1_4]);
			__BUFFER_TO_FREQ(data[2], perMsg[ANALOG_5_8]);
			__BUFFER_TO_FREQ(data[3], perMsg[ANALOG_9_12]);
			__BUFFER_TO_FREQ(data[4], perMsg[RTC_MSG]);
			__BUFFER_TO_FREQ(data[5], perMsg[VERIFY_MSG]);
			__BUFFER_TO_FREQ(data[6], perMsg[ECU_SAVE]);
			__BUFFER_TO_FREQ(data[7], perMsg[PDM_SAVE]);

			retVal = Save_EEPROM(hi2c);
			break;

		case 1:
			if(length != 7)
				break;

			__BUFFER_TO_FREQ(data[1], perCAN[ANALOG_1_4]);
			__BUFFER_TO_FREQ(data[2], perCAN[ANALOG_5_8]);
			__BUFFER_TO_FREQ(data[3], perCAN[ANALOG_9_12]);
			__BUFFER_TO_FREQ(data[4], perCAN[RTC_MSG]);
			__BUFFER_TO_FREQ(data[5], perCAN[VERIFY_MSG]);
			inputConfig = data[6];

			retVal = Save_EEPROM(hi2c);
			break;

		case 2:
			if(length != 7)
				break;

			thresholdBeacon  = data[1] << 8;
			thresholdBeacon |= data[2] & 0xff;
			thresholdRPM	 = data[3] << 8;
			thresholdRPM	|= data[4] & 0xff;
			thresholdSpeed	 = data[5] << 8;
			thresholdSpeed	|= data[6] & 0xff;

			retVal = Save_EEPROM(hi2c);
			break;

		case 3:
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

		default:
			break;
	}

	return retVal;
}

__weak void Principal_Hard_Code_Config(){
	perMsg[ANALOG_1_4] = MSG_FREQ_250HZ;
	perMsg[ANALOG_5_8] = MSG_FREQ_500HZ;
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
	thresholdRPM = 5000;
	thresholdSpeed = 1;

//	Save_EEPROM(&hi2c1);
}

static HAL_StatusTypeDef Load_EEPROM(I2C_HandleTypeDef* hi2c)
{
	uint8_t buffer[EEPROM_BUFFER_SIZE];
	HAL_StatusTypeDef retVal = HAL_OK;

	HAL_I2C_Master_Transmit(hi2c, EEPROM_ADDRESS_READ, NULL, 0, EEPROM_READ_TIMEOUT);

	retVal = HAL_I2C_Master_Receive(hi2c, EEPROM_ADDRESS_READ, buffer, sizeof(buffer), EEPROM_READ_TIMEOUT);

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
	uint8_t buffer[EEPROM_BUFFER_SIZE];
	HAL_StatusTypeDef retVal = HAL_OK;

	for(uint16_t i = 0; i < EEPROM_BUFFER_SIZE; i++)
		buffer[i] = 0;

	__FREQ_TO_BUFFER(buffer[0], perMsg[ANALOG_1_4]);

	__FREQ_TO_BUFFER(buffer[1], perMsg[ANALOG_5_8]);

	__FREQ_TO_BUFFER(buffer[2], perMsg[ANALOG_9_12]);

	__FREQ_TO_BUFFER(buffer[3], perMsg[RTC_MSG]);

	__FREQ_TO_BUFFER(buffer[4], perMsg[VERIFY_MSG]);

	__FREQ_TO_BUFFER(buffer[5], perMsg[ECU_SAVE]);

	__FREQ_TO_BUFFER(buffer[6], perMsg[PDM_SAVE]);

	__FREQ_TO_BUFFER(buffer[7], perCAN[ANALOG_1_4]);

	__FREQ_TO_BUFFER(buffer[8], perCAN[ANALOG_5_8]);

	__FREQ_TO_BUFFER(buffer[9], perCAN[ANALOG_9_12]);

	__FREQ_TO_BUFFER(buffer[10], perCAN[RTC_MSG]);

	__FREQ_TO_BUFFER(buffer[11], perCAN[VERIFY_MSG]);

	buffer[12] = inputConfig;
	buffer[13] = thresholdBeacon >> 8;
	buffer[14] = thresholdBeacon & 0xff;
	buffer[15] = thresholdRPM >> 8;
	buffer[16] = thresholdRPM & 0xff;
	buffer[17] = thresholdSpeed >> 8;
	buffer[18] = thresholdSpeed & 0xff;

	for(uint16_t i = 0; i < EEPROM_BUFFER_SIZE; i += 4)
		retVal = HAL_I2C_Master_Transmit(hi2c, EEPROM_ADDRESS_WRITE, &buffer[i], 4, EEPROM_WRITE_TIMEOUT);

	return retVal;
}
