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
		return FR_DISK_ERR;
	}

	BSP_SD_Init();

	retVal = f_mount(&fatfsStruct, SDPath, 1);

	if(retVal == FR_OK)
		flagDatalogger = DL_NO_SAVE;

	else
		flagDatalogger = DL_ERROR;

	accDatalogger[DL_ACC_COOLDOWN] = DATALOGGER_COOLDOWN;

	return retVal;
}

FRESULT Principal_Datalogger_Start()
{
	FRESULT retVal = FR_OK;

	if((flagDatalogger != DL_BUT_START) && (flagDatalogger != DL_NO_SAVE))
		return retVal;

	else if((flagRTC != RTC_OK)
			|| ((flagDatalogger != DL_BUT_START)
			&& __DL_THR_COND()))
	{
		flagDatalogger = DL_NO_SAVE;
		return retVal;
	}

	HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);

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

	sprintf(logString, "%s/%s_%02d-%02d-%02d.sd", dirString, dirString, sTime.Hours, sTime.Minutes, sTime.Seconds);

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

	dataloggerBufferPosition = 0;
	accDatalogger[DL_ACC_COOLDOWN] = DATALOGGER_COOLDOWN;

	HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);

	return retVal;
}

FRESULT Principal_Datalogger_Finish()
{
	FRESULT retVal = FR_OK;

	HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);

	accDatalogger[DL_ACC_COOLDOWN] = DATALOGGER_COOLDOWN;

	if(HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_SET)
		flagDatalogger = DL_NO_CARD;

	else if(flagDatalogger != DL_ERROR)
		flagDatalogger = DL_NO_SAVE;

#ifdef DATALOGGER_SYNC
	retVal = f_close(&fileStruct);
#endif

#if	_USE_LFN != 0
	f_closedir(&dirStruct);
#endif

	HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);

	return retVal;
}

void Principal_Datalogger_Save_Data(CAN_HandleTypeDef* hcan, uint32_t data_id, uint8_t data_length, uint8_t* data_buffer)
{
	uint8_t buffer[5 + data_length];
	UINT writeSize = 0;
	FRESULT verify;

	HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);

	buffer[0] = 'D';
	buffer[1] = 'L';
	buffer[2] = data_id & 0xff;
	buffer[3] = data_length;
	buffer[4] = accDatalogger[DL_ACC_TIMING];

	verify = f_write(&fileStruct, buffer, sizeof(buffer), &writeSize);

	dataloggerWrite++;

	if((dataloggerWrite > 0) && (accDatalogger[DL_ACC_TIMEOUT] > DATALOGGER_SAVE_TIME_THR) && (verify == FR_OK))
	{
		verify = f_sync(&fileStruct);

		dataloggerWrite = 0;
		accDatalogger[DL_ACC_TIMEOUT] = 0;
	}

	if(verify != FR_OK)
	{
		flagDatalogger = DL_ERROR;

		Principal_Datalogger_Finish();
	}

	HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);

	return;
}

void Principal_Datalogger_Save_Buffer(CAN_HandleTypeDef* hcan, uint32_t data_id, uint8_t data_length, uint8_t* data_buffer)
{
	uint8_t buffer[5 + data_length];
	UINT writeSize = 0;
	FRESULT verify[2];

//	HAL_CAN_DeactivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
	HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);

	buffer[0] = 'D';
	buffer[1] = 'L';
	buffer[2] = data_id & 0xff;
	buffer[3] = data_length;
	buffer[4] = accDatalogger[DL_ACC_TIMING];

	accDatalogger[DL_ACC_TIMING] = 0;

	for(uint8_t i = 0; i < data_length; i++)
		buffer[5 + i] = data_buffer[i];

	for(uint16_t i = 0; i < (5 + data_length); i++)
		dataloggerBuffer[dataloggerBufferPosition + i] = buffer[i];

	dataloggerBufferPosition += (5 + data_length);

	if((dataloggerBufferPosition > DATALOGGER_SAVE_THR))// || (accDatalogger[DL_ACC_TIMEOUT] > DATALOGGER_SAVE_TIME_THR))
	{
//		for(; dataloggerBufferPosition < DATALOGGER_BUFFER_SIZE; dataloggerBufferPosition++)
//			dataloggerBuffer[dataloggerBufferPosition] = 0;

#ifdef DATALOGGER_SYNC

		verify[0] = f_write(&fileStruct, dataloggerBuffer, DATALOGGER_BUFFER_SIZE, &writeSize);
		verify[1] = f_sync(&fileStruct);

#else

		verify[0] = f_open(&fileStruct, logString, FA_WRITE | FA_OPEN_APPEND);

		if(verify[0] == FR_OK)
		{
			verify[0] = f_write(&fileStruct, dataloggerBuffer, dataloggerBufferPosition, &writeSize);
			verify[1] = f_close(&fileStruct);
		}

#endif

		if((verify[0] != FR_OK) || (verify[1] != FR_OK) || (writeSize != DATALOGGER_BUFFER_SIZE))
		{
//			if(flagDatalogger == DL_SAVE)
				flagDatalogger = DL_ERROR;

			Principal_Datalogger_Finish();
		}

		memset(dataloggerBuffer, 0, DATALOGGER_BUFFER_SIZE);
		dataloggerBufferPosition = 0;
		accDatalogger[DL_ACC_TIMEOUT] = 0;
	}

//	HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
	HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);

	return;
}

void Principal_Datalogger_Button()
{
	if(accDatalogger[DL_ACC_COOLDOWN] == 0)
	{
		accDatalogger[DL_ACC_COOLDOWN] = DATALOGGER_COOLDOWN;

		if((flagDatalogger == DL_NO_SAVE) && (HAL_GPIO_ReadPin(VBUS_PIN) == GPIO_PIN_RESET))
			flagDatalogger = DL_BUT_START;

		else if(flagDatalogger == DL_SAVE)
			Principal_Datalogger_Finish();
	}
}

void Principal_Card_Detection()
{
	GPIO_PinState cd_pin = HAL_GPIO_ReadPin(SDIO_CD_PIN);

	if(((cd_pin == GPIO_PIN_SET) && (flagDatalogger != DL_NO_CARD))
			|| ((HAL_GPIO_ReadPin(VBUS_PIN) == GPIO_PIN_SET) && (flagDatalogger == DL_SAVE)))
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
