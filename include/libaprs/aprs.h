#pragma once

#include <iostream>
#include <stdint.h>
#include "interfaces.h"

typedef void (*receive_raw_callback_t)(char* data, uint32_t len);
// TODO: Make this function take in a pointer to an AX25 frame
// typedef void (*receive_ax25_callback_t)(AX25Frame* frame);

namespace APRS
{

extern receive_raw_callback_t receive_raw_callback;

void init_tty(const char* serialPort, uint32_t baudRate, uint iface);
void init_ip(const char* ipAddress, uint16_t port, uint iface);
void set_receive_raw_callback(receive_raw_callback_t callback);
// TODO: Implement this
//void set_receive_ax25_callback(receive_ax25_callback_t callback);

}