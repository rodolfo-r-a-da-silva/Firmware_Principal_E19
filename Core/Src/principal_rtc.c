/*
 * principal_rtc.c
 *
 *  Created on: Jan 11, 2022
 *      Author: Rodolfo
 */

#include "principal.h"

void Principal_RTC_Reg_Check(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime)
{
	if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) == RTC_STORE_DATA)
		Flag_RTC = RTC_OK;

	else
		Flag_RTC = RTC_Lost;

	HAL_RTC_GetDate(&hrtc, &Date, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&hrtc, &Time, RTC_FORMAT_BIN);
}

void Principal_RTC_Get_Date(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime)
{
	HAL_RTC_GetDate(&hrtc, sDate, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&hrtc, sTime, RTC_FORMAT_BIN);
}

void Principal_RTC_Set_Date(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime)
{
	//Standard Configuration
	sTime->DayLightSaving	= RTC_DAYLIGHTSAVING_NONE;
	sTime->StoreOperation 	= RTC_STOREOPERATION_RESET;
	sTime->TimeFormat 		= RTC_HOURFORMAT_24;

	HAL_RTC_SetDate(&hrtc, sDate, RTC_FORMAT_BIN);
	HAL_RTC_SetTime(&hrtc, sTime, RTC_FORMAT_BIN);

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_STORE_DATA);

	Flag_RTC = RTC_OK;
}
