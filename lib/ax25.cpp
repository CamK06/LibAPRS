#include "libaprs/ax25.h"
#include "libaprs/kiss.h"
#include <spdlog/spdlog.h>
#include <vector>
#include <fstream>

namespace AX25
{

void build_frame(AX25Callsign source, AX25Callsign destination, AX25Callsign* rptList, int rptCount, char* payload, AX25Frame* outFrame)
{
    outFrame->source = source;
    outFrame->destination = destination;
    for(int i = 0; i < rptCount; i++)
        outFrame->rptList[i] = rptList[i];
    outFrame->rptCount = rptCount;
    outFrame->pid = AX25_PROTOCOL;
    strcpy(outFrame->payload, payload);
}

void parse_raw_frame(char* data, uint32_t len, AX25Frame* outFrame)
{
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

void parse_tnc2_frame(char* data, uint32_t len, AX25Frame* outFrame)
{
    // TNC2 header as raw strings (to be converted to appropriate data types later)
    char source[7];
    char sourceSSID[3];
    char destination[7];
    char destinationSSID[2];
    char digipeaters[8][7];
    char digipeaterSSIDs[8][2];
    char digipeaterCount = 0;

    // Used to determmine which string/char array to write to in parsing
    int currentField = 0;
    int fieldStart = 0;

    // Parse
    for(int i = 0; i < len; i++) {
        switch(currentField)
        {
            // Source callsign
            case 0:
                if(data[i] == '>') {
                    currentField += 2; // Skip SSID
                    fieldStart = i+1;
                    source[i] = '\0';
                    break;
                }
                else if(data[i] == '-') {
                    currentField++;
                    fieldStart = i+1;
                    source[i] = '\0';
                    break;
                }
                if(i < 7)
                    source[i] = data[i];
                break;

            // SSID
            case 1:
                if(data[i] == '>') {
                    currentField++;
                    sourceSSID[i-fieldStart] = '\0';
                    fieldStart = i+1;
                    break;
                }
                if(i-fieldStart < 2)
                    sourceSSID[i-fieldStart] = data[i];
                break;

            default:
                break;
        }
    }

    // Format into the AX25Frame
    for(int i = 0; i < 6; i++) {
        outFrame->source.callsign[i] = source[i];
        outFrame->destination.callsign[i] = destination[i];
    }
    outFrame->source.ssid = atoi(sourceSSID);
}

void encode_frame(AX25Frame* frame, char* outData, uint32_t* outLen)
{   
    char* ax25Frame = new char[331];
    int index = 0;

    // Add the destination callsign
    for(int i = 0; i < 6; i++) {
        if(frame->destination.callsign[i] == '\0')
            break;
        ax25Frame[index] = frame->destination.callsign[i] << 1;
        index++;
    }

    // Add the destination SSID
    ax25Frame[index] = frame->destination.ssid << 1; //| 0x60; // OR with 0x60 to set the last two bits to 1 to indicate the end of the callsign
    index++;

    // Add the source callsign
    for(int i = 0; i < 6; i++) {
        if(frame->source.callsign[i] == '\0')
            break;
        ax25Frame[index] = frame->source.callsign[i] << 1;
        index++;
    }

    // Add the source SSID
    ax25Frame[index] = frame->source.ssid << 1; //| 0x60; // OR with 0x60 to set the last two bits to 1 to indicate the end of the callsign
    index++;

    // Add the digipeater list
    for(int i = 0; i < frame->rptCount; i++) {
        for(int j = 0; j < 6; j++) {
            if(frame->rptList[i].callsign[j] == '\0')
                break;
            ax25Frame[index] = frame->rptList[i].callsign[j] << 1;
            index++;
        }
        // Set the last bit of the SSID to 1 if it's the last digipeater
        if(i == frame->rptCount-1)
            ax25Frame[index] = (frame->rptList[i].ssid << 1) | 0x1;
        else
            ax25Frame[index] = frame->rptList[i].ssid << 1;
        index++;
    }

    // Add the control and protocol bytes
    ax25Frame[index++] = AX25_CONTROL;
    ax25Frame[index++] = AX25_PROTOCOL;

    // Add the payload
    for(int i = 0; i < 256; i++) {
        if(frame->payload[i] == '\0')
            break;
        ax25Frame[index++] = frame->payload[i];
    }

    // Return the new frame
    memcpy(outData, ax25Frame, index);
    *outLen = index;
    delete[] ax25Frame;
}

}