#ifndef __BMP280_H__
#define __BMP280_H__

#include "rp2040_i2c.h"

class BMP280 {
public:
	BMP280(RP2040_I2C *i2c);
	BMP280();

	bool measure();

	static float calculate_temperature(int32_t temperature);
	/// @brief Calculates pressure in hPa from raw data
	static float calculate_pressure(uint32_t pressure);

	float calculate_current_temperature();
	/// @brief Calculates pressure in hPa from raw data
	float calculate_current_pressure();

	int32_t temperature;
	uint32_t pressure;
private:
	RP2040_I2C *_i2c;

	// Compensation values
	uint16_t _dig_T1;
	int16_t  _dig_T2;
	int16_t  _dig_T3;
	uint16_t _dig_P1;
	int16_t  _dig_P2;
	int16_t  _dig_P3;
	int16_t  _dig_P4;
	int16_t  _dig_P5;
	int16_t  _dig_P6;
	int16_t  _dig_P7;
	int16_t  _dig_P8;
	int16_t  _dig_P9;

	void read_compensation_constants();

	int32_t _t_fine;
	int32_t compensate_temperature (int32_t adc_T);
	uint32_t compensate_pressure (int32_t adc_P);
};

#endif