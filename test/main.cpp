#include "libaprs.h"
#include <iostream>
#include <spdlog/spdlog.h>

void receive_raw_callback(char* data, uint32_t len)
{
    
}

void receive_ax25_callback(AX25Frame* frame)
{
    spdlog::debug("Test client: Received AX.25 frame");
    spdlog::debug("Test client: Callsign: {}", frame->callsign);
    spdlog::debug("Test client: SSID: {}", frame->ssid);
    spdlog::debug("Test client: Information field: {}", frame->payload);
}

int main()
{
    spdlog::set_level(spdlog::level::debug);
    APRS::init_ip("127.0.0.1", 8001, IFACE_KISS);
    APRS::set_receive_raw_callback(&receive_raw_callback);
    APRS::set_receive_ax25_callback(&receive_ax25_callback);
    while(true);
}

