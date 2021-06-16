#ifndef NETWORK_H
#define NETWORK_H

int is_interface_online(std::string interface);

class Network {
public:
    char *bridge;

    Network();
    ~Network();
};

#endif