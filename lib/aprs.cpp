#include "libaprs/aprs.h"
#include "libaprs/kiss.h"
#include <spdlog/spdlog.h>

namespace APRS
{

void init_ip(const char* ipAddress, uint16_t port, uint iface)
{
    // Initialize the chosen interface/protocol
    switch(iface) {
        case IFACE_KISS:
            KISS::init_tcp(ipAddress, port);
            break;
        default:
            spdlog::error("libAPRS: Invalid interface type");
            break;
    }

    spdlog::info("libAPRS: Initialized");
}

void init_tty(const char* serialPort, uint32_t baudRate, uint iface)
{
    // TODO: Implement
}

}