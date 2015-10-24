#include "unpack.h"
#include "obd2pids.h"

#include <stdio.h>

bool is_obd2_response(unsigned int message_id){
	if( (message_id <= CAN_OBD2_RESPONSE_MESSAGE_ID_HIGH) &&
			(message_id >= CAN_OBD2_RESPONSE_MESSAGE_ID_LOW))
	{
		return true;
	}

	return false;
}/*is_obd2_response*/

void unpack_obd2_response(obd2_response* response){
	switch(response->pid){
		case ENGINE_COOLANT_TEMP:
			printf("Engine coolant: %i Celsius\n", unpack_engine_coolant(response->A));
			break;
		case ENGINE_RPM:
			printf("Engine rpm: %u\n" , unpack_engine_rpm(response->A, response->B));
		default:
			break;
	}
}/*unpack_obd2_response*/

int unpack_engine_coolant(char A){
	int engine_coolant_temperature_centigrade = A-40;
	return engine_coolant_temperature_centigrade;
}/*unpack_engine_coolant*/

unsigned int unpack_engine_rpm(char A, char B){
	unsigned int engine_rpm = (A*256 + B)/4;
	return engine_rpm;
}/*unpack_engine_rpm*/

unsigned int unpack_intake_manifold_pressure(char A){
	unsigned int intake_manifold_pressure = A;
	return intake_manifold_pressure;
}/*unpack_intake_manifold_pressure*/

double unpack_mass_airflow_rate(char A, char B){
	double mass_airflow_rate = (A*256+B)/100;
	return mass_airflow_rate;
}/*unpack_mass_airflow_rate*/

double unpack_throttle_position(char A){
	double throttle_position = A*100/255;
	return throttle_position;
}/*unpack_throttle_position*/

int unpack_intake_air_temperature(char A){
	int intake_air_temperature = A-40;
	return intake_air_temperature;
}/*unpack_intake_air_temperature*/

unsigned int unpack_vehicle_speed(char A){
	unsigned int vehicle_speed = A;
	return vehicle_speed;
}/*unpack_vehicle_speed*/

double unpack_ethanol_fuel_percentage(char A){
	double ethanol_fuel_percentage = A*100/255;
	return ethanol_fuel_percentage;
}/*unpack_ethanol_fuel_percentage*/

double unpack_timing_advance(char A){
	double timing_advance = (A-128)/2;
}/*unpack_timing_advance*/
