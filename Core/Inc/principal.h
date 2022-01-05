/*
 * principal.h
 *
 *  Created on: 29 de dez de 2021
 *      Author: Rodolfp
 */

#ifndef INC_PRINCIPAL_H_
#define INC_PRINCIPAL_H_

#include "main.h"
#include "ft_can.h"

/*BEGIN DEFINES*/
//CAN
#define FIRST_ID 1

//DATALOGGER
#define DATALOGGER_BUFFER_SIZE	4096
#define DATALOGGER_MSG_MAX_SIZE	14

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
//CAN
uint8_t
	CAN_Rx_Data[8],
	CAN_Tx_Data[8];
uint32_t pTxMailbox;
CAN_RxHeaderTypeDef CAN_Rx_Header;
CAN_TxHeaderTypeDef CAN_Tx_Header;

//DATA
uint8_t Datalogger_Buffer[DATALOGGER_BUFFER_SIZE];
uint16_t ADC_Buffer[12];
FT_Data ECU_Data;

//FLAGS
uint8_t
	Flag_CAN,
	Flag_Datalogger,
	Flag_USB;

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
void Datalogger_Save_Buffer(uint32_t Data_ID, uint8_t Data_Length, uint8_t* Data_Buffer);
/*END FUNCTION PROTOTYPES*/

#endif /* INC_PRINCIPAL_H_ */
