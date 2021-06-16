#include <algorithm>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

#include "../include/Network.hpp"

Network::Network() {
    bridge = new char[22];
    sprintf(bridge, "br-lan0");
}

Network::~Network(){
    delete[] bridge;
}

int is_interface_online(std::string interface) {
    std::vector<char *> inets;
    struct ifaddrs *addresses;
    if (getifaddrs(&addresses) == -1)
    {
        printf("getifaddrs call failed\n");
        return -1;
    }

    struct ifaddrs *address = addresses;
    while (address)
    {
        int family = address->ifa_addr->sa_family;
        if (family == AF_INET || family == AF_INET6)
        {
            inets.push_back(address->ifa_name);
        }
        address = address->ifa_next;
    }
    freeifaddrs(addresses);

    for (size_t i = 0; i < inets.size(); i++){
        if (strcmp(inets[i], interface.c_str())){
            return 1;
        }
    }
    return 0;
}