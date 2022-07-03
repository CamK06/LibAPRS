#pragma once

#include <iostream>
#include <stdint.h>
#include "interfaces.h"

namespace APRS
{

void init_tty(const char* serialPort, uint32_t baudRate, uint iface);
void init_ip(const char* ipAddress, uint16_t port, uint iface);

}