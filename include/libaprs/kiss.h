#pragma once

#include <stdint.h>
#include "interfaces.h"

// KISS Characters
#define KISS_FEND 0xC0
#define KISS_FESC 0xDB
#define KISS_TFEND 0xDC
#define KISS_TFESC 0xDD

// KISS Commands
#define KISS_CMD_DATAFRAME 0x00
#define KISS_CMD_TXDELAY 0x01
#define KISS_CMD_PERSIST 0x02
#define KISS_CMD_SLOT_TIME 0x03
#define KISS_CMD_TX_TAIL 0x04
#define KISS_CMD_DUPLEX 0x05
#define KISS_CMD_SET_HARDWARE 0x06
#define KISS_CMD_RETURN 0xff

// Interfaces
#define KISS_TCP_IFACE 0x00
#define KISS_TTY_IFACE 0x01

namespace KISS
{
	void init_tcp(const char* ipAddress, uint16_t port);
	void init_tty(const char* serialPort, uint32_t baudRate);
	void send_raw(const char* data, uint32_t len);
	void handle_sigio(int sig);
}