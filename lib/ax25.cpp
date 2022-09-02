#include "libaprs/ax25.h"
#include <spdlog/spdlog.h>

namespace AX25
{

void parse_frame(char* data, uint32_t len, AX25Frame* outFrame)
{
    outFrame->len = len;
    int controlIndex = 0;

    // Make the whole packet into ASCII bytes
    for(int i = 0; i < len; i++) {
        // If we've passed addresses, break from the loop as these bytes are
        // already proper ASCII characters
        if(data[i] == AX25_CONTROL || data[i] == AX25_PROTOCOL)
            break; 
        data[i] = data[i] >> 1;
        data[i] = data[i] & 0x7f;
    }

    // Parse the destination
    for(int i = 0; i < 7; i++)
        if(data[i] == ' ')
            break;
        else
            outFrame->destination.callsign[i] = data[i];
    outFrame->destination.ssid = data[7];
    outFrame->destination.callsign[6] = '\0';

    // Parse the callsign
    for(int i = 7; i < 13; i++)
        if(data[i] == ' ')
            break;
        else
            outFrame->source.callsign[i-7] = data[i];
    outFrame->source.ssid = data[13];
    outFrame->source.callsign[6] = '\0';

    // Parse the digipeater list into individual callsigns and set the count
    outFrame->rptCount = 0;
    for(int i = 14; i < 56; i+=7) {
        if(data[i] == AX25_CONTROL) {
            controlIndex = i;
            break;
        }
        for(int j = 0; j < 7; j++)
            if(data[i] == ' ')
                break;
            else
                outFrame->rptList[outFrame->rptCount].callsign[j] = data[i+j];
        outFrame->rptList[outFrame->rptCount].ssid = data[i+6];
        outFrame->rptList[outFrame->rptCount].callsign[6] = '\0';
        outFrame->rptCount++;
    }

    // Parse the PID
    outFrame->pid = data[controlIndex+1];

    // Parse the information field
    for(int i = controlIndex+2; i < len; i++) {
        if(i-controlIndex-2 == AX25_FLAG) {
            outFrame->payload[i-controlIndex-2] = '\0';
            break;
        }
        outFrame->payload[i-controlIndex-2] = data[i];
    }
}

}