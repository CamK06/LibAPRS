#pragma once

#include "interfaces.h"
#include "ax25.h"

namespace APRSIS
{
    void init_tcp(const char* ipAddress, uint16_t port);
    void send_raw(const char* data, uint32_t len);
    void handle_sigio(int sig);
}