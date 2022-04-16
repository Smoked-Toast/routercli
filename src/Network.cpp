#include <algorithm>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <math.h>

#include "../include/Network.hpp"

Network::Network(char * CIDR) : size(4) {
    this->CIDR = CIDR;
    subnet_mask = this->cidr2mask(CIDR);
    this->set_networkip(CIDR);
}

Network::~Network(){
    delete [] subnet_mask;
    delete [] network_identifier;
    delete [] gateway_ip;
}


char * Network::mask_toString(){
    if (subnet_mask == NULL){
        return NULL;
    }
    char * res = new char[16];
    snprintf(res, 16, "%d.%d.%d.%d", subnet_mask[0], subnet_mask[1], subnet_mask[2], subnet_mask[3]);
    return res;
}

char * Network::get_gatewayip(){
    return this->gateway_ip;
}

char * Network::get_networkid(){
    return this->network_identifier;
}

char * Network::get_cidr(){
    return this->CIDR;
}

int * Network::cidr2mask(char * CIDR){

    int * mask = new int[4];

    char * e = strchr(CIDR, '/');
    int index = (int)(e - CIDR);
    int n = std::stoi(CIDR + index+1);

    if (n > 32){
        delete [] mask;
        return NULL;
    }

    for (int i = 0; i < 4; i++){

        if (n >= 8){
            mask[i] = 255;
        }
        else if (n < 0){
            mask[i] = 0;
        }
        else {
            mask[i] = 256 - pow(2, 8 - n);
        }
        n -= 8;
    }

    return mask;
}

void Network::set_networkip(char * CIDR){
    char * block = new char[4];
    char * block_copy = block;
    char * cidr_copy = CIDR;

    int * nid = new int[4];
    int i = 0;

    while (*cidr_copy) {
        if (*cidr_copy == '.'){
            *block_copy = '\0';
            nid[i++] = std::stoi(block);
            delete [] block;
            block = new char[4];
            block_copy = block;
            cidr_copy++;
        }
        else if (*cidr_copy == '/'){
            break;
        }
        else {
            *block_copy++ = *cidr_copy++;
        }
    }

    *block_copy = '\0';
    nid[i] = std::stoi(block);
    delete [] block;

    for (int i = 0; i < 4; i++){
        nid[i] = nid[i] & subnet_mask[i];
    }

    char * net = new char[16];
    char * firstip = new char[16];

    snprintf(net, 16, "%d.%d.%d.%d", nid[0], nid[1], nid[2], nid[3]);
    snprintf(firstip, 16, "%d.%d.%d.%d", nid[0], nid[1], nid[2], nid[3] + 1);
    network_identifier = net;
    gateway_ip = firstip;

    delete [] nid;
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

    int retval;
    for (size_t i = 0; i < inets.size(); i++){
        if (strcmp(inets[i], interface.c_str())){
            retval = 1;
            break;
        }
    }
    retval = 0;
    freeifaddrs(addresses);
    return retval;
}