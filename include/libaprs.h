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

// Abstracted user functions
void set_callsign(const char* callsign, uint8_t ssid);
void set_destination(const char* callsign, uint8_t ssid);
void set_digi_path(const char* callsign, uint8_t ssid, int index);
void send_raw_info(const char* info);
void send_message(const char* message, const char* destinationCall, uint8_t destinationSSID);

// "Backend" functions
void init_tty(const char* serialPort, uint32_t baudRate, int iface);
void init_ip(const char* ipAddress, uint16_t port, int iface);
void set_receive_raw_callback(receive_raw_callback_t callback);
void set_receive_ax25_callback(receive_ax25_callback_t callback);
void send_ax25(AX25Frame* frame);
void send_raw(const char* data, uint32_t len);

}