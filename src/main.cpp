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
#define SDA_PIN 26
#define SCL_PIN 27


#define AHT20_ADDRESS 0x38
#define AHT20_REG_STATUS 0x71

#define BMP280_ADDRESS 0x76

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

// Write 1 byte to the specified register
int reg_write( i2c_inst_t *i2c, const uint addr, const uint8_t reg, uint8_t *buf, const uint8_t nbytes) {
  int num_bytes_read = 0;
  uint8_t msg[nbytes + 1];

  // Check to make sure caller is sending 1 or more bytes
  if (nbytes < 1) {
      return 0;
  }

  // Append register address to front of data packet
  msg[0] = reg;
  for (int i = 0; i < nbytes; i++) {
      msg[i + 1] = buf[i];
  }

  // Write data to register(s) over I2C
  i2c_write_blocking(i2c, addr, msg, (nbytes + 1), false);

  return num_bytes_read;
}

// Read byte(s) from specified register. If nbytes > 1, read from consecutive
// registers.
int reg_read(i2c_inst_t *i2c, const uint addr, const uint8_t reg, uint8_t *buf, const uint8_t nbytes) {

  int num_bytes_read = 0;

  // Check to make sure caller is asking for 1 or more bytes
  if (nbytes < 1) {
      return 0;
  }

  // Read data from register(s) over I2C
  i2c_write_blocking(i2c, addr, &reg, 1, true);
  num_bytes_read = i2c_read_blocking(i2c, addr, buf, nbytes, false);

  return num_bytes_read;
}

bool start_aht_20 () {
}

int main() {
  // Setup stuff
  stdio_init_all();

  /*gpio_init(TRANSMITTER_PIN);
  gpio_set_dir(TRANSMITTER_PIN, GPIO_OUT);
  int len = 6;
  char *msg = "Tere!\n";

  while (true) {
    send_message(len, msg);
    sleep_ms(100);
  }*/

  // Ports
  i2c_inst_t *i2c = i2c1;

  // Buffer to store raw reads
  uint8_t data[10];

  // Initialize chosen serial port
  stdio_init_all();

  sleep_ms(2000);

  //Initialize I2C port at 400 kHz
  i2c_init(i2c, 400 * 1000);

  // Initialize I2C pins
  gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

  reg_read(i2c, AHT20_ADDRESS, AHT20_REG_STATUS, data, 1);

  printf("%x", data[0]);

  while (data[0] != 0x18) {
    printf("Device not initialized\n");
    data[0] = 0x08;
    data[1] = 0x00;

    reg_write(i2c, AHT20_ADDRESS, 0xBE, &data[0], 3);
    data[0] = 0;

    sleep_ms(10);
    reg_read(i2c, AHT20_ADDRESS, AHT20_REG_STATUS, data, 1);
  }

  sleep_ms(10);

  while (true){
    printf("Starting read\n");
    data[0] = 0x33;
    data[1] = 0x00;

    reg_write(i2c, AHT20_ADDRESS, 0xAC, &data[0], 3);
    while (true) {
      sleep_ms(100);
      reg_read(i2c, AHT20_ADDRESS, AHT20_REG_STATUS, data, 1);

      if ((data[0]&1) == 0) {
        break;
      }
      printf("Wrong status\n");
    }
    for (int i = 0; i < 10; i++) {
      data[i] = 0;
    }

    int num_bytes_read = i2c_read_blocking(i2c, AHT20_ADDRESS, data, 6, false);

    printf("Read %u bytes\n", num_bytes_read);
    for (int i = 0; i < 6; i++) {
      printf(" %x", data[i]);
    }
    printf("\n");
    sleep_ms(5000);

  }

  return 0;
}