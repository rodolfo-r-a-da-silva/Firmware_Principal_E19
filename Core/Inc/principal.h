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
#define CAN_CFG_FILTER	0x0000
#define CAN_CFG_MASK	0x0000
#define CAN_DAQ_FILTER	0x500
#define CAN_DAQ_MASK	0x700
#define CONFIG_ID		0
#define FIRST_ID		1
#define BEACON_ID		100
#define ECU_FIRST_ID	26
#define PDM_FIRST_ID	50

//CONFIGURATION
#define EEPROM_BUFFER_SIZE		11

//DATA
#define ADC_THRESHOLD		30
#define NBR_OF_CHANNELS		13
#define NBR_OF_MSGS			4
#define NBR_OF_LOCAL_MSGS	3

//DATALOGGER
#define BUTTON_COOLDOWN			500
#define DATALOGGER_BUFFER_SIZE	512
#define DATALOGGER_MSG_MAX_SIZE	13
#define DATALOGGER_SAVE_THR		DATALOGGER_BUFFER_SIZE - DATALOGGER_MSG_MAX_SIZE
#define DIRECTORY_STRING_SIZE	9
#define LOG_STRING_SIZE			50
#define RTC_STORE_DATA			0xE35C

//FREQUENCIES
#define MSG_FREQ_1HZ	1000
#define MSG_FREQ_2HZ	500
#define MSG_FREQ_5HZ	200
#define MSG_FREQ_10HZ	100
#define MSG_FREQ_20HZ	50
#define MSG_FREQ_25HZ	40
#define MSG_FREQ_50HZ	20
#define MSG_FREQ_100HZ	10
#define MSG_FREQ_200HZ	5
#define MSG_FREQ_250HZ	3
#define MSG_FREQ_500HZ	2

//INPUTS
#define INPUT_BEACON_PIN_0		0x00
#define INPUT_BEACON_PIN_1		0x01
#define INPUT_DATALOGGER_PIN_0	0x00
#define INPUT_DATALOGGER_PIN_1	0x02

//PINS
#define VBUS_PIN		GPIOA, GPIO_PIN_9
#define LED_OK			LED0_GPIO_Port, LED0_Pin
#define LED_DATALOGGER	LED1_GPIO_Port, LED1_Pin
#define LED_CAN_TX		LED2_GPIO_Port, LED2_Pin
#define LED_CAN_RX		LED3_GPIO_Port, LED3_Pin
/*END DEFINES*/

/*BEGIN MACROS*/
#define __LOAD_FREQ(__BUFFER__, __PER__)	\
	switch(__BUFFER__)						\
	{										\
		case 1:								\
			(__PER__) = MSG_FREQ_1HZ;		\
			break;							\
		case 2:								\
			(__PER__) = MSG_FREQ_2HZ;		\
			break;							\
		case 3:								\
			(__PER__) = MSG_FREQ_5HZ;		\
			break;							\
		case 4:								\
			(__PER__) = MSG_FREQ_10HZ;		\
			break;							\
		case 5:								\
			(__PER__) = MSG_FREQ_20HZ;		\
			break;							\
		case 6:								\
			(__PER__) = MSG_FREQ_25HZ;		\
			break;							\
		case 7:								\
			(__PER__) = MSG_FREQ_50HZ;		\
			break;							\
		case 8:								\
			(__PER__) = MSG_FREQ_100HZ;		\
			break;							\
		case 9:								\
			(__PER__) = MSG_FREQ_200HZ;		\
			break;							\
		case 10:							\
			(__PER__) = MSG_FREQ_250HZ;		\
			break;							\
		case 11:							\
			(__PER__) = MSG_FREQ_500HZ;		\
			break;							\
		default:							\
			(__PER__) = 0;					\
			break;							\
	}

#define __SAVE_FREQ(__BUFFER__, __PER__)	\
	switch(__PER__)							\
	{										\
		case MSG_FREQ_1HZ:					\
			(__BUFFER__) = 1;				\
			break;							\
		case MSG_FREQ_2HZ:					\
			(__BUFFER__) = 2;				\
			break;							\
		case MSG_FREQ_5HZ:					\
			(__BUFFER__) = 3;				\
			break;							\
		case MSG_FREQ_10HZ:					\
			(__BUFFER__) = 4;				\
			break;							\
		case MSG_FREQ_20HZ:					\
			(__BUFFER__) = 5;				\
			break;							\
		case MSG_FREQ_25HZ:					\
			(__BUFFER__) = 6;				\
			break;							\
		case MSG_FREQ_50HZ:					\
			(__BUFFER__) = 7;				\
			break;							\
		case MSG_FREQ_100HZ:				\
			(__BUFFER__) = 8;				\
			break;							\
		case MSG_FREQ_200HZ:				\
			(__BUFFER__) = 9;				\
			break;							\
		case MSG_FREQ_250HZ:				\
			(__BUFFER__) = 10;				\
			break;							\
		case MSG_FREQ_500HZ:				\
			(__BUFFER__) = 11;				\
			break;							\
		default:							\
			(__BUFFER__) = 0;				\
			break;							\
	}
