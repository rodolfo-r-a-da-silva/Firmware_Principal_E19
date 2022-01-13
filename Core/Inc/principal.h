/*
 * principal.h
 *
 *  Created on: 29 de dez de 2021
 *      Author: Rodolfp
 */

#ifndef INC_PRINCIPAL_H_
#define INC_PRINCIPAL_H_

#include "main.h"
#include "fatfs.h"
#include "ff.h"
#include "ft_can.h"
#include "pdm_can.h"

/*BEGIN DEFINES*/
//CAN
#define FIRST_ID		1
#define ECU_FIRST_ID	26
#define PDM_FIRST_ID	50

//DATALOGGER
#define DATALOGGER_BUFFER_SIZE	4096
#define DATALOGGER_MSG_MAX_SIZE	14
#define DIRECTORY_STRING_SIZE	9
#define LOG_STRING_SIZE			DIRECTORY_STRING_SIZE * 2

//MESSAGES
#define NBR_OF_MSGS			4
#define NBR_OF_LOCAL_MSGS	2

//PINS
#define LED_OK			LED0_GPIO_Port, LED0_Pin
#define LED_DATALOGGER	LED1_GPIO_Port, LED1_Pin
#define LED_RX			LED3_GPIO_Port, LED3_Pin
#define LED_TX			LED2_GPIO_Port, LED2_Pin
/*END DEFINES*/

/*BEGIN GLOBAL VARIABLES*/
//EXTERNAL VARIABLES
RTC_HandleTypeDef hrtc;

//CAN
uint8_t
	CAN_Rx_Data[8],
	CAN_Tx_Data[8];
uint32_t pTxMailbox;
CAN_RxHeaderTypeDef CAN_Rx_Header;
CAN_TxHeaderTypeDef CAN_Tx_Header;

//DATA
uint8_t Datalogger_Buffer[DATALOGGER_BUFFER_SIZE];
uint16_t Datalogger_Buffer_Position;
uint16_t ADC_Buffer[12];
FT_Data ECU_Data;
PDM_Data PDM_Readings;
DIR Dir_Struct;
FATFS Fatfs_Struct;
FIL File_Struct;
TCHAR Dir_String[DIRECTORY_STRING_SIZE];
TCHAR Log_String[LOG_STRING_SIZE];

//FLAGS
uint8_t
	Flag_CAN,
	Flag_Datalogger,
	Flag_USB;

//RTC
RTC_DateTypeDef Date;
RTC_TimeTypeDef Time;

//TIMING
uint8_t
	Freq_CAN[NBR_OF_MSGS],
	Freq_Msg[NBR_OF_MSGS + NBR_OF_LOCAL_MSGS];
uint16_t
	Per_CAN[NBR_OF_MSGS],
	Per_Msg[NBR_OF_MSGS + NBR_OF_LOCAL_MSGS];
uint32_t
	Acc_Datalogger,
	Acc_CAN[NBR_OF_MSGS],
	Acc_Msg[NBR_OF_MSGS + NBR_OF_LOCAL_MSGS];

//VERIFYS
uint8_t
	Verify_Datalogger,
	Verify_CAN[2],
	Verify_Data[12];
/*END GLOBAL VARIABLES*/

/*BEGIN FUNCTION PROTOTYPES*/
//CAN
void Principal_Transmit_Msg(CAN_HandleTypeDef* hcan, uint8_t msg_number);

//DATALOGGER
FRESULT Principal_Datalogger_Init(FATFS* fatfs_struct);

FRESULT Principal_Datalogger_Start(RTC_DateTypeDef* sDate, RTC_TimeTypeDef* sTime, char* dir, char* file, DIR* dir_struct, FIL* file_struct);

FRESULT Principal_Datalogger_Finish(DIR* dir_struct, FIL* file_struct);

void Principal_Datalogger_Save_Buffer(uint32_t Data_ID, uint8_t Data_Length, uint8_t* Data_Buffer, FIL* file_struct);
//RTC
void Principal_RTC_Init(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime);

void Principal_RTC_Get_Date(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime);
/*END FUNCTION PROTOTYPES*/

#endif /* INC_PRINCIPAL_H_ */
