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
    spdlog::debug("Test client: Callsign: {}", frame->source.callsign);
    spdlog::debug("Test client: Destination: {}", frame->destination.callsign);
    spdlog::debug("Test client: SSID: {}", frame->source.ssid);
    spdlog::debug("Test client: digipeater count: {}", frame->rptCount);
    for(int i = 0; i < frame->rptCount; i++) {
        spdlog::debug("Test client: Digipeater {}: {}{}", i+1, frame->rptList[i].callsign, frame->rptList[i].ssid);
    }
    spdlog::debug("Test client: PID: {}", frame->pid);
    spdlog::debug("Test client: Information field: {}", frame->payload);
}

int main()
{
    spdlog::set_level(spdlog::level::debug);
    APRS::init_ip("127.0.0.1", 8001, IFACE_KISS);
    APRS::set_receive_raw_callback(&receive_raw_callback);
    APRS::set_receive_ax25_callback(&receive_ax25_callback);
    while(true) {
        std::string input;
        std::getline(std::cin, input);
        
        AX25Frame* frame = new AX25Frame;
        strcpy(frame->source.callsign, "VE3KCN");
        frame->source.ssid = 0;
        strcpy(frame->destination.callsign, "APZ123");
        frame->destination.ssid = 0;
        frame->rptCount = 2;
        strcpy(frame->rptList[0].callsign, "WIDE1 ");
        frame->rptList[0].ssid = '1';
        strcpy(frame->rptList[1].callsign, "WIDE2 ");
        frame->rptList[1].ssid = '2';
        frame->pid = 0xf0;
        input += '\0';
        strcpy(frame->payload, input.c_str());
        
        APRS::send_ax25(frame);
    }
}

