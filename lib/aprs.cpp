#include "libaprs/aprs.h"
#include "libaprs/kiss.h"
#include <spdlog/spdlog.h>

namespace APRS
{

void init_ip(const char* ipAddress, uint16_t port, uint iface)
{
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("libAPRS is running in debug mode");
#endif

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

receive_raw_callback_t receive_raw_callback = nullptr;
// TODO: Implement this alongside the rest of AX25 stuff
//receive_ax25_callback_t receive_ax25_callback = nullptr;

void set_receive_raw_callback(receive_raw_callback_t callback)
{
    receive_raw_callback = callback;
}

}