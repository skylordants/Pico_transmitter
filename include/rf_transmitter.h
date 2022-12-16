#ifndef __RF_TRANSMITTER_H__
#define __RF_TRANSMITTER_H__

bool rf_transmitter_setup();

void rf_send_message (char *buffer, uint8_t len);

#endif