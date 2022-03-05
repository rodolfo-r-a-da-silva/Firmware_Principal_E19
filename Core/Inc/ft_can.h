/*
 * ft600_can.h
 *
 *  Created on: 27 de set de 2020
 *      Author: Rodolfo
 */

#ifndef INC_FT_CAN_H_
#define INC_FT_CAN_H_

#include "main.h"

//#define FT_CAN_SIMPLE

#ifndef FT_CAN_SIMPLE

//#define FT_CAN_LAMBDA
//#define FT_CAN_EGT
#define FT_CAN_TIMING
#define FT_CAN_EXTRA
//#define FT_CAN_EXTERNAL_IGNITION
//#define  FT_CAN_INJECTION

#endif

typedef enum{
	FT_Gear_Controller 		= 0x0140,
	FT_Knock_Meter 			= 0x0141,
	FT_Boost_Controller2 	= 0x0142,
	FT_Injector_Driver		= 0x0150,
	FT_WBO2_Nano			= 0x0240,
	FT_WBO2_Slim			= 0x0241,
	FT_Alcohol_O2			= 0x0242,
	FTSpark					= 0x0243,
	FT_Switchpad			= 0x0244,
	FT500					= 0x0280,
	FT600					= 0x0281
}Product_IDs;

typedef struct{

#ifdef FT_CAN_LAMBDA
	int16_t cylinder_1_lambda;		//*0.001	(Lambda)
	int16_t cylinder_2_lambda;		//*0.001	(Lambda)
	int16_t cylinder_3_lambda;		//*0.001	(Lambda)
	int16_t cylinder_4_lambda;		//*0.001	(Lambda)
	int16_t cylinder_5_lambda;		//*0.001	(Lambda)
	int16_t cylinder_6_lambda;		//*0.001	(Lambda)
	int16_t cylinder_7_lambda;		//*0.001	(Lambda)
	int16_t cylinder_8_lambda;		//*0.001	(Lambda)
	int16_t cylinder_9_lambda;		//*0.001	(Lambda)
	int16_t cylinder_10_lambda;		//*0.001	(Lambda)
	int16_t cylinder_11_lambda;		//*0.001	(Lambda)
	int16_t cylinder_12_lambda;		//*0.001	(Lambda)
	int16_t cylinder_13_lambda;		//*0.001	(Lambda)
	int16_t cylinder_14_lambda;		//*0.001	(Lambda)
	int16_t cylinder_15_lambda;		//*0.001	(Lambda)
	int16_t cylinder_16_lambda;		//*0.001	(Lambda)
	int16_t cylinder_17_lambda;		//*0.001	(Lambda)
	int16_t cylinder_18_lambda;		//*0.001	(Lambda)
	int16_t lambda_disabled;		//*0.001	(Lambda)
	int16_t left_bank_lambda;		//*0.001	(Lambda)
	int16_t right_bank_lambda;		//*0.001	(Lambda)
#endif

#ifdef FT_CAN_EGT
	int16_t disabled_egt;			//*0.1		(°C)
	int16_t cylinder_1_egt;			//*0.1		(°C)
	int16_t cylinder_2_egt;			//*0.1		(°C)
	int16_t cylinder_3_egt;			//*0.1		(°C)
	int16_t cylinder_4_egt;			//*0.1		(°C)
	int16_t cylinder_5_egt;			//*0.1		(°C)
	int16_t cylinder_6_egt;			//*0.1		(°C)
	int16_t cylinder_7_egt;			//*0.1		(°C)
	int16_t cylinder_8_egt;			//*0.1		(°C)
	int16_t cylinder_9_egt;			//*0.1		(°C)
	int16_t cylinder_10_egt;		//*0.1		(°C)
	int16_t cylinder_11_egt;		//*0.1		(°C)
	int16_t cylinder_12_egt;		//*0.1		(°C)
	int16_t cylinder_13_egt;		//*0.1		(°C)
	int16_t cylinder_14_egt;		//*0.1		(°C)
	int16_t cylinder_15_egt;		//*0.1		(°C)
	int16_t cylinder_16_egt;		//*0.1		(°C)
	int16_t cylinder_17_egt;		//*0.1		(°C)
	int16_t cylinder_18_egt;		//*0.1		(°C)
	int16_t left_bank_egt;			//*0.1		(°C)
	int16_t right_bank_egt;			//*0.1		(°C)
	int16_t exhaust_egt;			//*0.1		(°C)
#endif

#ifdef FT_CAN_TIMING
	int16_t ignition_advance;				//*0.1		(°)
	int16_t injection_bank_a_time;			//*0.01		(ms)
	int16_t injection_bank_b_time;			//*0.01		(ms)
	int16_t injection_bank_a_duty_cycle;	//*0.1		(%)
	int16_t injection_bank_b_duty_cycle;	//*0.1		(%)
#endif

#ifdef FT_CAN_EXTERNAL_IGNITION
	int16_t ext_ignition_1_discharge_time;			//*1		(us)
	int16_t ext_ignition_2_discharge_time;			//*1		(us)
	int16_t ext_ignition_3_discharge_time;			//*1		(us)
	int16_t ext_ignition_4_discharge_time;			//*1		(us)
	int16_t ext_ignition_5_discharge_time;			//*1		(us)
	int16_t ext_ignition_6_discharge_time;			//*1		(us)
	int16_t ext_ignition_7_discharge_time;			//*1		(us)
	int16_t ext_ignition_8_discharge_time;			//*1		(us)
	int16_t ext_ignition_9_discharge_time;			//*1		(us)
	int16_t ext_ignition_10_discharge_time;			//*1		(us)
	int16_t ext_ignition_11_discharge_time;			//*1		(us)
	int16_t ext_ignition_12_discharge_time;			//*1		(us)
	int16_t ext_ignition_13_discharge_time;			//*1		(us)
	int16_t ext_ignition_14_discharge_time;			//*1		(us)
	int16_t ext_ignition_15_discharge_time;			//*1		(us)
	int16_t ext_ignition_16_discharge_time;			//*1		(us)
	int16_t ext_ignition_capacitor_1_charge;		//*0.1		(V)
	int16_t ext_ignition_capacitor_2_charge;		//*0.1		(V)
	int16_t ext_ignition_capacitor_3_charge;		//*0.1		(V)
	int16_t ext_ignition_capacitor_4_charge;		//*0.1		(V)
	int16_t ext_ignition_capacitor_1_charge_time;	//*0.1		(V)
	int16_t ext_ignition_capacitor_2_charge_time;	//*0.1		(V)
	int16_t ext_ignition_capacitor_3_charge_time;	//*0.1		(V)
	int16_t ext_ignition_capacitor_4_charge_time;	//*0.1		(V)
	int16_t ext_ignition_power_supply;				//*0.001	(V)
	int16_t ext_ignition_power_supply_drop;			//*0.001	(V)
	int16_t ext_ignition_power_level;				//*1		(mJ)
	int16_t ext_ignition_temperature;				//*0.1		(°C)
	int16_t ext_ignition_power_level_config;		//*1		(mJ)
	int16_t ext_ignition_switch_voltage;			//*0.001	(V)
	int16_t ext_ignition_cpu_supply_voltage;		//*0.001	(V)
	int16_t ext_ignition_cpu_temperature;			//*0.1		(°C)
	int16_t ext_ignition_operation_time;			//*0.1		(s)
	int16_t ext_ignition_error_code;
	int16_t ext_ignition_no_load_outputs;
	int16_t ext_ignition_partial_discharge_outputs;
	int16_t ext_ignition_damaged_outputs;
	int16_t ext_ignition_disabled_outputs;
	int16_t ext_ignition_operation_status;
#endif

#ifdef FT_CAN_INJECTION
	int16_t bank_a_injector_1_duty_cycle;		//*0.1		(%)
	int16_t bank_a_injector_2_duty_cycle;		//*0.1		(%)
	int16_t bank_a_injector_3_duty_cycle;		//*0.1		(%)
	int16_t bank_a_injector_4_duty_cycle;		//*0.1		(%)
	int16_t bank_a_injector_5_duty_cycle;		//*0.1		(%)
	int16_t bank_a_injector_6_duty_cycle;		//*0.1		(%)
	int16_t bank_a_injector_7_duty_cycle;		//*0.1		(%)
	int16_t bank_a_injector_8_duty_cycle;		//*0.1		(%)
	int16_t bank_a_injector_9_duty_cycle;		//*0.1		(%)
	int16_t bank_a_injector_10_duty_cycle;		//*0.1		(%)
	int16_t bank_a_injector_11_duty_cycle;		//*0.1		(%)
	int16_t bank_a_injector_12_duty_cycle;		//*0.1		(%)
	int16_t bank_b_injector_1_duty_cycle;		//*0.1		(%)
	int16_t bank_b_injector_2_duty_cycle;		//*0.1		(%)
	int16_t bank_b_injector_3_duty_cycle;		//*0.1		(%)
	int16_t bank_b_injector_4_duty_cycle;		//*0.1		(%)
	int16_t bank_b_injector_5_duty_cycle;		//*0.1		(%)
	int16_t bank_b_injector_6_duty_cycle;		//*0.1		(%)
	int16_t bank_b_injector_7_duty_cycle;		//*0.1		(%)
	int16_t bank_b_injector_8_duty_cycle;		//*0.1		(%)
	int16_t bank_b_injector_9_duty_cycle;		//*0.1		(%)
	int16_t bank_b_injector_10_duty_cycle;		//*0.1		(%)
	int16_t bank_b_injector_11_duty_cycle;		//*0.1		(%)
	int16_t bank_b_injector_12_duty_cycle;		//*0.1		(%)
#endif

#ifdef FT_CAN_EXTRA
	int16_t air_conditioning;		//0 == off, 1 == on
	int16_t battery_voltage;		//*0.1		(V)
	int16_t battery_temperature;	//*0.1		(°C)
	int16_t brake_pressure;			//*0.001	(Bar)
	int16_t electro_fan;			//0 == off, 1 == on

	int16_t lambda_average;			//*0.001	(Lambda)
	int16_t fuel_cut;				//*1		(%)
	int16_t total_fuel_flow;		//*0.01		(L/min)

	int16_t driveshaft_rpm;			//*1		(RPM)
	int16_t gear_cut;				//*1		(%)
	int16_t gear_retard;			//*0.1		(°)
	int16_t gear_sensor_voltage;	//*0.001	(V)

	int16_t launch_mode;			//0 == none (running), 1 == burnout, 2 == burnout + 2-step, 3 == 3-step, 4 == 2-step
	int16_t burnout_signal;			//0 == off, 1 == on
	int16_t two_step_signal;		//0 == off, 1 == on
	int16_t three_step_signal;		//0 == off, 1 == on
	int16_t drag_speed;				//*1		(Km/h)
	int16_t traction_speed;			//*1 		(Km/h)

	int16_t wheel_speed_fl;			//*1		(Km/h)
	int16_t wheel_speed_fr;			//*1		(Km/h)
	int16_t wheel_speed_rl;			//*1		(Km/h)
	int16_t wheel_speed_rr;			//*1		(Km/h)

	int16_t day_night_state;		//0 == day, 1 == night
	int16_t generic_outputs_state;
	int16_t self_dial;				//*0.001	(s)
#endif

	int16_t ect;					//*0.1		(°C)
	int16_t coolant_pressure;		//*0.001	(Bar)
	int16_t iat;					//*0.1		(°C)
	int16_t fuel_pressure;			//*0.001	(Bar)
	int16_t gear;
	int16_t lambda;					//*0.001	(Lambda)
	int16_t map;					//*0.001	(Bar)
	int16_t oil_pressure;			//*0.001	(Bar)
	int16_t oil_temperature;		//*0.1		(°C)
	int16_t pit_limiter;			//0 == off, 1 == on
	int16_t rpm;					//*1		(RPM)
	int16_t tps;					//*0.1		(%)

#ifndef FT_CAN_SIMPLE
	uint16_t data_content;
	uint16_t data_id;
	uint16_t data_length;
	uint16_t data_last_seg;
#endif
}FT_Data;

HAL_StatusTypeDef FT_CAN_FilterConfig(CAN_HandleTypeDef *hcan, uint16_t FT_Product, uint8_t filter_bank_position, uint32_t Filter_FIFO);

void FT_CAN_ReceiveData(uint32_t RxID, uint32_t RxLength, uint8_t* pData, FT_Data* FT_Data_Struct);

#ifndef FT_CAN_SIMPLE
void FT_CAN_ProcessCustomData(uint16_t id, uint16_t data);
#endif

#endif /* INC_FT_CAN_H_ */
