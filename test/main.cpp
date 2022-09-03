#include "libaprs.h"
#include <iostream>
#include <spdlog/spdlog.h>
#include <string.h>

void receive_raw_callback(char* data, uint32_t len)
{
    
}

void receive_ax25_callback(AX25Frame* frame)
{
    spdlog::debug("Test client: Received AX.25 frame");
    // Create a continuous string of the digi path
    std::string digiPath = "";
    for(int i = 0; i < frame->rptCount; i++) {
        if(i != 0)
            digiPath += ",";
        digiPath += frame->rptList[i].callsign;
        digiPath += "-";
        digiPath += std::to_string(frame->rptList[i].ssid);
    }
    // Remove spaces from the digiPath
    digiPath.erase(std::remove(digiPath.begin(), digiPath.end(), ' '), digiPath.end());
    spdlog::debug("Test client: {}>{},{}:{}", frame->source.callsign, frame->destination.callsign, digiPath, frame->payload);
}

int main()
{
    spdlog::set_level(spdlog::level::debug);
    APRS::init_ip("127.0.0.1", 8001, IFACE_KISS);
    APRS::set_receive_raw_callback(&receive_raw_callback);
    APRS::set_receive_ax25_callback(&receive_ax25_callback);
    APRS::set_callsign("VE3KCN", 0);
    while(true) {}
}

