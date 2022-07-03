#include <iostream>
#include "libaprs/aprs.h"

int main()
{
    APRS::init_ip("127.0.0.1", 8001, IFACE_KISS);
    while(true);
}