/*
 * principal_variables.c
 *
 *  Created on: 9 de mar de 2022
 *      Author: Rodolfo
 */

#include "principal.h"

//CAN
uint8_t rxData[8];
uint8_t txData[8];
uint32_t txMailbox;
CAN_RxHeaderTypeDef rxHeader;
CAN_TxHeaderTypeDef txHeader;

//DATA
uint8_t lapNumber;
uint16_t adcBuffer[NBR_OF_CHANNELS];
uint32_t dataloggerWrite;
uint32_t lapTime;
FT_Data ecuData;
PDM_Data pdmReadings;

//FLAGS
uint8_t flagCAN;
USB_Flag_Typedef flagUSB;
DL_Flag_TypeDef flagDatalogger;
RTC_Flag_TypeDef flagRTC;

//INPUTS
uint8_t inputConfig;

//RTC
RTC_DateTypeDef rtcDate;
RTC_TimeTypeDef rtcTime;

//TIMING
uint16_t perCAN[NBR_OF_MSGS];
uint16_t perMsg[NBR_OF_MSGS + NBR_OF_LOCAL_MSGS];
uint32_t accDatalogger[3];
uint32_t accCAN[NBR_OF_MSGS];
uint32_t accMsg[NBR_OF_MSGS + NBR_OF_LOCAL_MSGS];
uint32_t accLap;

//VERIFYS
uint8_t	verifyCAN;
uint16_t verifyADC;
