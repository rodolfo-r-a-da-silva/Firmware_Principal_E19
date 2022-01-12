/*
 * principal_rtc.c
 *
 *  Created on: Jan 11, 2022
 *      Author: Rodolfo
 */

#include "principal.h"

void Principal_RTC_Init(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime)
{
	sDate->Year = 0x22;
	sDate->Month = 0x01;
	sDate->Date = 0x11;
	sTime->Hours = 0x00;
	sTime->Minutes = 0x00;
	sTime->Seconds = 0x00;
	sDate->WeekDay = RTC_WEEKDAY_MONDAY;

	//Standard Configuration
	sTime->DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime->StoreOperation = RTC_STOREOPERATION_RESET;
	sTime->TimeFormat = RTC_HOURFORMAT_24;

	HAL_RTC_SetDate(&hrtc, sDate, RTC_FORMAT_BCD);
	HAL_RTC_SetTime(&hrtc, sTime, RTC_FORMAT_BCD);

	return;
}

void Principal_RTC_Get_Date(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime)
{
	HAL_RTC_GetDate(&hrtc, sDate, RTC_FORMAT_BCD);
	HAL_RTC_GetTime(&hrtc, sTime, RTC_FORMAT_BCD);
}
