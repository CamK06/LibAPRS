#include "libaprs/js8.h"
#include "libaprs/ax25.h"
#include "libaprs.h"

#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/file.h>
#include <sys/signal.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string>
#include <ctime>
#include <spdlog/fmt/fmt.h>

namespace JS8
{

// General vars
int sockfd = -1;

void init_tcp(const char* ipAddress, uint16_t port)
{
    spdlog::info("libAPRS: Initializing JS8Call TCP interface...");
    spdlog::info("libAPRS: IP Address: {}", ipAddress);
    spdlog::info("libAPRS: Port: {}", port);

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        spdlog::error("libAPRS: Error opening JS8 socket");
        return;
    }

    // Get and verify the JS8Call IP address
    struct sockaddr_in js8_addr;
    js8_addr.sin_family = AF_INET;
    js8_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, ipAddress, &js8_addr.sin_addr) <= 0) {
        spdlog::error("libAPRS: Invalid JS8Call address");
        return;
    }

    // Connect to JS8Call
    spdlog::info("libAPRS: Connecting to JS8Call...");
    if(connect(sockfd, (struct sockaddr *)&js8_addr, sizeof(js8_addr)) < 0) {
        spdlog::error("libAPRS: Error connecting to JS8Call");
        return;
    }
    spdlog::info("libAPRS: Connected to JS8Call");

    // Set the socket to non-blocking and set O_ASYNC
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK | O_ASYNC);

    // Ensure we receive SIGIO on data received
    signal(SIGIO, handle_sigio);
    if(fcntl(sockfd, F_SETOWN, getpid()) < 0) {
        spdlog::error("libAPRS: Error setting SIGIO handler");
        return;
    }
}

void handle_sigio(int sig)
{
    // TODO: Implement
}

void send_grid(const char* grid)
{
    long id = std::time(nullptr)*1000;
    std::string message = fmt::format("{{\"type\": \"{}\", \"value\": \"@APRSIS GRID {}\", \"params\": {{ \"_ID\": {} }}}}\n", SEND_MESSAGE, grid, id);
    int n = write(sockfd, message.c_str(), message.length());
    if(n < 0) {
        spdlog::error("libAPRS: Error sending JS8Call grid");
        return;
    }
    spdlog::debug("libAPRS: Sent JS8Call grid: {}", message);
}

void send_raw_info(const char* data, uint32_t len)
{
    long id = std::time(nullptr)*1000;
    std::string message = fmt::format("{{\"type\": \"{}\", \"value\": \"@APRSIS CMD {}\", \"params\": {{ \"_ID\": {} }}}}\n", SEND_MESSAGE, data, id);
    int n = write(sockfd, message.c_str(), message.length());
    if(n < 0) {
        spdlog::error("libAPRS: Error sending JS8Call message");
        return;
    }
    spdlog::debug("libAPRS: Sent JS8Call message: {}", message);
}

}