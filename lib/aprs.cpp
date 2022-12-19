#include "libaprs.h"
#include "libaprs/kiss.h"
#include "libaprs/js8.h"
#include "libaprs/aprsis.h"
#include <spdlog/spdlog.h>
#include <string>

namespace APRS
{

int currentIface = -1;

// User info
AX25Callsign callsign = { {"N0CALL"}, 0 };
AX25Callsign destination = { "APZ123", 0 };
AX25Callsign path[8] = { {"WIDE1 ", 1}, {"WIDE2 ", 2} };
int pathCount = 2;

// Abstracted user functions
void set_callsign(const char* callsign, uint8_t ssid)
{
    for(int i = 0; i < 6; i++) {
        if(i > strlen(callsign)-1) {
            APRS::callsign.callsign[i] = ' ';
            continue;
        }
        APRS::callsign.callsign[i] = callsign[i];
    }
    APRS::callsign.ssid = ssid;
}

void set_destination(const char* callsign, uint8_t ssid)
{
    for(int i = 0; i < 6; i++) {
        if(i > strlen(callsign)-1) {
            APRS::destination.callsign[i] = ' ';
            continue;
        }
        APRS::destination.callsign[i] = callsign[i];
    }
    APRS::destination.ssid = ssid;
}

void set_digi_path(const char* callsign, uint8_t ssid, int index)
{
    for(int i = 0; i < 6; i++) {
        if(i > strlen(callsign)-1) {
            APRS::path[index].callsign[i] = ' ';
            continue;
        }
        APRS::path[index].callsign[i] = callsign[i];
    }
    APRS::path[index].ssid = ssid;
    pathCount = index + 1;
}

void send_status(const char* status)
{
    // Format the status message into the info field
    char* info = new char[256];
    info[0] = '>';
    strcpy(info+1, status);

    // Build the frame
    AX25Frame frame;
    AX25::build_frame(callsign, destination, path, pathCount, info, &frame);

    // Send the frame
    send_ax25(&frame);
}

void send_raw_info(const char* info)
{
    // Build the frame
    AX25Frame frame;
    AX25::build_frame(callsign, destination, path, pathCount, (char*)info, &frame);

    // Send the frame
    send_ax25(&frame);
}

void send_message(const char* message, const char* destinationCall, uint8_t destinationSSID)
{
    // Format the message for the info field/payload
    char* info = new char[256];
    info[0] = ':';
    for(int i = 1; i < 10; i++) {
        if(i > strlen(destinationCall)) {
            info[i] = ' ';
            continue;
        }
        info[i] = destinationCall[i-1];
    }
    info[10] = ':';
    strcpy(info+11, message);

    // Build the frame
    AX25Frame frame;
    AX25::build_frame(callsign, destination, path, pathCount, info, &frame);

    // Send the frame
    send_ax25(&frame);
}

// "Backend" functions
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
        case IFACE_JS8CALL:
            JS8::init_tcp(ipAddress, port);
            currentIface = IFACE_JS8CALL;
            break;
        //case IFACE_APRSIS:
        //    APRSIS::init_tcp(ipAddress, port);
        //    currentIface = IFACE_APRSIS;
        //    break;
        default:
            spdlog::error("libAPRS: Invalid interface type");
            break;
    }

    spdlog::info("libAPRS: Initialized");
}

void init_tty(const char* serialPort, uint32_t baudRate, int iface)
{
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("libAPRS is running in debug mode");
#endif

    // Initialize the chosen interface/protocol
    switch(iface) {
        case IFACE_KISS:
            KISS::init_tty(serialPort, baudRate);
            currentIface = IFACE_KISS;
            break;
        default:
            spdlog::error("libAPRS: Invalid interface type. Are you sure this interface type exists and supports TTY?");
            break;
    }

    spdlog::info("libAPRS: Initialized");
}

void send_ax25(AX25Frame* frame)
{
    // If we're using JS8Call, just send the info field as is
    if(currentIface == IFACE_JS8CALL) {
        JS8::send_raw_info(frame->payload, strlen(frame->payload));
        return;
    }

    // Build an AX.25 frame
	char* ax25Frame = new char[331];
	uint32_t len;
	AX25::encode_frame(frame, ax25Frame, &len);

	// Send the frame
	send_raw(ax25Frame, len);
}

void send_raw(const char* data, uint32_t len)
{
    switch(currentIface) {
        case IFACE_KISS:
            KISS::send_raw(data, len);
            break;
        case IFACE_JS8CALL:
            spdlog::warn("libAPRS: Raw data over JS8 is raw text content, NOT an encoded AX.25 frame! Please only send_raw with JS8 if you absolutely know what you're doing!");
            JS8::send_raw_info(data, len);
        break;
        //case IFACE_APRSIS:
        //    APRSIS::send_raw(data, len);
        //break;
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
