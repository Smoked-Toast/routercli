#ifndef DEPLOYMENT_H
#define DEPLOYMENT_H
#include <stdio.h>
#include <string.h>
#include <vector>


class Deployment {
public:
    char * vmid;
    char * ram;
    char * vcpu;
    char * ostype;
    char * vni;
    char * mac;
    char * bootdisk;
    char * configdisk;
    char * networkconfig;

    std::vector<char *> * subnets;

    Deployment();

    ~Deployment();

    int add(char * name, char * property);
};

#endif