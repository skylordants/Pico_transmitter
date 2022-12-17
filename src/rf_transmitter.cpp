#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "rf_transmitter.h"

#define RF_BIT 250  //The symbol bit length in us
#define RF_SYMBOL 4*RF_BIT  //The symbol length
#define RF_ZERO 1*RF_BIT
#define RF_ONE 3*RF_BIT
#define RF_REP 3
#define RF_REP_PAUSE 10*RF_SYMBOL

#define RF_PREAMPLE 16  //Number of 1 bits before a transmission
#define RF_START 4  //Number of 0 and 1 bit pairs before a packet

#define TRANSMITTER_PIN 14

bool rf_transmitter_setup() {
	gpio_init(TRANSMITTER_PIN);
	gpio_set_dir(TRANSMITTER_PIN, GPIO_OUT);
	gpio_put(TRANSMITTER_PIN, 1);
	return true;
}

void rf_send_bit (bool bit) {
	gpio_put(TRANSMITTER_PIN, 1);

	if (bit) {
		sleep_us(RF_ONE);
	}
	else {
		sleep_us(RF_ZERO);
	}

	gpio_put(TRANSMITTER_PIN, 0);

	if (bit) {
		sleep_us(RF_SYMBOL-RF_ONE);
	}
	else {
		sleep_us(RF_SYMBOL-RF_ZERO);
	}
}

void rf_send_byte (uint8_t byte) {
	for (uint8_t i = 1<<7; i > 0; i>>=1) {
		rf_send_bit(byte&i);
	}
}

void rf_send_preample() {
	for (int i = 0; i < RF_PREAMPLE; i++) {
		rf_send_bit(1);
	}
}

void rf_send_start() {
	for (int i = 0; i < RF_START; i++) {
		rf_send_bit(0);
		rf_send_bit(1);
	}
}

uint8_t calculate_error_detection (char *buffer, uint8_t len) {
	uint8_t res = 0;
	for (int i = 0; i < len; i++) {
		res ^= buffer[i];
	}

	return res;
}

void rf_send_message (char *buffer, uint8_t len) {
	uint8_t error_det = calculate_error_detection(buffer, len);
	for (int cycles = 0; cycles < RF_REP; cycles++) {
		rf_send_preample();
		rf_send_start();
		rf_send_byte(len+1);
		for (int byte = 0; byte < len; byte++) {
			rf_send_byte(buffer[byte]);
		}
		rf_send_byte(error_det);
		sleep_us(RF_REP_PAUSE);
	}
}
