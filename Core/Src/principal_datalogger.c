/*
 * principal_datalogger.c
 *
 *  Created on: Jan 5, 2022
 *      Author: Rodolfo
 */

#include "principal.h"

void Datalogger_Init()
{

}

void Datalogger_Save_Buffer(uint32_t Data_ID, uint8_t Data_Length, uint8_t* Data_Buffer)
{
	uint8_t buffer[5 + Data_Length];

	buffer[0] = 'D';
	buffer[1] = 'L';
	buffer[2] = Data_ID & 0xff;
	buffer[3] = Data_Length;
	buffer[4] = Acc_Datalogger;

	Acc_Datalogger = 0;

	for(uint8_t i = 0; i < Data_Length; i++)
		buffer[5 + i] = Data_Buffer[i];

//	if()
}
