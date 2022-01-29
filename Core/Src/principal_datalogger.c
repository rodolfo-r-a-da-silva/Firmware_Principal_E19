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
		Flag_Datalogger = DL_No_Card;
		return FR_DISK_ERR;
	}

	BSP_SD_Init();

	retVal = f_mount(fatfs_struct, SDPath, 1);

	Flag_Datalogger = DL_No_Save;

	if(retVal != FR_OK)
	{
		Flag_Datalogger = DL_Error;
		f_mount(0, SDPath, 0);
	}

	return retVal;
}

FRESULT Principal_Datalogger_Start(RTC_DateTypeDef* sDate, RTC_TimeTypeDef* sTime, char* dir, char* file, DIR* dir_struct, FIL* file_struct)
{
	FRESULT retVal = FR_OK;

	if(HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_SET)
	{
		Flag_Datalogger = DL_No_Card;
		return FR_DISK_ERR;
	}

	if((HAL_GPIO_ReadPin(VBUS_PIN) == GPIO_PIN_RESET)
			&& (ECU_Data.rpm < Threshold_RPM)
			&& (ECU_Data.wheel_speed_fl < Threshold_Speed)
			&& (ECU_Data.wheel_speed_fr < Threshold_Speed)
			&& (ECU_Data.wheel_speed_rl < Threshold_Speed)
			&& (ECU_Data.wheel_speed_rr < Threshold_Speed)
			&& (Flag_Datalogger != DL_But_Press)
			&& (HAL_GPIO_ReadPin(VBUS_PIN) == GPIO_PIN_RESET))
		return FR_OK;

	Datalogger_Buffer_Position = 0;
	memset(Datalogger_Buffer, '\0', DATALOGGER_BUFFER_SIZE);

	Principal_RTC_Get_Date(sDate, sTime);
	sprintf(dir, "%02d_%02d_%02d", sDate->Year, sDate->Month, sDate->Date);

	sprintf(file, "%s/%02d_%02d_%02d_%02d_%02d_%02d.sd", dir, sDate->Year, sDate->Month, sDate->Date, sTime->Hours, sTime->Minutes, sTime->Seconds);

	retVal = f_mkdir(dir);

	res[0] = retVal;

	if((retVal != FR_OK) && (retVal != FR_EXIST))
	{
		Flag_Datalogger = DL_Error;
		return retVal;
	}

	retVal = f_opendir(dir_struct, dir);

	res[1] = retVal;

	if(retVal != FR_OK)
	{
		Flag_Datalogger = DL_Error;
		return retVal;
	}

	retVal = f_open(file_struct, file, FA_WRITE | FA_CREATE_ALWAYS);

	res[2] = retVal;

	if(retVal == FR_OK)
		Flag_Datalogger = DL_Save;
	else
		Flag_Datalogger = DL_Error;

	return retVal;
}

FRESULT Principal_Datalogger_Finish(DIR* dir_struct, FIL* file_struct)
{
	FRESULT retVal = FR_OK;

	f_closedir(dir_struct);
	retVal = f_close(file_struct);

	if(HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_SET)
	{
		f_mount(0, SDPath, 0);
		Flag_Datalogger = DL_No_Card;
	}

	else
		Flag_Datalogger = DL_No_Save;

	Verify_Datalogger = 0;

	return retVal;
}

void Principal_Datalogger_Save_Buffer(uint32_t Data_ID, uint8_t Data_Length, uint8_t* Data_Buffer, FIL* file_struct)
{
	uint8_t buffer[5 + Data_Length];
	UINT byte;
	FRESULT verify[2];

	if(HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin) == GPIO_PIN_SET)
		return;

	else if(HAL_GPIO_ReadPin(VBUS_PIN) == GPIO_PIN_SET)
		return;

	buffer[0] = 'D';
	buffer[1] = 'L';
	buffer[2] = Data_ID & 0xff;
	buffer[3] = Data_Length;
	buffer[4] = Acc_Datalogger[0];

	Acc_Datalogger[0] = 0;

	for(uint8_t i = 0; i < Data_Length; i++)
		buffer[5 + i] = Data_Buffer[i];

//	memcpy(&buffer[5], Data_Buffer, Data_Length);

	memcpy(Datalogger_Buffer + Datalogger_Buffer_Position, buffer, 5 + Data_Length);

	Datalogger_Buffer_Position += (5 + Data_Length);

	if(Datalogger_Buffer_Position > DATALOGGER_SAVE_THR)
	{
		Datalogger_Buffer_Position++;

		verify[0] = f_write(file_struct, Datalogger_Buffer, Datalogger_Buffer_Position, &byte);
		verify[1] = f_sync(file_struct);

		res[3] = verify[0];
		res[4] = verify[1];

		if((verify[0] == FR_OK) && (verify[1] == FR_OK) && (Datalogger_Buffer_Position == byte))
			Verify_Datalogger = 1;
		else
			Verify_Datalogger = 0;

		Datalogger_Buffer_Position = 0;
	}
}

void Principal_Datalogger_Button(DIR* dir_struct, FIL* file_struct)
{
	if(Acc_Datalogger[1] == 0)
	{
		Acc_Datalogger[1] = BUTTON_COOLDOWN;

		if(Flag_Datalogger == DL_No_Save)
			Flag_Datalogger = DL_But_Press;

		else
			Principal_Datalogger_Finish(dir_struct, file_struct);
	}
}

void Principal_Card_Detection(FATFS* fatfs_struct, DIR* dir_struct, FIL* file_struct)
{
	GPIO_PinState cd_pin = HAL_GPIO_ReadPin(SDIO_CD_GPIO_Port, SDIO_CD_Pin);

	if((cd_pin == GPIO_PIN_SET) && ((Flag_Datalogger == DL_Save) || (Flag_Datalogger == DL_Error)))
		Principal_Datalogger_Finish(dir_struct, file_struct);

	else if((cd_pin == GPIO_PIN_RESET) && (Flag_Datalogger == DL_No_Card))
		Principal_Datalogger_Init(fatfs_struct);
}

void Principal_Beacon_Detect()
{
	Lap_Number++;

	Principal_Transmit_Msg(&hcan1, Beacon_Msg);
}
