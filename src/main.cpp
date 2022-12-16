// Inspired by https://github.com/wagiminator/ATtiny13-TinyRemoteRF/blob/main/software/remote_4_buttons/RF_Remote_4B.ino

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "bmp280.h"
#include "aht20.h"
#include "i2c.h"


int main() {
  // Setup stuff
  stdio_init_all();
  i2c_setup();
  aht20_setup();
  bmp280_setup();

  /*gpio_init(TRANSMITTER_PIN);
  gpio_set_dir(TRANSMITTER_PIN, GPIO_OUT);
  int len = 6;
  char *msg = "Tere!\n";

  while (true) {
    send_message(len, msg);
    sleep_ms(100);
  }*/

  uint32_t temperature = 0;
  uint32_t humidity = 0;
  int32_t temperature_2 = 0;
  uint32_t pressure = 0;

  while (true){
    aht20_measure(&temperature, &humidity);
    bmp280_measure(&temperature_2, &pressure);
    printf("BMP280 - Temperature: %f°C, Pressure %f Pa\n", (float)temperature_2/100, (float)pressure/256);
    printf("AHT20 - Temperature: %f°C, Humidity %f%%\n", aht20_calculate_temperature(temperature), aht20_calculate_humidity(humidity));
    sleep_ms(500);
  }
  return 0;
}