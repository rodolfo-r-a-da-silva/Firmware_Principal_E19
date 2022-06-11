/*
 * principal_datalogger.c
 *
 *  Created on: Jan 5, 2022
 *      Author: Rodolfo
 */

#include "principal.h"
#include "stdio.h"
#include "string.h"

//DATA BUFFER
uint8_t dataloggerBuffer[DATALOGGER_BUFFER_SIZE];
uint16_t dataloggerBufferPosition;

//FILE HANDLING
FATFS fatfsStruct;
DIR dirStruct;
FIL fileStruct;
TCHAR dirString[DIR_STRING_SIZE];
TCHAR logString[LOG_STRING_SIZE];

//VARIABLES
int16_t thresholdBeacon;
int16_t thresholdRPM;
int16_t thresholdSpeed;


FRESULT Principal_Datalogger_Init()
{
	FRESULT retVal = FR_OK;

	if(HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_SET)
	{
		flagDatalogger = DL_NO_CARD;
		memset(&fatfsStruct, '\0', sizeof(FATFS));
		return FR_DISK_ERR;
	}

	BSP_SD_Init();

	retVal = f_mount(&fatfsStruct, SDPath, 1);

	flagDatalogger = DL_NO_SAVE;

	if(retVal != FR_OK)
	{
		flagDatalogger = DL_ERROR;
		f_mount(0, SDPath, 0);
		memset(&fatfsStruct, '\0', sizeof(FATFS));
	}

	accDatalogger[DL_ACC_COOLDOWN] = DATALOGGER_COOLDOWN;

	return retVal;
}

FRESULT Principal_Datalogger_Start()
{
	FRESULT retVal = FR_OK;

	if((flagDatalogger == DL_BUT_PRESS) && (accDatalogger[DL_ACC_COOLDOWN] > 0))
	{
		flagDatalogger = DL_NO_SAVE;
		return FR_OK;
	}

	else if((HAL_GPIO_ReadPin(VBUS_PIN) == GPIO_PIN_SET)
			|| (HAL_GPIO_ReadPin(SDIO_CD_PIN) == GPIO_PIN_SET)
			|| (flagRTC != RTC_OK)
			|| ((flagDatalogger != DL_BUT_PRESS)
			&& (ecuData.rpm < thresholdRPM)
			&& (ecuData.wheel_speed_fl < thresholdSpeed)
			&& (ecuData.wheel_speed_fr < thresholdSpeed)
			&& (ecuData.wheel_speed_rl < thresholdSpeed)
			&& (ecuData.wheel_speed_rr < thresholdSpeed)))
	{
		flagDatalogger = DL_NO_SAVE;
		return FR_OK;
	}

	accDatalogger[DL_ACC_COOLDOWN] = DATALOGGER_COOLDOWN;

	dataloggerBufferPosition = 0;

#ifdef DATALOGGER_LEGACY_NAME

	for(uint16_t i = 0; i < 10000; i++)
	{
		sprintf(logString, "LOG%d.sd", i);

		if(f_stat(logString, NULL) == FR_NO_FILE)
		{
#ifdef DATALOGGER_SYNC
			retVal = f_open(&fileStruct, logString, FA_WRITE | FA_CREATE_ALWAYS);
#endif
			if(retVal == FR_OK)
			{
				flagDatalogger = DL_SAVE;
				accDatalogger[DL_ACC_TIMING] = 0;
				accDatalogger[DL_ACC_TIMEOUT] = 0;
			}

			break;
		}
	}

#else

	RTC_DateTypeDef sDate;
	RTC_TimeTypeDef sTime;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

#if _USE_LFN == 0
	sprintf(logString, "%02d%02d%02d.sd", sTime.Hours, sTime.Minutes, sTime.Seconds);
#else
	sprintf(dirString, "%02d-%02d-%02d", sDate.Year, sDate.Month, sDate.Date);

	sprintf(logString, "%s/%s_%02d-%02d-%02d.sd", dir, dir, sTime.Hours, sTime.Minutes, sTime.Seconds);

	retVal = f_mkdir(dirString);

	if((retVal == FR_OK) || (retVal == FR_EXIST))
	{
		retVal = f_opendir(&dirStruct, dirString);

		if(retVal == FR_OK)
		{
#endif

#ifdef DATALOGGER_SYNC
			retVal = f_open(&fileStruct, logString, FA_WRITE | FA_CREATE_ALWAYS);
#endif

			if(retVal == FR_OK)
			{
				flagDatalogger = DL_SAVE;
				accDatalogger[DL_ACC_TIMING] = 0;
				accDatalogger[DL_ACC_TIMEOUT] = 0;
			}
#if _USE_LFN != 0
		}
	}
#endif

#endif

	if(retVal != FR_OK)
		flagDatalogger = DL_ERROR;

	return retVal;
}

