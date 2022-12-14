#ifndef __BMP280_H__
#define __BMP280_H__

bool bmp280_setup(i2c_inst_t *i2c);

bool bmp280_measure(i2c_inst_t *i2c, int32_t *temperature, uint32_t *pressure);

#endif