/*END MACROS*/

/*BEGIN ENUMS*/
typedef enum{
	DL_No_Save = 0x00,
	DL_Save,
	DL_No_Card,
	DL_But_Press,
	DL_Error,
	DL_Test_Opt
}DL_Flag_TypeDef;

typedef enum{
	Analog_1_4 = 0x00,
	Analog_5_8,
	Analog_9_12,
	RTC_Msg,
	Verify_Msg,
	ECU_Save,
	PDM_Save,
	Beacon_Msg
}Message_TypeDef;

typedef enum{
	RTC_OK = 0x00,
	RTC_Lost
}RTC_Flag_TypeDef;
/*END ENUMS*/

/*BEGIN GLOBAL VARIABLES*/
//EXTERNAL VARIABLES
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern CAN_HandleTypeDef hcan1;
extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;
extern SD_HandleTypeDef hsd;

//CAN
uint8_t
	RxData[8],
	TxData[8];
uint32_t TxMailbox;
CAN_RxHeaderTypeDef RxHeader;
CAN_TxHeaderTypeDef TxHeader;

//DATA
uint8_t Lap_Number;
uint16_t ADC_Buffer[NBR_OF_CHANNELS];
uint32_t Lap_Time;
FT_Data ECU_Data;
PDM_Data PDM_Readings;

//DATALOGGER
uint8_t
	Datalogger_CD,
	Datalogger_Buffer[DATALOGGER_BUFFER_SIZE];
uint16_t Datalogger_Buffer_Position;
int16_t
	Threshold_RPM,
	Threshold_Speed;
DIR Dir_Struct;
FATFS Fatfs_Struct;
FIL File_Struct;
TCHAR Dir_String[DIRECTORY_STRING_SIZE];
TCHAR Log_String[LOG_STRING_SIZE];

//FLAGS
uint8_t
	Flag_CAN,
	Flag_USB;
DL_Flag_TypeDef Flag_Datalogger;
RTC_Flag_TypeDef Flag_RTC;

//INPUTS
uint8_t Input_Config;

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
	Acc_Datalogger[2],
	Acc_CAN[NBR_OF_MSGS],
	Acc_Msg[NBR_OF_MSGS + NBR_OF_LOCAL_MSGS],
	Acc_Lap;

//VERIFYS
uint8_t
	Verify_Datalogger,
	Verify_CAN;
uint16_t Verify;
/*END GLOBAL VARIABLES*/

/*BEGIN FUNCTION PROTOTYPES*/
//CAN
void Principal_CAN_Start(CAN_HandleTypeDef* hcan);

void Principal_Transmit_Msg(CAN_HandleTypeDef* hcan, uint8_t msg_number);

//CONFIGURATION
void Principal_Init(CAN_HandleTypeDef* hcan, I2C_HandleTypeDef* hi2c, TIM_HandleTypeDef* htim);

HAL_StatusTypeDef Principal_Receive_Config(I2C_HandleTypeDef* hi2c, uint8_t* data);

void Principal_Hard_Code_Config();

//DATALOGGER
FRESULT Principal_Datalogger_Init(FATFS* fatfs_struct);

FRESULT Principal_Datalogger_Start(RTC_DateTypeDef* sDate, RTC_TimeTypeDef* sTime, char* dir, char* file, DIR* dir_struct, FIL* file_struct);

FRESULT Principal_Datalogger_Finish(DIR* dir_struct, FIL* file_struct);

void Principal_Datalogger_Save_Buffer(uint32_t Data_ID, uint8_t Data_Length, uint8_t* Data_Buffer, FIL* file_struct);

void Principal_Datalogger_Button(DIR* dir_struct, FIL* file_struct);

void Principal_Card_Detection(FATFS* fatfs_struct, DIR* dir_struct, FIL* file_struct);

void Principal_Beacon_Detect();

//RTC
void Principal_RTC_Reg_Check(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime);

void Principal_RTC_Get_Date(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime);

void Principal_RTC_Set_Date(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime);
/*END FUNCTION PROTOTYPES*/

#endif /* INC_PRINCIPAL_H_ */
