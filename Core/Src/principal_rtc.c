/*
 * principal_rtc.c
 *
 *  Created on: Jan 11, 2022
 *      Author: Rodolfo
 */

#include "principal.h"

void Principal_RTC_Calibrate(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime)
{
	sDate->Year = 22;
	sDate->Month = 01;
	sDate->Date = 11;
	sTime->Hours = 00;
	sTime->Minutes = 00;
	sTime->Seconds = 00;
	sDate->WeekDay = RTC_WEEKDAY_MONDAY;

	//Standard Configuration
	sTime->DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime->StoreOperation = RTC_STOREOPERATION_RESET;
	sTime->TimeFormat = RTC_HOURFORMAT_24;

	HAL_RTC_SetDate(&hrtc, sDate, RTC_FORMAT_BIN);
	HAL_RTC_SetTime(&hrtc, sTime, RTC_FORMAT_BIN);

	return;
}

void Principal_RTC_Get_Date(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime)
{
	HAL_RTC_GetDate(&hrtc, sDate, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&hrtc, sTime, RTC_FORMAT_BIN);
}
