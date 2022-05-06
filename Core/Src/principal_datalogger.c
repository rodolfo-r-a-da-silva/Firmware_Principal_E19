/*
 * principal_datalogger.c
 *
 *  Created on: Jan 5, 2022
 *      Author: Rodolfo
 */

#include "principal.h"
#include "stdio.h"
#include "string.h"

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
			|| (accDatalogger[DL_ACC_COOLDOWN] > 0)
			|| ((flagDatalogger != DL_BUT_PRESS)
			&& (ecuData.rpm < thresholdRPM)
			&& (ecuData.wheel_speed_fl < thresholdSpeed)
			&& (ecuData.wheel_speed_fr < thresholdSpeed)
			&& (ecuData.wheel_speed_rl < thresholdSpeed)
			&& (ecuData.wheel_speed_rr < thresholdSpeed)))
		return FR_OK;

	accDatalogger[DL_ACC_COOLDOWN] = DATALOGGER_BUTTON_COOLDOWN;

	RTC_DateTypeDef sDate;
	RTC_TimeTypeDef sTime;

	dataloggerBufferPosition = 0;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	sprintf(dir, "%02d-%02d-%02d", sDate.Year, sDate.Month, sDate.Date);

//	sprintf(file, "%s/%02d-%02d-%02d_%02d-%02d-%02d.sd", dir, sDate.Year, sDate.Month, sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds);
	sprintf(file, "%s/%s_%02d-%02d-%02d.sd", dir, dir, sTime.Hours, sTime.Minutes, sTime.Seconds);

	retVal = f_mkdir(dir);

	if((retVal != FR_OK) && (retVal != FR_EXIST))
	{
		flagDatalogger = DL_ERROR;
		return retVal;
	}

	retVal = f_opendir(dir_struct, dir);

	if(retVal != FR_OK)
	{
		flagDatalogger = DL_ERROR;
		return retVal;
	}

	retVal = f_open(file_struct, file, FA_WRITE | FA_CREATE_ALWAYS);

	if(retVal == FR_OK)
	{
		flagDatalogger = DL_SAVE;
		accDatalogger[DL_ACC_TIMING] = 0;
		accDatalogger[DL_ACC_TIMEOUT] = 0;
	}
	else
		flagDatalogger = DL_ERROR;

	return retVal;
}

FRESULT Principal_Datalogger_Finish(DIR* dir_struct, FIL* file_struct)
{
	FRESULT retVal = FR_OK;

	while(HAL_SD_GetCardState(&hsd) == HAL_SD_STATE_BUSY);

	retVal = f_close(file_struct);
	f_closedir(dir_struct);

	if(HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_SET)
	{
		retVal = f_mount(0, SDPath, 0);

		flagDatalogger = DL_NO_CARD;
	}

	else if(flagDatalogger != DL_ERROR)
		flagDatalogger = DL_NO_SAVE;

	accDatalogger[DL_ACC_COOLDOWN] = DATALOGGER_BUTTON_COOLDOWN;

	return retVal;
}

void Principal_Datalogger_Save_Buffer(uint32_t data_id, uint8_t data_length, uint8_t* data_buffer, DIR* dir_struct, FIL* file_struct)
{
	uint8_t buffer[5 + data_length];
	UINT writeSize;
	FRESULT verify[2];

	if((HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_SET)
		|| (HAL_GPIO_ReadPin(VBUS_PIN) == GPIO_PIN_SET))
	{
		Principal_Datalogger_Finish(dir_struct, file_struct);
		return;
	}

	buffer[0] = 'D';
	buffer[1] = 'L';
	buffer[2] = data_id & 0xff;
	buffer[3] = data_length;
	buffer[4] = accDatalogger[DL_ACC_TIMING];

	accDatalogger[DL_ACC_TIMING] = 0;

	for(uint8_t i = 0; i < data_length; i++)
		buffer[5 + i] = data_buffer[i];

	memcpy(dataloggerBuffer + dataloggerBufferPosition, buffer, 5 + data_length);

	dataloggerBufferPosition += (5 + data_length);

	if((dataloggerBufferPosition > DATALOGGER_SAVE_THR) || (accDatalogger[DL_ACC_TIMEOUT] > DATALOGGER_SAVE_TIME_THR))
	{
		dataloggerBufferPosition++;

		verify[0] = f_write(file_struct, dataloggerBuffer, dataloggerBufferPosition, &writeSize);
		verify[1] = f_sync(file_struct);

		if(((verify[0] != FR_OK) || (verify[1] != FR_OK) || (writeSize != dataloggerBufferPosition)) && (flagDatalogger != DL_SAVE))
		{
			flagDatalogger = DL_ERROR;
			Principal_Datalogger_Finish(dir_struct, file_struct);
		}

		dataloggerBufferPosition = 0;
		accDatalogger[DL_ACC_TIMEOUT] = 0;
	}
}

void Principal_Datalogger_Button(DIR* dir_struct, FIL* file_struct)
{
	if(accDatalogger[DL_ACC_COOLDOWN] == 0)
	{
		if((flagDatalogger == DL_NO_SAVE) && (HAL_GPIO_ReadPin(VBUS_PIN) == GPIO_PIN_RESET))
			flagDatalogger = DL_BUT_PRESS;

		else if(flagDatalogger == DL_SAVE)
			Principal_Datalogger_Finish(dir_struct, file_struct);
	}
}

void Principal_Card_Detection(FATFS* fatfs_struct, DIR* dir_struct, FIL* file_struct)
{
	GPIO_PinState cd_pin = HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin);

	if((cd_pin == GPIO_PIN_SET) && (flagDatalogger != DL_NO_CARD))
		Principal_Datalogger_Finish(dir_struct, file_struct);

	else if((cd_pin == GPIO_PIN_RESET) && (flagDatalogger == DL_NO_CARD))
		Principal_Datalogger_Init(fatfs_struct);
}

void Principal_Beacon_Detect()
{
	if(accLap < thresholdBeacon)
		return;

	lapNumber++;

	Principal_Transmit_Msg(&hcan1, BEACON_MSG);
}
