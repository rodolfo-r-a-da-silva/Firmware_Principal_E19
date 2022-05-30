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
		memset(fatfs_struct, '\0', sizeof(FATFS));
		return FR_DISK_ERR;
	}

	BSP_SD_Init();

	retVal = f_mount(fatfs_struct, SDPath, 1);

	flagDatalogger = DL_NO_SAVE;

	if(retVal != FR_OK)
	{
		flagDatalogger = DL_ERROR;
		f_mount(0, SDPath, 0);
		memset(fatfs_struct, '\0', sizeof(FATFS));
	}

	accDatalogger[DL_ACC_COOLDOWN] = DATALOGGER_COOLDOWN;

	return retVal;
}

FRESULT Principal_Datalogger_Start(char* dir, char* file, DIR* dir_struct, FIL* file_struct)
{
	FRESULT retVal = FR_OK;

	if(HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_SET)
	{
		Principal_Datalogger_Finish(dir_struct, file_struct);
		return FR_OK;
	}

	else if((flagDatalogger == DL_BUT_PRESS) && (accDatalogger[DL_ACC_COOLDOWN] > 0))
	{
		flagDatalogger = DL_NO_SAVE;
		return FR_OK;
	}

	else if((HAL_GPIO_ReadPin(VBUS_PIN) == GPIO_PIN_SET)
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

	RTC_DateTypeDef sDate;
	RTC_TimeTypeDef sTime;

	accDatalogger[DL_ACC_COOLDOWN] = DATALOGGER_COOLDOWN;

	dataloggerBufferPosition = 0;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

#if _USE_LFN == 0
	sprintf(file, "%02d%02d%02d.sd", sTime.Hours, sTime.Minutes, sTime.Seconds);
#else
	sprintf(dir, "%02d-%02d-%02d", sDate.Year, sDate.Month, sDate.Date);

	sprintf(file, "%s/%s_%02d-%02d-%02d.sd", dir, dir, sTime.Hours, sTime.Minutes, sTime.Seconds);
//	sprintf(file, "%s/%02d-%02d-%02d.sd", dir, sTime.Hours, sTime.Minutes, sTime.Seconds);

	retVal = f_mkdir(dir);

	if((retVal == FR_OK) || (retVal == FR_EXIST))
	{
		retVal = f_opendir(dir_struct, dir);

		if(retVal == FR_OK)
		{
#endif
			retVal = f_open(file_struct, file, FA_WRITE | FA_CREATE_ALWAYS);

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

	if(retVal != FR_OK)
		flagDatalogger = DL_ERROR;

	return retVal;
}

FRESULT Principal_Datalogger_Finish(DIR* dir_struct, FIL* file_struct)
{
	FRESULT retVal = FR_OK;

	if(HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_SET)
	{
		retVal = FR_OK;
		memset(file_struct, '\0', sizeof(FIL));
		memset(dir_struct, '\0', sizeof(DIR));
		memset(&fatfsStruct, '\0', sizeof(FATFS));
		flagDatalogger = DL_NO_CARD;
	}

	else if(flagDatalogger != DL_ERROR)
		flagDatalogger = DL_NO_SAVE;

	if(flagDatalogger != DL_NO_CARD)
	{
		retVal = f_close(file_struct);
		f_closedir(dir_struct);
	}

	accDatalogger[DL_ACC_COOLDOWN] = DATALOGGER_COOLDOWN;

	return retVal;
}

void Principal_Datalogger_Save_Buffer(CAN_HandleTypeDef* hcan, uint32_t data_id, uint8_t data_length, uint8_t* data_buffer, DIR* dir_struct, FIL* file_struct)
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
		Principal_Datalogger_Finish(dir_struct, file_struct);

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

	for(uint8_t i = 0; i < data_length; i++, index++)
		dataloggerBuffer[index + 5] = data_buffer[i];

	dataloggerBufferPosition += (5 + data_length);

	accDatalogger[DL_ACC_TIMING] = 0;

	if((dataloggerBufferPosition > DATALOGGER_SAVE_THR))// || (accDatalogger[DL_ACC_TIMEOUT] > DATALOGGER_SAVE_TIME_THR))
	{
		dataloggerBufferPosition++;

		for(; dataloggerBufferPosition < DATALOGGER_BUFFER_SIZE; dataloggerBufferPosition++)
			dataloggerBuffer[dataloggerBufferPosition] = 0;;

		verify[0] = f_write(file_struct, dataloggerBuffer, (dataloggerBufferPosition + 1), &writeSize);
		verify[1] = f_sync(file_struct);

		if((verify[0] != FR_OK) || (verify[1] != FR_OK))// || (writeSize != (dataloggerBufferPosition + 1)))
		{
			if(flagDatalogger == DL_SAVE)
				flagDatalogger = DL_ERROR;

			Principal_Datalogger_Finish(dir_struct, file_struct);
		}

		dataloggerBufferPosition = 0;
		accDatalogger[DL_ACC_TIMEOUT] = 0;
	}

	HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

	return;
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
