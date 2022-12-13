// Inspired by https://github.com/wagiminator/ATtiny13-TinyRemoteRF/blob/main/software/remote_4_buttons/RF_Remote_4B.ino

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#define RF_BIT 150  //The symbol bit length in us
#define RF_SYMBOL 4*RF_BIT  //The symbol length
#define RF_ZERO 1*RF_BIT
#define RF_ONE 3*RF_BIT
#define RF_REP 3
#define RF_REP_PAUSE 2*RF_SYMBOL

#define RF_PREAMPLE 16  //Number of 1 bits before a transmission
#define RF_START 4  //Number of 0 and 1 bit pairs before a packet

#define TRANSMITTER_PIN 14

void send_bit (bool bit) {
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

void send_byte (uint8_t byte) {
  for (uint8_t i = 1<<7; i > 0; i>>=1) {
    send_bit(byte&i);
  }
}

void send_preample() {
  for (int i = 0; i < RF_PREAMPLE; i++) {
    send_bit(1);
  }
}

void send_start() {
  for (int i = 0; i < RF_START; i++) {
    send_bit(0);
    send_bit(1);
  }
}

void send_message (uint8_t len, char *buffer) {
  send_preample();
  send_start();
  send_byte(len);
  for (int cycles = 0; cycles < RF_REP; cycles++) {
    for (int byte = 0; byte < len; byte++) {
      send_byte(buffer[byte]);
    }
    sleep_us(RF_REP_PAUSE);
  }
}

int main() {
  // Setup stuff
  //stdio_init_all();

  gpio_init(TRANSMITTER_PIN);
  gpio_set_dir(TRANSMITTER_PIN, GPIO_OUT);
  int len = 6;
  char *msg = "Tere!\n";

  while (true) {
    send_message(len, msg);
    sleep_ms(100);
  }

  return 0;
}