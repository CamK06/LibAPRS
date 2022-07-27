#include "libaprs/kiss.h"
#include "libaprs/ax25.h"
#include "libaprs.h"

#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <arpa/inet.h>
#include <fstream>

namespace KISS
{

// TCP vars
int sockfd;

// General vars
int currentIface;

void init_tcp(const char* ipAddress, uint16_t port)
{
	spdlog::info("libAPRS: Initializing KISS over TCP...");
	spdlog::info("libAPRS: IP Address: {}", ipAddress);
	spdlog::info("libAPRS: Port: {}", port);
	
	// Create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		spdlog::error("libAPRS: Error opening KISS socket");
		return;
	}
	
	// Get and verify the KISS server IP address
	struct sockaddr_in kiss_addr;
	kiss_addr.sin_family = AF_INET;
	kiss_addr.sin_port = htons(port);
	if(inet_pton(AF_INET, ipAddress, &kiss_addr.sin_addr) <= 0) {
		spdlog::error("libAPRS: Invalid KISS address");
		return;
	}

	// Connect to the KISS server
	spdlog::info("libAPRS: Connecting to KISS server...");
	if(connect(sockfd, (struct sockaddr *)&kiss_addr, sizeof(kiss_addr)) < 0) {
		spdlog::error("libAPRS: Error connecting to KISS TNC");
		return;
	}
	spdlog::info("libAPRS: Connected to KISS server");

	// Set the socket to non-blocking and set O_ASYNC
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK | O_ASYNC);
	
	// Ensure we receive SIGIO on data received
	signal(SIGIO, handle_sigio);
	if(fcntl(sockfd, F_SETOWN, getpid()) < 0) {
		spdlog::error("libAPRS: Error setting SIGIO handler");
		return;
	}

	// Set the current iface to TCP
	currentIface = KISS_TCP_IFACE;
}

void handle_sigio(int sig)
{
	// Read data from the socket
	char buffer[1024];
	int n = read(sockfd, buffer, 1024);
	if(n < 0) {
		spdlog::error("libAPRS: Error reading from KISS socket");
		return;
	}
	spdlog::debug("libAPRS: Received {} bytes from APRS", n);

	// Shift the buffer to remove the KISS framing characters
	n -= 3; // Remove the KISS framing characters from iteration
	for(int i = 0; i < n; i++) 
		buffer[i] = buffer[i+2];
	buffer[n] = '\0'; // Null terminate the string (we should only have a string after removing framing characters)

	// Pass the data to the raw receive callback
	if(APRS::receive_raw_callback != nullptr)
		APRS::receive_raw_callback(buffer, n);

	// TODO: Implement
	// Parse the AX.25 data
	// AX25Frame* frame = new AX25Frame();
	// AX25::parse_frame(buffer, n, frame);
	// if(APRS::receive_ax25_callback != nullptr)
	// 	APRS::receive_ax25_callback(frame);
}

void init_tty(const char* serialPort, uint32_t baudRate)
{
	// TODO
}

void send_raw(const char* data, uint32_t len)
{
	// Build a KISS frame
	char* kissFrame = new char[len+3];
	kissFrame[0] = 0xC0;
	kissFrame[1] = 0x00;
	for(int i = 0; i < len; i++)
		kissFrame[i+2] = data[i];
	kissFrame[len+2] = 0xC0;

	// Send the frame
	if(currentIface == KISS_TCP_IFACE) {
		int n = write(sockfd, kissFrame, len+3);
		if(n < 0) {
			spdlog::error("libAPRS: Error writing to KISS socket");
			return;
		}
		spdlog::debug("libAPRS: Sent {} bytes over KISS", n);
	}
	else if(currentIface == KISS_TTY_IFACE) {
		// TODO
	}

	// Cleanup
	delete[] kissFrame;
}

}