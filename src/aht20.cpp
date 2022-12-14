#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "i2c.h"

#define AHT20_ADDRESS 0x38
#define AHT20_REG_STATUS 0x71
#define AHT20_INIT 0xBE
#define AHT20_INIT_P1 0x08
#define AHT20_INIT_P2 0x00
#define AHT20_TRIG 0xAC
#define AHT20_TRIG_P1 0x33
#define AHT20_TRIG_P2 0x00

//TODO: Check if working
bool aht20_setup (i2c_inst_t *i2c) {
  uint8_t data[2];
  reg_read(i2c, AHT20_ADDRESS, AHT20_REG_STATUS, data, 1);

  // Device not calibrated
  if ((data[0]&(1<<3)) == 0) {
    data[0] = AHT20_INIT_P1;
    data[1] = AHT20_INIT_P2;

    reg_write(i2c, AHT20_ADDRESS, AHT20_INIT, data, 2);
  }

  return true;
}

//TODO: Check validity
bool aht20_measure(i2c_inst_t *i2c, uint32_t *temperature, uint32_t* humidity) {
  uint8_t data[6] = {AHT20_TRIG_P1, AHT20_TRIG_P2};
  
  reg_write(i2c, AHT20_ADDRESS, AHT20_TRIG, data, 2);
  sleep_ms(100);
  do {
    reg_read(i2c, AHT20_ADDRESS, AHT20_REG_STATUS, data, 1);

    sleep_ms(20);
  } while ((data[0]&1) == 1);
  

  i2c_read_blocking(i2c, AHT20_ADDRESS, data, 6, false);

  *temperature = (int32_t)(data[3]&0x0F)<<16 | (int32_t)data[4]<<8 | data[5];
  *humidity = (int32_t)data[1]<<12 | (int32_t)data[2]<<4 | data[3]>>4;

  return true;
}

float aht20_calculate_temperature(uint32_t temperature) {
  return ((float)temperature/(1<<20))*200-50;
}

float aht20_calculate_humidity(uint32_t humidity) {
  return ((float)humidity/(1<<20)*200-50);
}