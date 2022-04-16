#include "../include/Parser.hpp"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <regex>
#include <fstream>

std::vector<char *> * subnetlist2vector(char * subnets){
    char * subnet = new char[50];
    char * subnet_copy = subnet;
    char * subnets_copy = subnets;
    std::vector<char *> * results = new std::vector<char *>();

    //int word_index = 0;
    while (*subnets_copy) {
        if (*subnets_copy == ','){
            *subnet_copy = '\0';
            results->push_back(subnet);
            subnet = new char[50];
            subnet_copy = subnet;
            subnets_copy++;
        }
        else {
            *subnet_copy++ = *subnets_copy++;
        }
    }
    *subnet_copy = '\0';
    results->push_back(subnet);

    return results;
}

Deployment::Deployment(){}

Deployment::~Deployment(){
    delete [] vmid;
    delete [] ram;
    delete [] vcpu;
    delete [] ostype;
    delete [] mac;
    delete [] bootdisk;
    delete [] configdisk;
    delete [] networkconfig;

    for (size_t i = 0; i < subnets->size(); i++){
        delete [] (*subnets)[i];
    }
    delete subnets;
}


int Deployment::add(char * name, char * property){

    if (strcmp(name,"vmid")==0){
        vmid = property;
    }
    else if (strcmp(name,"ram")==0){
        ram = property;
    }
    else if (strcmp(name,"vcpu")==0){
        vcpu = property;
    }
    else if (strcmp(name,"ostype")==0){
        ostype = property;
    }
    else if (strcmp(name,"mac")==0){
        mac = property;
    }
    else if (strcmp(name,"subnets")==0){
        subnets = subnetlist2vector(property);
        delete [] property;
    } 
    else {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

const std::regex id_regex("^[a-z0-9]*$");

bool Testid(std::string id){
    return std::regex_match(id, id_regex);
}

Command ParseArgs(int argc, char *argv[]) {
    Command mycmd;

    const char * short_opts = "a:v:h";
    const struct option long_opts[] = {
        {"action", required_argument, 0, 'a'},
        {"vni", required_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {nullptr, no_argument, nullptr, 0}};
    mycmd.action = ERROR;
    mycmd.error = "Error: Bad input for action.";

    int opt;
    opterr = 0;
    while ((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1)
    {
        switch (opt)
        {
        case 'a':
            mycmd.action = mycmd.GetAction(optarg);
            if (mycmd.action == ERROR)
            {
                return mycmd;
            }
            break;
        case 'v':
            if (Testid(optarg))
            {
                mycmd.vni = optarg;
            }
            break;
        case 'h':
            mycmd.action = USAGE;
            return mycmd;
        }
    }

    return mycmd;
}

// TODO. Efficient string parsing
Deployment* ParseDeployment(char * file) {
    Deployment * d = new Deployment();
    std::fstream newfile;
    newfile.open(file, std::ios::in);
    if (newfile.is_open()) {
        std::string line;
        while (getline(newfile, line)) {
            int i = line.find("=");
            if (i >= 1){
                std::string n = line.substr(0, i);
                std::string p = line.substr(i+1);
                char * name = new char[n.length()+1];
                char * property = new char[p.length()+1];
                strcpy(name, n.c_str());
                strcpy(property, p.c_str());

                int res = d->add(name, property);
                if (res == EXIT_FAILURE){
                    delete [] name;
                    delete [] property;
                    delete d;
                    printf("Error: adding attribute: %s", name);
                    return NULL;
                }
                delete [] name;
            }
        }
        newfile.close();

        

        char * bootdisk = new char[256];
        char * configdisk = new char[256];
        char * networkconfig = new char[256];
        sprintf(bootdisk, "path=/mnt/dcimages/networks/%s/disks/boot.qcow2,device=disk", d->vmid);
        sprintf(configdisk, "path=/mnt/dcimages/networks/%s/disks/seed.qcow2,device=disk", d->vmid);
        sprintf(networkconfig, "bridge=br-lan0,model=virtio,mac=%s", d->mac);
        d->bootdisk = bootdisk;
        d->configdisk = configdisk;
        d->networkconfig = networkconfig;
    }
    else {
        // TODO. Handle Error
        printf("Error: opening deployment file: %s", file);
        return NULL;
    }
    return d;
}