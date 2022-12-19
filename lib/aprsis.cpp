#include "libaprs.h"
#include "libaprs/aprsis.h"

#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/file.h>
#include <sys/signal.h>
#include <arpa/inet.h>
#include <fcntl.h>

namespace APRSIS
{

// General vars
int sockfd = -1;

void init_tcp(const char* ipAddress, uint16_t port)
{
    spdlog::info("Initializing APRS-IS TCP interface...");
    spdlog::info("IP Address: {}", ipAddress);
    spdlog::info("Port: {}", port);

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        spdlog::error("libAPRS: Error opening APRS-IS socket");
        return;
    }

    // TODO: Handle DNS here; getaddrinfo()

    // Get and verify the APRS-IS server IP address
    struct sockaddr_in aprsis_addr;
    aprsis_addr.sin_family = AF_INET;
    aprsis_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, ipAddress, &aprsis_addr.sin_addr) <= 0) {
        spdlog::error("libAPRS: Invalid APRS-IS address");
        return;
    }

    // Connect to APRS-IS
    spdlog::info("libAPRS: Connecting to APRS-IS...");
    if(connect(sockfd, (struct sockaddr *)&aprsis_addr, sizeof(aprsis_addr)) < 0) {
        spdlog::error("libAPRS: Error connecting to APRS-IS");
        return;
    }
    spdlog::info("libAPRS: Connected to APRS-IS");

    // Set the socket to non-blocking and set O_ASYNC
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK | O_ASYNC);

    // Ensure we receive SIGIO on data received
    signal(SIGIO, handle_sigio);
    if(fcntl(sockfd, F_SETOWN, getpid()) < 0) {
        spdlog::error("libAPRS: Error setting SIGIO handler");
        return;
    }

    // Log in
    send_raw("user N0CALL -1 libAPRS 0\n", 26);
}

void send_raw(const char* data, uint32_t len)
{
    // Send data to the APRS-IS socket
    if(write(sockfd, data, len) < 0) {
        spdlog::error("libAPRS: Error writing to APRS-IS socket");
        return;
    }
    spdlog::info(data);
    spdlog::debug("libAPRS: Wrote {} bytes to APRS-IS", len);
}

void handle_sigio(int sig)
{
    // Read data from the APRS-IS socket
    char buffer[1024];
    int n = read(sockfd, buffer, 1024);
    if(n < 0) {
        spdlog::error("libAPRS: Error reading from APRS-IS socket");
        return;
    }
    //spdlog::debug("libAPRS: Read {} bytes from APRS-IS", n);

    // Ignore comments/server messages
    if(buffer[0] == '#')
        return;

    // Loop over each line of received data
    char* line = strtok(buffer, "\n");
    while(line != nullptr) {
        AX25Frame* frame = new AX25Frame();
        AX25::parse_tnc2_frame(line, strlen(line), frame);
        if(APRS::receive_ax25_callback != nullptr)
            APRS::receive_ax25_callback(frame);
        line = strtok(nullptr, "\n");
    }
}

}