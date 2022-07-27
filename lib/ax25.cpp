#include "libaprs/ax25.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <iostream>

namespace AX25
{

void parse_frame(char* data, uint32_t len, AX25Frame* outFrame)
{
    // **NONE** OF THIS WORKS. I DONT HAVE A DAMN CLUE WHAT IM DOING.
    // TODO:

    // // Make the whole packet into ASCII bytes
    // for(int i = 0; i < len; i++) {
    //     // If we've passed addresses, break from the loop as these bytes are
    //     // already proper ASCII characters
    //     if(data[i] == AX25_CONTROL || data[i] == AX25_PROTOCOL)
    //         continue; 
    //     data[i] = data[i] >> 1;
    //     data[i] = data[i] & 0x7f;
    // }

    // // Parse the callsign
    // for(int i = 7; i < 13; i++)
    //     outFrame->callsign[i-7] = data[i];
    // outFrame->callsign[7] = '\0';

    // // Parse the SSID
    // outFrame->ssid = data[13];
    // if(outFrame->ssid == 'p') // If the SSID is 'p', no SSID was specified, assume 0
    //     outFrame->ssid = '0';

    // // Parse the path
    // // TODO: Improve this because it fucking sucks
    // for(int i = 0; i < 8; i++) {
    //     for(int j = 0; j < 7; j++)
    //         outFrame->path[i][j] = data[14+i*7+j];
    //     outFrame->path[i][6] = '\0';
    // }

    // // Parse the information field
    // for(int i = 0; i < len-14-8*7; i++)
    //     outFrame->payload[i] = data[14+8*7+i];
    // outFrame->payload[len-14-8*7] = '\0';
}

}