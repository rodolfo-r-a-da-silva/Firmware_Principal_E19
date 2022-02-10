/*
 * principal_datalogger.c
 *
 *  Created on: Jan 5, 2022
 *      Author: Rodolfo
 */

#include "principal.h"
#include "stdio.h"
#include "string.h"

FRESULT res[5];

FRESULT Principal_Datalogger_Init(FATFS* fatfs_struct)
{
	FRESULT retVal = FR_OK;

	if(HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_SET)
	{
		flagDatalogger = DL_NO_CARD;
		return FR_DISK_ERR;
	}

	BSP_SD_Init();

	retVal = f_mount(fatfs_struct, SDPath, 1);

	flagDatalogger = DL_NO_SAVE;

	if(retVal != FR_OK)
	{
		flagDatalogger = DL_ERROR;
		f_mount(0, SDPath, 0);
	}

	return retVal;
}

FRESULT Principal_Datalogger_Start(char* dir, char* file, DIR* dir_struct, FIL* file_struct)
{
	FRESULT retVal = FR_OK;

	if(HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_SET)
	{
		flagDatalogger = DL_NO_CARD;
		return FR_DISK_ERR;
	}

	if((HAL_GPIO_ReadPin(VBUS_PIN) == GPIO_PIN_SET)
			|| (flagRTC != RTC_OK)
			|| ((flagDatalogger != DL_BUT_PRESS)
			&& (ecuData.rpm < thresholdRPM)
			&& (ecuData.wheel_speed_fl < thresholdSpeed)
			&& (ecuData.wheel_speed_fr < thresholdSpeed)
			&& (ecuData.wheel_speed_rl < thresholdSpeed)
			&& (ecuData.wheel_speed_rr < thresholdSpeed)))
		return FR_OK;

	RTC_DateTypeDef sDate;
	RTC_TimeTypeDef sTime;

	dataloggerBufferPosition = 0;
	memset(dataloggerBuffer, '\0', DATALOGGER_BUFFER_SIZE);

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	sprintf(dir, "%02d_%02d_%02d", sDate.Year, sDate.Month, sDate.Date);

	sprintf(file, "%s/%02d_%02d_%02d_%02d_%02d_%02d.sd", dir, sDate.Year, sDate.Month, sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds);

	retVal = f_mkdir(dir);

	res[0] = retVal;

	if((retVal != FR_OK) && (retVal != FR_EXIST))
	{
		flagDatalogger = DL_ERROR;
		return retVal;
	}

	retVal = f_opendir(dir_struct, dir);

	res[1] = retVal;

	if(retVal != FR_OK)
	{
		flagDatalogger = DL_ERROR;
		return retVal;
	}

	retVal = f_open(file_struct, file, FA_WRITE | FA_CREATE_ALWAYS);

	res[2] = retVal;

	if(retVal == FR_OK)
		flagDatalogger = DL_SAVE;
	else
		flagDatalogger = DL_ERROR;

	return retVal;
}

FRESULT Principal_Datalogger_Finish(DIR* dir_struct, FIL* file_struct)
{
	FRESULT retVal = FR_OK;

	retVal = f_close(file_struct);
	f_closedir(dir_struct);

	if(HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_SET)
	{
		f_mount(0, SDPath, 0);
		flagDatalogger = DL_NO_CARD;
	}

	else
		flagDatalogger = DL_NO_SAVE;

	verifyDatalogger = 0;

	return retVal;
}

void Principal_Datalogger_Save_Buffer(uint32_t data_id, uint8_t data_length, uint8_t* data_buffer, FIL* file_struct)
{
//	uint8_t buffer[5 + data_length];
	UINT byte;
	FRESULT verify[2];

	if(HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_SET)
		return;

	else if(HAL_GPIO_ReadPin(VBUS_PIN) == GPIO_PIN_SET)
		return;

//	buffer[0] = 'D';
//	buffer[1] = 'L';
//	buffer[2] = data_id & 0xff;
//	buffer[3] = data_length;
//	buffer[4] = accDatalogger[0];

	dataloggerBuffer[dataloggerBufferPosition++] = 'D';
	dataloggerBuffer[dataloggerBufferPosition++] = 'L';
	dataloggerBuffer[dataloggerBufferPosition++] = data_id & 0xff;
	dataloggerBuffer[dataloggerBufferPosition++] = data_length;
	dataloggerBuffer[dataloggerBufferPosition++] = accDatalogger[0];

	accDatalogger[0] = 0;

//	for(uint8_t i = 0; i < data_length; i++)
//		buffer[5 + i] = data_buffer[i];
//
//	memcpy(dataloggerBuffer + dataloggerBufferPosition, buffer, 5 + data_length);
//
//	dataloggerBufferPosition += (5 + data_length);

	for(uint8_t i = 0; i < data_length; i++, dataloggerBufferPosition++)
		dataloggerBuffer[dataloggerBufferPosition] = data_buffer[i];

	if(dataloggerBufferPosition > DATALOGGER_SAVE_THR)
	{
		verify[0] = f_write(file_struct, dataloggerBuffer, dataloggerBufferPosition, &byte);
		verify[1] = f_sync(file_struct);

		res[3] = verify[0];
		res[4] = verify[1];

		if((verify[0] == FR_OK) && (verify[1] == FR_OK) && (dataloggerBufferPosition == byte))
			verifyDatalogger = 1;
		else
			verifyDatalogger = 0;

		dataloggerBufferPosition = 0;
	}
}

void Principal_Datalogger_Button(DIR* dir_struct, FIL* file_struct)
{
	if(accDatalogger[1] == 0)
	{
		accDatalogger[1] = BUTTON_COOLDOWN;

		if(flagDatalogger == DL_NO_SAVE)
			flagDatalogger = DL_BUT_PRESS;

		else
			Principal_Datalogger_Finish(dir_struct, file_struct);
	}
}

void Principal_Card_Detection(FATFS* fatfs_struct, DIR* dir_struct, FIL* file_struct)
{
	GPIO_PinState cd_pin = HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin);

	if((cd_pin == GPIO_PIN_SET) && ((flagDatalogger == DL_SAVE) || (flagDatalogger == DL_ERROR)))
		Principal_Datalogger_Finish(dir_struct, file_struct);

	else if((cd_pin == GPIO_PIN_RESET) && (flagDatalogger == DL_NO_CARD))
		Principal_Datalogger_Init(fatfs_struct);
}

void Principal_Beacon_Detect()
{
	lapNumber++;

	Principal_Transmit_Msg(&hcan1, BEACON_MSG);
}
