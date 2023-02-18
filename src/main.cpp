// Inspired by https://github.com/wagiminator/ATtiny13-TinyRemoteRF/blob/main/software/remote_4_buttons/RF_Remote_4B.ino

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "rp2040_i2c.h"
#include "bmp280.h"
#include "aht20.h"
#include "rf_transmitter.h"

#define SDA_PIN 26
#define SCL_PIN 27

RP2040_I2C i2c;
BMP280 bmp280;
AHT20 aht20;

int main() {
	stdio_init_all();
	sleep_ms(1000);
	printf("Starting transmitter\n");

	// Setup stuff
	i2c = RP2040_I2C(i2c1, SDA_PIN, SCL_PIN, 400 * 1000);

	aht20 = AHT20(&i2c);
	bmp280 = BMP280(&i2c);
	rf_transmitter_setup();

	int len = 24;
	char msg[25] = "SENSORS:";

	while (true) {
		aht20.measure();
		bmp280.measure();
		printf("BMP280 - Temperature: %f°C, Pressure %f Pa\n", bmp280.calculate_current_temperature(), bmp280.calculate_current_pressure());
		printf("AHT20 - Temperature: %f°C, Humidity %f%%\n", aht20.calculate_current_temperature(), aht20.calculate_current_humidity());
		
		memcpy(msg+8, &aht20.temperature, 4);
		memcpy(msg+12, &aht20.humidity, 4);
		memcpy(msg+16, &bmp280.temperature, 4);
		memcpy(msg+20, &bmp280.pressure, 4);
		rf_send_message(msg, len);
		sleep_ms(500);
	}

	return 0;
}