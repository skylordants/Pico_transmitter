// Inspiration: https://github.com/BoschSensortec/BMP2-Sensor-API/
// https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp280-ds001.pdf

#include "pico/stdlib.h"

#include "bmp280.h"

#define BMP280_ADDRESS 0x77
#define BMP280_ID 0xD0
#define BMP280_MEASUREMENTS 0xF7
#define BMP280_CTRL_MEAS 0xF4
#define BMP280_CONFIG 0xF5
#define BMP280_CONSTANTS 0x88

//TODO: Check if setup worked
BMP280::BMP280(RP2040_I2C *i2c)
	: _i2c(i2c)
{
	uint8_t data[2] = {0b10110111, 0b01101000};

	// Write both CTRL_MEAS and CONFIG registers
	_i2c->reg_write(BMP280_ADDRESS, BMP280_CTRL_MEAS, data, 2);

	read_compensation_constants();

}

BMP280::BMP280() {
	
}

void BMP280::read_compensation_constants() {
	uint8_t data[24];

	_i2c->reg_read(BMP280_ADDRESS, BMP280_CONSTANTS, data, 24);

	_dig_T1 = (uint16_t)data[1]<<8 | data[0];
	_dig_T2 = (int16_t)data[3]<<8 | data[2];
	_dig_T3 = (int16_t)data[5]<<8 | data[4];
	_dig_P1 = (uint16_t)data[7]<<8 | data[6];
	_dig_P2 = (int16_t)data[9]<<8 | data[8];
	_dig_P3 = (int16_t)data[11]<<8 | data[10];
	_dig_P4 = (int16_t)data[13]<<8 | data[12];
	_dig_P5 = (int16_t)data[15]<<8 | data[14];
	_dig_P6 = (int16_t)data[17]<<8 | data[16];
	_dig_P7 = (int16_t)data[19]<<8 | data[18];
	_dig_P8 = (int16_t)data[21]<<8 | data[20];
	_dig_P9 = (int16_t)data[23]<<8 | data[22];
}


int32_t BMP280::compensate_temperature (int32_t adc_T) {
	int32_t var1, var2, T;
	var1 = ((((adc_T>>3) - ((int32_t)_dig_T1<<1))) * ((int32_t)_dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t)_dig_T1)) * ((adc_T>>4) - ((int32_t)_dig_T1))) >> 12) * ((int32_t)_dig_T3)) >> 14;
	_t_fine = var1 + var2;
	T = (_t_fine * 5 + 128) >> 8;
	return T;
}

uint32_t BMP280::compensate_pressure (int32_t adc_P) {
	int64_t var1, var2, p;
	var1 = ((int64_t)_t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)_dig_P6;
	var2 = var2 + ((var1*(int64_t)_dig_P5)<<17);
	var2 = var2 + (((int64_t)_dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)_dig_P3)>>8) + ((var1 * (int64_t)_dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)_dig_P1)>>33;
	if (var1 == 0) {
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((int64_t)_dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)_dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)_dig_P7)<<4);
	return (uint32_t)p;
}

//TODO: find errors and stuff
bool BMP280::measure() {
	uint8_t data[6];

	_i2c->reg_read(BMP280_ADDRESS, BMP280_MEASUREMENTS, data, 6);

	int32_t adc_T = (int32_t)data[3]<<12 | (int32_t)data[4]<<4 | data[5]>>4, adc_P = (int32_t)data[0]<<12 | (int32_t)data[1]<<4 | data[2]>>4;

	temperature = compensate_temperature(adc_T);
	pressure = compensate_pressure(adc_P);

	return true;
}


float BMP280::calculate_temperature(int32_t temperature) {
	return (float)temperature/100;
}

float BMP280::calculate_pressure(uint32_t pressure) {
	return (float)pressure/256;
}

float BMP280::calculate_current_temperature() {
	return calculate_temperature(temperature);
}

float BMP280::calculate_current_pressure() {
	return calculate_pressure(pressure);
}
