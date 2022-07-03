#include "libaprs.h"

void receive_raw_callback(char* data, uint32_t len)
{
    std::cout << data << std::endl;
}

int main()
{
    APRS::init_ip("127.0.0.1", 8001, IFACE_KISS);
    APRS::set_receive_raw_callback(&receive_raw_callback);
    while(true);
}

