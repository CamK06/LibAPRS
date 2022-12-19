#pragma once

#include <stdint.h>
#include <stddef.h>

#define AX25_FLAG 0x7e
#define AX25_CONTROL 0x03
#define AX25_PROTOCOL 0xf0

typedef struct AX25Callsign
{
	char callsign[7];
	char ssid;
} AX25Callsign;

typedef struct AX25Frame
{
	AX25Callsign source;
	AX25Callsign destination;
	AX25Callsign rptList[8];
	char rptCount;
	char pid;
	char payload[256];
} AX25Frame;

namespace AX25
{
	void build_frame(AX25Callsign source, AX25Callsign destination, AX25Callsign* rptList, int rptCount, char* payload, AX25Frame* outFrame);
    void parse_tnc2_frame(char* data, uint32_t len, AX25Frame* outFrame);
	void parse_raw_frame(char* data, uint32_t len, AX25Frame* outFrame);
	void encode_frame(AX25Frame* frame, char* outData, uint32_t* outLen);
}