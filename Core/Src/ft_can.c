/*
 * ft_can.c
 *
 *  Created on: 27 de set de 2020
 *      Author: Rodolfo
 */

#include "ft_can.h"

static void FT_CAN_ProcessData(FT_Data* FT_Data_Struct);

HAL_StatusTypeDef FT_CAN_FilterConfig(CAN_HandleTypeDef *hcan, uint16_t FT_Product, uint8_t filter_bank_position, uint32_t Filter_FIFO)
{
	CAN_FilterTypeDef sFilterConfig;
	uint32_t filter_id = FT_Product << 19, mask_id = 0x1FF80000;

	sFilterConfig.FilterBank = filter_bank_position;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = filter_id >> 13;
	sFilterConfig.FilterIdLow = (filter_id << 3) & 0xFFF8;
	sFilterConfig.FilterMaskIdHigh = mask_id >> 13;
	sFilterConfig.FilterMaskIdLow = (mask_id << 3) & 0xFFF8;
	sFilterConfig.FilterFIFOAssignment = Filter_FIFO;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14 + filter_bank_position;

	return HAL_CAN_ConfigFilter(hcan, &sFilterConfig);
}

void FT_CAN_ReceiveData(CAN_RxHeaderTypeDef* pRxHeader, uint8_t* pData, FT_Data* FT_Data_Struct)
{
	uint8_t buffer[8];
	uint8_t DLC = pRxHeader->DLC;
	uint32_t ExtId = pRxHeader->ExtId >> 19;

	if(	   (ExtId != FT_Gear_Controller)
		&& (ExtId != FT_Knock_Meter)
		&& (ExtId != FT_Boost_Controller2)
		&& (ExtId != FT_Injector_Driver)
		&& (ExtId != FT_WBO2_Nano)
		&& (ExtId != FT_WBO2_Slim)
		&& (ExtId != FT_Alcohol_O2)
		&& (ExtId != FTSpark)
		&& (ExtId != FT_Switchpad)
		&& (ExtId != FT500)
		&& (ExtId != FT_Power_ECU)
		&& (pRxHeader->IDE != CAN_ID_EXT))
		return;

	ExtId = pRxHeader->ExtId;

	for(uint8_t i = 0; i < DLC; i++)
		buffer[i] = pData[i];

	if((((ExtId / 0x800) & 0x07) == 0x00))// || (((ExtId / 0x800) & 0x07) == 0x01))
	{
		if((ExtId & 0xFFF) == 0x600)
		{
			FT_Data_Struct->tps  = buffer[0] << 8;
			FT_Data_Struct->tps |= buffer[1];
			FT_Data_Struct->map  = buffer[2] << 8;
			FT_Data_Struct->map |= buffer[3];
			FT_Data_Struct->iat  = buffer[4] << 8;
			FT_Data_Struct->iat |= buffer[5];
			FT_Data_Struct->ect  = buffer[6] << 8;
			FT_Data_Struct->ect |= buffer[7];

			return;
		}

		else if((ExtId & 0xFFF) == 0x601)
		{
			FT_Data_Struct->oil_pressure  	  = buffer[0] << 8;
			FT_Data_Struct->oil_pressure 	 |= buffer[1];
			FT_Data_Struct->fuel_pressure 	  = buffer[2] << 8;
			FT_Data_Struct->fuel_pressure 	 |= buffer[3];
			FT_Data_Struct->coolant_pressure  = buffer[4] << 8;
			FT_Data_Struct->coolant_pressure |= buffer[5];
			FT_Data_Struct->gear  			  = buffer[6] << 8;
			FT_Data_Struct->gear 			 |= buffer[7];

			return;
		}

		else if((ExtId & 0xFFF) == 0x602)
		{
			FT_Data_Struct->lambda  		 = buffer[0] << 8;
			FT_Data_Struct->lambda 			|= buffer[1];
			FT_Data_Struct->rpm  			 = buffer[2] << 8;
			FT_Data_Struct->rpm 			|= buffer[3];
			FT_Data_Struct->oil_temperature  = buffer[4] << 8;
			FT_Data_Struct->oil_temperature |= buffer[5];
			FT_Data_Struct->pit_limiter  	 = buffer[6] << 8;
			FT_Data_Struct->pit_limiter 	|= buffer[7];

			return;
		}

		else if((ExtId & 0xFFF) == 0x603)
		{
			FT_Data_Struct->wheel_speed_fr	 = buffer[0] << 8;
			FT_Data_Struct->wheel_speed_fr	|= buffer[1];
			FT_Data_Struct->wheel_speed_fl	 = buffer[2] << 8;
			FT_Data_Struct->wheel_speed_fl	|= buffer[3];
			FT_Data_Struct->wheel_speed_rr	 = buffer[4] << 8;
			FT_Data_Struct->wheel_speed_rr	|= buffer[5];
			FT_Data_Struct->wheel_speed_rl 	 = buffer[6] << 8;
			FT_Data_Struct->wheel_speed_rl 	|= buffer[7];

			return;
		}

		else if((ExtId & 0xFFF) == 0x606)
		{
			FT_Data_Struct->accel_long		 = buffer[0] << 8;
			FT_Data_Struct->accel_long		|= buffer[1];
			FT_Data_Struct->accel_lat		 = buffer[2] << 8;
			FT_Data_Struct->accel_lat		|= buffer[3];
			FT_Data_Struct->yaw_rate_pitch	 = buffer[4] << 8;
			FT_Data_Struct->yaw_rate_pitch	|= buffer[5];
			FT_Data_Struct->yaw_rate_roll	 = buffer[6] << 8;
			FT_Data_Struct->yaw_rate_roll	|= buffer[7];

			return;
		}

		else if((ExtId & 0xFFF) == 0x607)
		{
			FT_Data_Struct->lambda_correction  = buffer[0] << 8;
			FT_Data_Struct->lambda_correction |= buffer[1];
			FT_Data_Struct->fuel_flow_total	   = buffer[2] << 8;
			FT_Data_Struct->fuel_flow_total	  |= buffer[3];

			return;
		}

#ifndef FT_CAN_FIXED
		else if((ExtId & 0xFF) == 0xFF)
		{
			FT_Data_Struct->data_id  	  = buffer[0] << 8;
			FT_Data_Struct->data_id 	 |= buffer[1];
			FT_Data_Struct->data_content  = buffer[2] << 8;
			FT_Data_Struct->data_content |= buffer[3];

			FT_CAN_ProcessData(FT_Data_Struct);

			if(DLC == 8)
			{
				FT_Data_Struct->data_id  	  = buffer[4] << 8;
				FT_Data_Struct->data_id 	 |= buffer[5];
				FT_Data_Struct->data_content  = buffer[6] << 8;
				FT_Data_Struct->data_content |= buffer[7];

				FT_CAN_ProcessData(FT_Data_Struct);
			}

			return;
		}
	}

	else if((((ExtId / 0x800) & 0x07) == 0x02))// || ((((ExtId / 0x800) & 0x07) == 0x03)))
	{
		if(buffer[0] == 0xFF)
		{
			FT_Data_Struct->data_id  	  = buffer[1] << 8;
			FT_Data_Struct->data_id 	 |= buffer[2];
			FT_Data_Struct->data_content  = buffer[3] << 8;
			FT_Data_Struct->data_content |= buffer[4];
			FT_CAN_ProcessData(FT_Data_Struct);

			return;
		}

		//Segmented Messages
		else
		{
			//First Message
			if(buffer[0] == 0)
			{
				FT_Data_Struct->data_length   = buffer[1] << 8;
				FT_Data_Struct->data_length  |= buffer[2];

				FT_Data_Struct->data_id  	  = buffer[3] << 8;
				FT_Data_Struct->data_id 	 |= buffer[4];
				FT_Data_Struct->data_content  = buffer[5] << 8;
				FT_Data_Struct->data_content |= buffer[6];
				FT_CAN_ProcessData(FT_Data_Struct);

				FT_Data_Struct->data_id = buffer[7] << 8;
				FT_Data_Struct->data_last_seg = 0;

				return;
			}
			//Messages 1, 5, etc
			else if((buffer[0] % 4) == 1)
			{
				if(buffer[0] == (FT_Data_Struct->data_last_seg + 1))
				{
					FT_Data_Struct->data_id 	 |= buffer[1];
					FT_Data_Struct->data_content  = buffer[2] << 8;
					FT_Data_Struct->data_content |= buffer[3];
					FT_CAN_ProcessData(FT_Data_Struct);
				}

				if(DLC == 8)
				{
					FT_Data_Struct->data_id  	  = buffer[4] << 8;
					FT_Data_Struct->data_id 	 |= buffer[5];
					FT_Data_Struct->data_content  = buffer[6] << 8;
					FT_Data_Struct->data_content |= buffer[7];
					FT_CAN_ProcessData(FT_Data_Struct);
				}

				FT_Data_Struct->data_last_seg = buffer[0];

				return;
			}
			//messages 2, 6, etc
			else if((buffer[0] % 4) == 2)
			{
				FT_Data_Struct->data_id  	  = buffer[1] << 8;
				FT_Data_Struct->data_id 	 |= buffer[2];
				FT_Data_Struct->data_content  = buffer[3] << 8;
				FT_Data_Struct->data_content |= buffer[4];
				FT_CAN_ProcessData(FT_Data_Struct);

				if(DLC == 8)
				{
					FT_Data_Struct->data_id  	 = buffer[5] << 8;
					FT_Data_Struct->data_id 	|= buffer[6];
					FT_Data_Struct->data_content = buffer[7] << 8;
				}

				FT_Data_Struct->data_last_seg = buffer[0];

				return;
			}
			//Messages 3, 7, etc
			else if((buffer[0] % 4) == 3)
			{
				if(buffer[0] == (FT_Data_Struct->data_last_seg + 1))
				{
					FT_Data_Struct->data_content |= buffer[1];
					FT_CAN_ProcessData(FT_Data_Struct);
				}

				if(DLC >= 6)
				{
					FT_Data_Struct->data_id  	  = buffer[2] << 8;
					FT_Data_Struct->data_id 	 |= buffer[3];
					FT_Data_Struct->data_content  = buffer[4] << 8;
					FT_Data_Struct->data_content |= buffer[5];
					FT_CAN_ProcessData(FT_Data_Struct);
				}

				if(DLC == 8)
				{
					FT_Data_Struct->data_id  = buffer[6] << 8;
					FT_Data_Struct->data_id |= buffer[7];
				}

				FT_Data_Struct->data_last_seg = buffer[0];

				return;
			}
			//Messages 4, 8, etc
			else if((buffer[0] % 4) == 0)
			{
				if(buffer[0] == (FT_Data_Struct->data_last_seg + 1))
				{
					FT_Data_Struct->data_content  = buffer[1] << 8;
					FT_Data_Struct->data_content |= buffer[2];
					FT_CAN_ProcessData(FT_Data_Struct);
				}

				if(DLC >= 7)
				{
					FT_Data_Struct->data_id  	  = buffer[3] << 8;
					FT_Data_Struct->data_id 	 |= buffer[4];
					FT_Data_Struct->data_content  = buffer[5] << 8;
					FT_Data_Struct->data_content |= buffer[6];
					FT_CAN_ProcessData(FT_Data_Struct);
				}

				if(DLC == 8)
					FT_Data_Struct->data_id	= buffer[7] << 8;

				FT_Data_Struct->data_last_seg = buffer[0];

				return;
			}
		}
#endif
	}
}