FRESULT Principal_Datalogger_Finish()
{
	FRESULT retVal = FR_OK;

	if(HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_SET)
	{
		retVal = FR_OK;
		f_mount(0, SDPath, 0);
		memset(&fileStruct, '\0', sizeof(FIL));
		memset(&dirStruct, '\0', sizeof(DIR));
		memset(&fatfsStruct, '\0', sizeof(FATFS));
		flagDatalogger = DL_NO_CARD;
	}

	else if(flagDatalogger != DL_ERROR)
		flagDatalogger = DL_NO_SAVE;

	if(flagDatalogger != DL_NO_CARD)
	{
#ifdef DATALOGGER_SYNC
		retVal = f_close(&fileStruct);
#endif
		f_closedir(&dirStruct);
	}

	accDatalogger[DL_ACC_COOLDOWN] = DATALOGGER_COOLDOWN;

	return retVal;
}

void Principal_Datalogger_Save_Buffer(CAN_HandleTypeDef* hcan, uint32_t data_id, uint8_t data_length, uint8_t* data_buffer)
{
//	uint8_t buffer[5 + data_length];
	uint16_t index = 0;
	UINT writeSize = 0;
	FRESULT verify[2];

	HAL_CAN_DeactivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

	if((HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_SET)
		|| (HAL_GPIO_ReadPin(VBUS_PIN) == GPIO_PIN_SET)
		|| (flagDatalogger != DL_SAVE))
	{
		Principal_Datalogger_Finish(&dirStruct, &fileStruct);

		HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

		return;
	}

//	buffer[0] = 'D';
//	buffer[1] = 'L';
//	buffer[2] = data_id & 0xff;
//	buffer[3] = data_length;
//	buffer[4] = accDatalogger[DL_ACC_TIMING];

//	accDatalogger[DL_ACC_TIMING] = 0;

//	for(uint8_t i = 0; i < data_length; i++)
//		buffer[5 + i] = data_buffer[i];

//	memcpy(&dataloggerBuffer[dataloggerBufferPosition], buffer, 5 + data_length);

	index = dataloggerBufferPosition;

	dataloggerBuffer[index] 	= 'D';
	dataloggerBuffer[index + 1] = 'L';
	dataloggerBuffer[index + 2] = data_id & 0xff;
	dataloggerBuffer[index + 3] = data_length;
	dataloggerBuffer[index + 4] = accDatalogger[DL_ACC_TIMING];

	accDatalogger[DL_ACC_TIMING] = 0;

	index += 5;

	for(uint8_t i = 0; i < data_length; i++, index++)
		dataloggerBuffer[index] = data_buffer[i];

	dataloggerBufferPosition += (5 + data_length);

	if((dataloggerBufferPosition > DATALOGGER_SAVE_THR))// || (accDatalogger[DL_ACC_TIMEOUT] > DATALOGGER_SAVE_TIME_THR))
	{
		for(; dataloggerBufferPosition < DATALOGGER_BUFFER_SIZE; dataloggerBufferPosition++)
			dataloggerBuffer[dataloggerBufferPosition] = 0;

#ifdef DATALOGGER_SYNC
		verify[0] = f_write(&fileStruct, dataloggerBuffer, dataloggerBufferPosition, &writeSize);
		verify[1] = f_sync(&fileStruct);
#else
		verify[0] = f_open(&fileStruct, logString, FA_WRITE | FA_OPEN_APPEND);

		if(verify[0] == FR_OK)
		{
			verify[0] = f_write(&fileStruct, dataloggerBuffer, dataloggerBufferPosition, &writeSize);
			verify[1] = f_close(&fileStruct);
		}
#endif

		if((verify[0] != FR_OK) || (verify[1] != FR_OK))// || (writeSize != dataloggerBufferPosition))
		{
			if(flagDatalogger == DL_SAVE)
				flagDatalogger = DL_ERROR;

			Principal_Datalogger_Finish();
		}

		dataloggerBufferPosition = 0;
		accDatalogger[DL_ACC_TIMEOUT] = 0;
	}

	HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

	return;
}

void Principal_Datalogger_Button()
{
	if(accDatalogger[DL_ACC_COOLDOWN] == 0)
	{
		if((flagDatalogger == DL_NO_SAVE) && (HAL_GPIO_ReadPin(VBUS_PIN) == GPIO_PIN_RESET))
			flagDatalogger = DL_BUT_PRESS;

		else if(flagDatalogger == DL_SAVE)
			Principal_Datalogger_Finish();
	}
}

void Principal_Card_Detection()
{
	GPIO_PinState cd_pin = HAL_GPIO_ReadPin(SDIO_CD_PIN);

	if((cd_pin == GPIO_PIN_SET) && (flagDatalogger != DL_NO_CARD))
		Principal_Datalogger_Finish();

	else if((cd_pin == GPIO_PIN_RESET) && (flagDatalogger == DL_NO_CARD))
		Principal_Datalogger_Init();
}

void Principal_Beacon_Detect()
{
	if(accLap < thresholdBeacon)
		return;

	lapNumber++;

	Principal_Transmit_Msg(&hcan1, BEACON_MSG);
}
