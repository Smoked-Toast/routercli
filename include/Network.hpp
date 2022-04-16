#ifndef NETWORK_H
#define NETWORK_H

int is_interface_online(std::string interface);
int * buildSubnet();

class Network {

    public:
        Network(char * CIDR);

        ~Network();

        char * mask_toString();

        char * get_networkid();

        char * get_gatewayip();

        char * get_cidr();

    private:
        char * network_identifier;
        char * gateway_ip;
        char * CIDR;
        int * subnet_mask;
        int size;
        int error;

        int * cidr2mask(char * CIDR);

        void set_networkip(char * CIDR);
};

#endif