#ifndef FT_CAN_FIXED

static void FT_CAN_ProcessData(FT_Data* FT_Data_Struct)
{
	uint16_t id = FT_Data_Struct->data_id;
	uint16_t data = FT_Data_Struct->data_content;

	if((id & 0x1) == 0x1)
		return;


	switch(id >> 1)
	{
		case 0x0001: //TPS
			FT_Data_Struct->tps = data;
			break;
		case 0x0002: //MAP
			FT_Data_Struct->map = data;
			break;
		case 0x0003: //IAT
			FT_Data_Struct->iat = data;
			break;
		case 0x0004: //ECT
			FT_Data_Struct->ect = data;
			break;
		case 0x0005: //Oil Pressure
			FT_Data_Struct->oil_pressure = data;
			break;
		case 0x0006: //Fuel Pressure
			FT_Data_Struct->fuel_pressure = data;
			break;
		case 0x0007: //Coolant Pressure
			FT_Data_Struct->coolant_pressure = data;
			break;
		case 0x0011: //Gear
			FT_Data_Struct->gear = data;
			break;
		case 0x0027: //Exhaust Lambda
			FT_Data_Struct->lambda = data;
			break;
		case 0x0042: //RPM
			FT_Data_Struct->rpm = data;
			break;
		case 0x008C: //Oil Temperature
			FT_Data_Struct->oil_temperature = data;
			break;
		case 0x008D: //Pit Limit Switch
			FT_Data_Struct->pit_limiter = data;
			break;
		case 0x0150: //Total Fuel Flow
			FT_Data_Struct->fuel_flow_total = data;
			break;

#ifdef FT_CAN_LAMBDA
		case 0x0012: //Disabled Lambda
			FT_Data_Struct->lambda_disabled = data;
			break;
		case 0x0013: //Cylinder 1 Lambda
			FT_Data_Struct->cylinder_1_lambda = data;
			break;
		case 0x0014: //Cylinder 2 Lambda
			FT_Data_Struct->cylinder_2_lambda = data;
			break;
		case 0x0015: //Cylinder 3 Lambda
			FT_Data_Struct->cylinder_3_lambda = data;
			break;
		case 0x0016: //Cylinder 4 Lambda
			FT_Data_Struct->cylinder_4_lambda = data;
			break;
		case 0x0017: //Cylinder 5 Lambda
			FT_Data_Struct->cylinder_5_lambda = data;
			break;
		case 0x0018: //Cylinder 6 Lambda
			FT_Data_Struct->cylinder_6_lambda = data;
			break;
		case 0x0019: //Cylinder 7 Lambda
			FT_Data_Struct->cylinder_7_lambda = data;
			break;
		case 0x001A: //Cylinder 8 Lambda
			FT_Data_Struct->cylinder_8_lambda = data;
			break;
		case 0x001B: //Cylinder 9 Lambda
			FT_Data_Struct->cylinder_9_lambda = data;
			break;
		case 0x001C: //Cylinder 10 Lambda
			FT_Data_Struct->cylinder_10_lambda = data;
			break;
		case 0x001D: //Cylinder 11 Lambda
			FT_Data_Struct->cylinder_11_lambda = data;
			break;
		case 0x001E: //Cylinder 12 Lambda
			FT_Data_Struct->cylinder_12_lambda = data;
			break;
		case 0x001F: //Cylinder 13 Lambda
			FT_Data_Struct->cylinder_13_lambda = data;
			break;
		case 0x0020: //Cylinder 14 Lambda
			FT_Data_Struct->cylinder_14_lambda = data;
			break;
		case 0x0021: //Cylinder 15 Lambda
			FT_Data_Struct->cylinder_15_lambda = data;
			break;
		case 0x0022: //Cylinder 16 Lambda
			FT_Data_Struct->cylinder_16_lambda = data;
			break;
		case 0x0023: //Cylinder 17 Lambda
			FT_Data_Struct->cylinder_17_lambda = data;
			break;
		case 0x0024: //Cylinder 18 Lambda
			FT_Data_Struct->cylinder_18_lambda = data;
			break;
		case 0x0025: //Left Bank Lambda
			FT_Data_Struct->left_bank_lambda = data;
			break;
		case 0x0026: //Right Bank Lambda
			FT_Data_Struct->right_bank_lambda = data;
			break;
#endif

#ifdef FT_CAN_EGT
		case 0x0028: //Disabled EGT
			FT_Data_Struct->disabled_egt = data;
			break;
		case 0x0029: //Cylinder 1 EGT
			FT_Data_Struct->cylinder_1_egt = data;
			break;
		case 0x002A: //Cylinder 2 EGT
			FT_Data_Struct->cylinder_2_egt = data;
			break;
		case 0x002B: //Cylinder 3 EGT
			FT_Data_Struct->cylinder_3_egt = data;
			break;
		case 0x002C: //Cylinder 4 EGT
			FT_Data_Struct->cylinder_4_egt = data;
			break;
		case 0x002D: //Cylinder 5 EGT
			FT_Data_Struct->cylinder_5_egt = data;
			break;
		case 0x002E: //Cylinder 6 EGT
			FT_Data_Struct->cylinder_6_egt = data;
			break;
		case 0x002F: //Cylinder 7 EGT
			FT_Data_Struct->cylinder_7_egt = data;
			break;
		case 0x0030: //Cylinder 8 EGT
			FT_Data_Struct->cylinder_8_egt = data;
			break;
		case 0x0031: //Cylinder 9 EGT
			FT_Data_Struct->cylinder_9_egt = data;
			break;
		case 0x0032: //Cylinder 10 EGT
			FT_Data_Struct->cylinder_10_egt = data;
			break;
		case 0x0033: //Cylinder 11 EGT
			FT_Data_Struct->cylinder_11_egt = data;
			break;
		case 0x0034: //Cylinder 12 EGT
			FT_Data_Struct->cylinder_12_egt = data;
			break;
		case 0x0035: //Cylinder 13 EGT
			FT_Data_Struct->cylinder_13_egt = data;
			break;
		case 0x0036: //Cylinder 14 EGT
			FT_Data_Struct->cylinder_14_egt = data;
			break;
		case 0x0037: //Cylinder 15 EGT
			FT_Data_Struct->cylinder_15_egt = data;
			break;
		case 0x0038: //Cylinder 16 EGT
			FT_Data_Struct->cylinder_16_egt = data;
			break;
		case 0x0039: //Cylinder 17 EGT
			FT_Data_Struct->cylinder_17_egt = data;
			break;
		case 0x003A: //Cylinder 18 EGT
			FT_Data_Struct->cylinder_18_egt = data;
			break;
		case 0x003B: //Left Bank EGT
			FT_Data_Struct->left_bank_egt = data;
			break;
		case 0x003C: //Right Bank EGT
			FT_Data_Struct->right_bank_egt = data;
			break;
		case 0x003D: //Exhaust EGT
			FT_Data_Struct->exhaust_egt = data;
			break;
#endif

#ifdef FT_CAN_TIMING
		case 0x0043: //Injection Bank A Timing
			FT_Data_Struct->injection_bank_a_time = data;
			break;
		case 0x0044: //Injection Bank B Timing
			FT_Data_Struct->injection_bank_b_time = data;
			break;
		case 0x0045: //Injection Bank A Duty Cycle
			FT_Data_Struct->injection_bank_a_duty_cycle = data;
			break;
		case 0x0046: //Injection Bank B Duty Cycle
			FT_Data_Struct->injection_bank_b_duty_cycle = data;
			break;
		case 0x0047: //Ignition Advance
			FT_Data_Struct->ignition_advance = data;
			break;
#endif

#ifdef FT_CAN_EXTERNAL_IGNITION
		case 0x0052: //Ignition 1 Output
			FT_Data_Struct->ext_ignition_1_discharge_time = data;
			break;
		case 0x0053: //Ignition 2 Output
			FT_Data_Struct->ext_ignition_2_discharge_time = data;
			break;
		case 0x0054: //Ignition 3 Output
			FT_Data_Struct->ext_ignition_3_discharge_time = data;
			break;
		case 0x0055: //Ignition 4 Output
			FT_Data_Struct->ext_ignition_4_discharge_time = data;
			break;
		case 0x0056: //Ignition 5 Output
			FT_Data_Struct->ext_ignition_5_discharge_time = data;
			break;
		case 0x0057: //Ignition 6 Output
			FT_Data_Struct->ext_ignition_6_discharge_time = data;
			break;
		case 0x0058: //Ignition 7 Output
			FT_Data_Struct->ext_ignition_7_discharge_time = data;
			break;
		case 0x0059: //Ignition 8 Output
			FT_Data_Struct->ext_ignition_8_discharge_time = data;
			break;
		case 0x005A: //Ignition 9 Output
			FT_Data_Struct->ext_ignition_9_discharge_time = data;
			break;
		case 0x005B: //Ignition 10 Output
			FT_Data_Struct->ext_ignition_10_discharge_time = data;
			break;
		case 0x005C: //Ignition 11 Output
			FT_Data_Struct->ext_ignition_11_discharge_time = data;
			break;
		case 0x005D: //Ignition 12 Output
			FT_Data_Struct->ext_ignition_12_discharge_time = data;
			break;
		case 0x005E: //Ignition 13 Output
			FT_Data_Struct->ext_ignition_13_discharge_time = data;
			break;
		case 0x005F: //Ignition 14 Output
			FT_Data_Struct->ext_ignition_14_discharge_time = data;
			break;
		case 0x0060: //Ignition 15 Output
			FT_Data_Struct->ext_ignition_15_discharge_time = data;
			break;
		case 0x0061: //Ignition 16 Output
			FT_Data_Struct->ext_ignition_16_discharge_time = data;
			break;
		case 0x0062: //Ignition Power Supply
			FT_Data_Struct->ext_ignition_power_supply = data;
			break;
		case 0x0063: //Ignition Power Supply Drop
			FT_Data_Struct->ext_ignition_power_supply_drop = data;
			break;
		case 0x0064: //Ignition Power Level
			FT_Data_Struct->ext_ignition_power_level = data;
			break;
		case 0x0065: //Ignition Temperature
			FT_Data_Struct->ext_ignition_temperature = data;
			break;
		case 0x0066: //Ignition Capacitor 1 Charge
			FT_Data_Struct->ext_ignition_capacitor_1_charge = data;
			break;
		case 0x0067: //Ignition Capacitor 2 Charge
			FT_Data_Struct->ext_ignition_capacitor_2_charge = data;
			break;
		case 0x0068: //Ignition Capacitor 3 Charge
			FT_Data_Struct->ext_ignition_capacitor_3_charge = data;
			break;
		case 0x0069: //Ignition Capacitor 4 Charge
			FT_Data_Struct->ext_ignition_capacitor_4_charge = data;
			break;
		case 0x006A: //Ignition Capacitor 1 Charge Time
			FT_Data_Struct->ext_ignition_capacitor_1_charge_time = data;
			break;
		case 0x006B: //Ignition Capacitor 2 Charge Time
			FT_Data_Struct->ext_ignition_capacitor_2_charge_time = data;
			break;
		case 0x006C: //Ignition Capacitor 3 Charge Time
			FT_Data_Struct->ext_ignition_capacitor_3_charge_time = data;
			break;
		case 0x006D: //Ignition Capacitor 4 Charge Time
			FT_Data_Struct->ext_ignition_capacitor_4_charge_time = data;
			break;
		case 0x006E: //Ignition Error Code
			FT_Data_Struct->ext_ignition_error_code = data;
			break;
		case 0x006F: //Ignition no Load Outputs
			FT_Data_Struct->ext_ignition_no_load_outputs = data;
			break;
		case 0x0070: //Ignition Partial Discharge Outputs
			FT_Data_Struct->ext_ignition_partial_discharge_outputs = data;
			break;
		case 0x0071: //Ignition Damaged Outputs
			FT_Data_Struct->ext_ignition_damaged_outputs = data;
			break;
		case 0x0072: //Ignition Disabled Outputs
			FT_Data_Struct->ext_ignition_disabled_outputs = data;
			break;
		case 0x0073: //Ignition Operation Status
			FT_Data_Struct->ext_ignition_operation_status = data;
			break;
		case 0x0074: //Ignition Power Level Configuration
			FT_Data_Struct->ext_ignition_power_level_config = data;
			break;
		case 0x0116: //Ignition Switch Voltage
			FT_Data_Struct->ext_ignition_switch_voltage = data;
			break;
		case 0x0117: //Ignition CPU Supply Voltage
			FT_Data_Struct->ext_ignition_cpu_supply_voltage = data;
			break;
		case 0x0118: //Ignition CPU Temperature
			FT_Data_Struct->ext_ignition_cpu_temperature = data;
			break;
		case 0x0119: //Ignition Operation Time
			FT_Data_Struct->ext_ignition_operation_time = data;
			break;
#endif

#ifdef FT_CAN_INJECTION
		case 0x011F: //Bank A Injector 1 Duty Cycle
			FT_Data_Struct->bank_a_injector_1_duty_cycle = data;
			break;
		case 0x0120: //Bank A Injector 2 Duty Cycle
			FT_Data_Struct->bank_a_injector_2_duty_cycle = data;
			break;
		case 0x0121: //Bank A Injector 3 Duty Cycle
			FT_Data_Struct->bank_a_injector_3_duty_cycle = data;
			break;
		case 0x0122: //Bank A Injector 4 Duty Cycle
			FT_Data_Struct->bank_a_injector_4_duty_cycle = data;
			break;
		case 0x0123: //Bank A Injector 5 Duty Cycle
			FT_Data_Struct->bank_a_injector_5_duty_cycle = data;
			break;
		case 0x0124: //Bank A Injector 6 Duty Cycle
			FT_Data_Struct->bank_a_injector_6_duty_cycle = data;
			break;
		case 0x0125: //Bank A Injector 7 Duty Cycle
			FT_Data_Struct->bank_a_injector_7_duty_cycle = data;
			break;
		case 0x0126: //Bank A Injector 8 Duty Cycle
			FT_Data_Struct->bank_a_injector_8_duty_cycle = data;
			break;
		case 0x0127: //Bank A Injector 9 Duty Cycle
			FT_Data_Struct->bank_a_injector_9_duty_cycle = data;
			break;
		case 0x0128: //Bank A Injector 10 Duty Cycle
			FT_Data_Struct->bank_a_injector_10_duty_cycle = data;
			break;
		case 0x0129: //Bank A Injector 11 Duty Cycle
			FT_Data_Struct->bank_a_injector_11_duty_cycle = data;
			break;
		case 0x012A: //Bank A Injector 12 Duty Cycle
			FT_Data_Struct->bank_a_injector_12_duty_cycle = data;
			break;
		case 0x012B: //Bank B Injector 1 Duty Cycle
			FT_Data_Struct->bank_b_injector_1_duty_cycle = data;
			break;
		case 0x012C: //Bank B Injector 2 Duty Cycle
			FT_Data_Struct->bank_b_injector_2_duty_cycle = data;
			break;
		case 0x012D: //Bank B Injector 3 Duty Cycle
			FT_Data_Struct->bank_b_injector_3_duty_cycle = data;
			break;
		case 0x012E: //Bank B Injector 4 Duty Cycle
			FT_Data_Struct->bank_b_injector_4_duty_cycle = data;
			break;
		case 0x012F: //Bank B Injector 5 Duty Cycle
			FT_Data_Struct->bank_b_injector_5_duty_cycle = data;
			break;
		case 0x0130: //Bank B Injector 6 Duty Cycle
			FT_Data_Struct->bank_b_injector_6_duty_cycle = data;
			break;
		case 0x0131: //Bank B Injector 7 Duty Cycle
			FT_Data_Struct->bank_b_injector_7_duty_cycle = data;
			break;
		case 0x0132: //Bank B Injector 8 Duty Cycle
			FT_Data_Struct->bank_b_injector_8_duty_cycle = data;
			break;
		case 0x0133: //Bank B Injector 9 Duty Cycle
			FT_Data_Struct->bank_b_injector_9_duty_cycle = data;
			break;
		case 0x0134: //Bank B Injector 10 Duty Cycle
			FT_Data_Struct->bank_b_injector_10_duty_cycle = data;
			break;
		case 0x0135: //Bank B Injector 11 Duty Cycle
			FT_Data_Struct->bank_b_injector_11_duty_cycle = data;
			break;
		case 0x0136: //Bank B Injector 12 Duty Cycle
			FT_Data_Struct->bank_b_injector_12_duty_cycle = data;
			break;
#endif

#ifdef FT_CAN_EXTRA
		case 0x0008: //Launch Mode
			FT_Data_Struct->launch_mode = data;
			break;
		case 0x0009: //Battery Voltage
			FT_Data_Struct->battery_voltage = data;
			break;
		case 0x000A: //Traction Speed
			FT_Data_Struct->traction_speed = data;
			break;
		case 0x000B: //Drag Speed
			FT_Data_Struct->drag_speed = data;
			break;
		case 0x000C: //Wheel Speed FL
			FT_Data_Struct->wheel_speed_fl = data;
			break;
		case 0x000D: //Wheel Speed FR
			FT_Data_Struct->wheel_speed_fr = data;
			break;
		case 0x000E: //Wheel Speed RL
			FT_Data_Struct->wheel_speed_rl = data;
			break;
		case 0x000F: //Wheel Speed RR
			FT_Data_Struct->wheel_speed_rr = data;
			break;
		case 0x0010: //Driveshaft RPM
			FT_Data_Struct->driveshaft_rpm = data;
			break;
		case 0x0048: //2-Step
			FT_Data_Struct->two_step_signal = data;
			break;
		case 0x0049: //3-Step
			FT_Data_Struct->three_step_signal = data;
			break;
		case 0x004A: //Burnout
			FT_Data_Struct->burnout_signal = data;
			break;
		case 0x004B: //Fuel Cut
			FT_Data_Struct->fuel_cut = data;
			break;
		case 0x004C: //Air Conditioning
			FT_Data_Struct->air_conditioning = data;
			break;
		case 0x004D: //ElectroFAN
			FT_Data_Struct->electro_fan = data;
			break;
		case 0x004E: //Gear Cut
			FT_Data_Struct->gear_cut = data;
			break;
		case 0x004F: //Gear Retard
			FT_Data_Struct->gear_retard = data;
			break;
		case 0x0050: //Gear Sensor Voltage
			FT_Data_Struct->gear_sensor_voltage = data;
			break;
		case 0x0051: //Lambda Meter (Average)
			FT_Data_Struct->lambda_average = data;
			break;
		case 0x0138: //Battery Temperature
			FT_Data_Struct->battery_temperature = data;
			break;
		case 0x014A: //Self Dial Time
			FT_Data_Struct->self_dial = data;
			break;
		case 0x0151: //Brake Pressure
			FT_Data_Struct->brake_pressure = data;
			break;
		case 0x0152: //Generic Outputs State
			FT_Data_Struct->generic_outputs_state = data;
			break;
		case 0x0153: //Day/Night State
			FT_Data_Struct->day_night_state = data;
			break;
	}
#endif

	FT_CAN_ProcessCustomData(id, data);

	return;
}

__weak void FT_CAN_ProcessCustomData(uint16_t id, uint16_t data)
{
	UNUSED(id);
	UNUSED(data);
}

#endif
