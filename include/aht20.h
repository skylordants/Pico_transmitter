#ifndef __AHT20_H__
#define __AHT20_H__

bool aht20_setup (i2c_inst_t *i2c);

bool aht20_measure(i2c_inst_t *i2c, uint32_t *temperature, uint32_t* humidity);

float aht20_calculate_temperature(uint32_t temperature);

float aht20_calculate_humidity(uint32_t humidity);

#endif