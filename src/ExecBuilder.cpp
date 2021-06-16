#include <string>

#include "../include/ExecBuilder.hpp"
#include "../include/Network.hpp"
#include "../include/Deployment.hpp"

void createbridgeBuilder(std::vector<char *> *cmd, Network *n)
{
    cmd->push_back((char *)"/usr/sbin/ip");
    cmd->push_back((char *)"link");
    cmd->push_back((char *)"add");
    cmd->push_back((char *)n->bridge);
    cmd->push_back((char *)"type");
    cmd->push_back((char *)"bridge");
    cmd->push_back(NULL);
}

void upbridgeBuilder(std::vector<char *> *cmd, Network *n)
{
    cmd->push_back((char *)"/usr/sbin/ip");
    cmd->push_back((char *)"link");
    cmd->push_back((char *)"set");
    cmd->push_back((char *)n->bridge);
    cmd->push_back((char *)"up");
    cmd->push_back(NULL);
}

void spawnvmBuilder(std::vector<char *> *cmd, Deployment *d)
{

    cmd->push_back((char *)"/usr/bin/virt-install");
    cmd->push_back((char *)"--virt-type");
    cmd->push_back((char *)"kvm");
    cmd->push_back((char *)"--name");
    cmd->push_back(d->vmid);
    cmd->push_back((char *)"--ram");
    cmd->push_back(d->ram);
    cmd->push_back((char *)"--vcpus");
    cmd->push_back(d->vcpu);
    cmd->push_back((char *)"--os-type");
    cmd->push_back(d->ostype);
    cmd->push_back((char *)"--disk");
    cmd->push_back(d->bootdisk);
    cmd->push_back((char *)"--disk");
    cmd->push_back(d->configdisk);
    // TODO
    // Attatch additional disks?
    cmd->push_back((char *)"--import");
    cmd->push_back((char *)"--network");
    cmd->push_back(d->networkconfig);
    cmd->push_back((char *)"--noautoconsole");
    cmd->push_back(NULL);
}

void destroyvmBuilder(std::vector<char *> *cmd, Deployment *d)
{
    cmd->push_back((char *)"/usr/bin/virsh");
    cmd->push_back((char *)"destroy");
    cmd->push_back(d->vmid);
    cmd->push_back(NULL);
}

void undefinevmBuilder(std::vector<char *> *cmd, Deployment *d)
{
    cmd->push_back((char *)"/usr/bin/virsh");
    cmd->push_back((char *)"undefine");
    cmd->push_back(d->vmid);
    cmd->push_back(NULL);
}

void deletebridgeBuilder(std::vector<char *> *cmd, Network *n)
{
    cmd->push_back((char *)"/usr/sbin/ip");
    cmd->push_back((char *)"link");
    cmd->push_back((char *)"delete");
    cmd->push_back((char *)n->bridge);
    cmd->push_back(NULL);
}