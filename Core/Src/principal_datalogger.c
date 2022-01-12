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

	BSP_SD_Init();

	retVal = f_mount(fatfs_struct, SDPath, 1);

	if(retVal != FR_OK)
		f_mount(0, SDPath, 0);

	return retVal;
}

FRESULT Principal_Datalogger_Start(RTC_DateTypeDef* sDate, RTC_TimeTypeDef* sTime, char* dir, char* file, DIR* dir_struct, FIL* file_struct)
{
	FRESULT retVal = FR_OK;

	Principal_RTC_Get_Date(sDate, sTime);
	sprintf(dir, "%02d-%02d-%02d\n", sDate->Year, sDate->Month, sDate->Date);

	retVal = f_mkdir(dir);

	if((retVal != FR_OK) && (retVal != FR_EXIST))
		return retVal;

	retVal = f_opendir(dir_struct, dir);

	if(retVal != FR_OK)
		return retVal;

	sprintf(file, "%02d-%02d-%02d-%02d-%02d-%02d\n", sDate->Year, sDate->Month, sDate->Date, sTime->Hours, sTime->Minutes, sTime->Seconds);

	return f_open(file_struct, file, FA_WRITE | FA_CREATE_ALWAYS);
}

FRESULT Principal_Datalogger_Finish(DIR* dir_struct, FIL* file_struct)
{
	FRESULT retVal = FR_OK;

	retVal = f_close(file_struct);

	f_closedir(dir_struct);

	return retVal;
}

void Principal_Datalogger_Save_Buffer(uint32_t Data_ID, uint8_t Data_Length, uint8_t* Data_Buffer, FIL* file_struct)
{
	uint8_t buffer[5 + Data_Length];
	BYTE byte;
	FRESULT verify[2];

	buffer[0] = 'D';
	buffer[1] = 'L';
	buffer[2] = Data_ID & 0xff;
	buffer[3] = Data_Length;
	buffer[4] = Acc_Datalogger;

	Acc_Datalogger = 0;

	for(uint8_t i = 0; i < Data_Length; i++)
		buffer[5 + i] = Data_Buffer[i];

	memcpy(&Datalogger_Buffer + Datalogger_Buffer_Position, buffer, 5 + Data_Length);

	Datalogger_Buffer_Position += (5 + Data_Length);

	if(Datalogger_Buffer_Position > (DATALOGGER_BUFFER_SIZE - DATALOGGER_MSG_MAX_SIZE))
	{
		verify[0] = f_write(file_struct, &Datalogger_Buffer, Datalogger_Buffer_Position, &byte);

		verify[1] = f_sync(file_struct);

		if((verify[0] == FR_OK) && (verify[1] == FR_OK))
			Verify_Datalogger = 1;
		else
			Verify_Datalogger = 0;
	}
}
