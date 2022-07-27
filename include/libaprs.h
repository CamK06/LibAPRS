#pragma once

#include <stdint.h>
#include "libaprs/interfaces.h"
#include "libaprs/ax25.h"

typedef void (*receive_raw_callback_t)(char* data, uint32_t len);
typedef void (*receive_ax25_callback_t)(AX25Frame* frame);

namespace APRS
{

extern receive_raw_callback_t receive_raw_callback;
extern receive_ax25_callback_t receive_ax25_callback;

void init_tty(const char* serialPort, uint32_t baudRate, int iface);
void init_ip(const char* ipAddress, uint16_t port, int iface);
void set_receive_raw_callback(receive_raw_callback_t callback);
void set_receive_ax25_callback(receive_ax25_callback_t callback);

}