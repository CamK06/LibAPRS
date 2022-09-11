#pragma once

#include "interfaces.h"
#include "ax25.h"

// These just exist because I'm weird and don't like the way the code
// looks with strings used directly
#define SEND_MESSAGE "TX.SEND_MESSAGE"
#define SET_TEXT "TX.SET_TEXT"
#define STATION_SET_GRID "TX.STATION_SET_GRID"

namespace JS8
{
    void init_tcp(const char* ipAddress, uint16_t port);
    void send_grid(const char* grid);
    void send_raw_info(const char* data, uint32_t len);
    void handle_sigio(int sig);
}