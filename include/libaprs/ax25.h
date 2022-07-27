#pragma once

#include <stdint.h>

#define AX25_FLAG 0x7e
#define AX25_CONTROL 0x03
#define AX25_PROTOCOL 0xf0

// This might be wrong, possibly will be changed later
struct AX25Frame
{
	char callsign[7];
	char ssid;
	char path[8][7];
	char digi_path[8][7];
	char digi_path_ssid[8];
	char payload[256];
};

namespace AX25
{
    void parse_frame(char* data, uint32_t len, AX25Frame* outFrame);
    //void send_frame(AX25Frame* frame);
}