#include "libaprs.h"
#include "libaprs/kiss.h"
#include <spdlog/spdlog.h>

namespace APRS
{

int currentIface = -1;

void init_ip(const char* ipAddress, uint16_t port, int iface)
{
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("libAPRS is running in debug mode");
#endif

    // Initialize the chosen interface/protocol
    switch(iface) {
        case IFACE_KISS:
            KISS::init_tcp(ipAddress, port);
            currentIface = IFACE_KISS;
            break;
        default:
            spdlog::error("libAPRS: Invalid interface type");
            break;
    }

    spdlog::info("libAPRS: Initialized");
}

void init_tty(const char* serialPort, uint32_t baudRate, int iface)
{
    // TODO: Implement
}

void send_ax25(AX25Frame* frame)
{
    switch(currentIface) {
        case IFACE_KISS:
            KISS::send_ax25(frame);
            break;
        default:
            spdlog::error("libAPRS: No interface selected! Are you sure you initialized libAPRS?");
            break;
    }
}

void send_raw(const char* data, uint32_t len)
{
    switch(currentIface) {
        case IFACE_KISS:
            KISS::send_raw(data, len);
            break;
        default:
            spdlog::error("libAPRS: No interface selected! Are you sure you initialized libAPRS?");
            break;
    }
}

receive_raw_callback_t receive_raw_callback = nullptr;
receive_ax25_callback_t receive_ax25_callback = nullptr;

void set_receive_raw_callback(receive_raw_callback_t callback)
{
    receive_raw_callback = callback;
}

void set_receive_ax25_callback(receive_ax25_callback_t callback)
{
    receive_ax25_callback = callback;
}

}