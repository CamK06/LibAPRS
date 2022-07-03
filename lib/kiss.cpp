#include "libaprs/kiss.h"

#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>

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

	// Set the socket to non-blocking
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	
	// Set the current iface to TCP
	currentIface = KISS_TCP_IFACE;


}

void init_tty(const char* serialPort, uint32_t baudRate)
{
	// TODO
}

}