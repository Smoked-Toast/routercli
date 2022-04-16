#include <algorithm>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include "../include/Lock.hpp"
#include "../include/Parser.hpp"
#include "../include/Controller.hpp"
#include "../include/Network.hpp"
#include "../include/Deployment.hpp"
#include "../include/ExecBuilder.hpp"

char * sshcmd(const char * ip, std::vector<const char *> * cmdbuilder);

Controller::Controller() {}

//TODO: give the caller the exit status of the sandboxed process.
int Controller::Sandbox(char *argv[])
{
    pid_t pid;
    int status;
    if ((pid = fork()) == 0)
    {
        //Call the input function pointer
        execv(argv[0], argv);
        return EXIT_SUCCESS;
    }
    else if (pid == -1)
    {
        printf("Error: fork()\n");
        return EXIT_FAILURE;
    }
    else
    {
        if (waitpid(pid, &status, 0) > 0)
        {

            if (WIFEXITED(status) && !WEXITSTATUS(status))
            {
                printf("program execution successful\n");
                return EXIT_SUCCESS;
            }
            else if (WIFEXITED(status) && WEXITSTATUS(status))
            {
                if (WEXITSTATUS(status) == 127)
                {
                    printf("execv failed\n");
                    return EXIT_FAILURE;
                }
                else
                {
                    printf("program terminated normally, but returned a non-zero status\n");
                    return EXIT_FAILURE;
                }
            }
            else
            {
                printf("program didn't terminate normally\n");
                return EXIT_FAILURE;
            }
        }
        else
        {
            printf("waitpid() failed\n");
            return EXIT_FAILURE;
        }
    }
}

void Controller::printUsage(char *name)
{
    printf("%s --action [info|deploy|destroy] --vmid <vmid>\n", name);
}

int Controller::execute(int argc, char *argv[])
{
    if (argc == 1)
    {
        this->printUsage(argv[0]);
        return EXIT_SUCCESS;
    }
    Command cmd = ParseArgs(argc, argv);

    if (cmd.action == USAGE)
    {
        this->printUsage(argv[0]);
        return EXIT_SUCCESS;
    }
    else if (cmd.action == ERROR)
    {
        // TODO
        // Log error
        printf("%s\n", cmd.error.c_str());
        return EXIT_FAILURE;
    }
    if (cmd.vni.length() == 0)
    {

        printf("Error: Bad input for vmid.\n");

        return EXIT_FAILURE;
    }

    int retval;
    retval = EXIT_SUCCESS;

    if (cmd.action == INFO)
    {
        // TODO
    }
    else if (cmd.action == DEPLOY)
    {
        // TODO
        // Check for LOCK. Create a Lock if it doesnt exist
        std::vector<char *> createbridge;
        std::vector<char *> upbridge;
        std::vector<char *> spawnvm;

        int res;

        char dpath[256];
        char lpath[256];
        sprintf(dpath, "/mnt/dcimages/networks/vxlan%s/config/deployment", cmd.vni.c_str());
        sprintf(lpath, "/mnt/dcimages/networks/vxlan%s/lock", cmd.vni.c_str());

        Deployment * d = NULL;

        std::vector<Network *> networks;

        char *hostname = NULL;

        char * bridge = NULL;

        FILE * fp = NULL;
        char * cmdstring = NULL;;

        std::vector<const char *> cmdbuilder;

        
        
        // TODO 
        // MAKE DYNAMIC
        const size_t CMD_MAX = 512;
        char createlink[CMD_MAX];
        char addfdb[CMD_MAX];
        
        char linkup[CMD_MAX];


        char input1[CMD_MAX];
        char input2[CMD_MAX];
        char input4[CMD_MAX];

        char forward1[CMD_MAX];
        char forward2[CMD_MAX];
        char forward3[CMD_MAX];
        char forward4[CMD_MAX];

        char output1[CMD_MAX];

        char nat1[CMD_MAX];
        char nat2[CMD_MAX];
        char nat3[CMD_MAX];

        char natip[CMD_MAX];

        // ---------
        const char * underlay_network = "192.168.1.0/24";
        const char * router_ip = "192.168.1.102";

        const char * vm_ip_nosub = "192.168.200.2";
        const char * nat_ip = "192.168.1.111/24";
        const char * nat_ip_nosub = "192.168.1.111";
        // ----------
        bridge = new char[22];
        sprintf(bridge, "br-lan0");

        int iteration = 0;
        std::vector <char *> setaddress;
        std::vector <char *> input3;
        // Build deployment object
        if ((d = ParseDeployment(dpath)) == NULL)
        {
            printf("Error: parsing deployment\n");
            retval = EXIT_FAILURE;
            goto EXIT_DEPLOY;
        }
        for (size_t i = 0; i < d->subnets->size(); i++){
            networks.push_back(new Network((*d->subnets)[i]));
        }   

        snprintf(createlink, CMD_MAX, "sudo ip link add vxlan%s type vxlan id %s dev eth0 dstport 0", cmd.vni.c_str(), cmd.vni.c_str());
        snprintf(addfdb, CMD_MAX, "sudo bridge fdb append to 00:00:00:00:00:00 dst 192.168.1.14 dev vxlan%s", cmd.vni.c_str());
        //snprintf(setaddress, CMD_MAX, "sudo ip addr add %s dev vxlan%s", gateway_ip, cmd.vni.c_str());
        snprintf(linkup, CMD_MAX, "sudo ip link set up dev vxlan%s", cmd.vni.c_str());

        for (size_t i = 0; i < networks.size(); i++){
            char * setaddresstemp = new char[CMD_MAX];
            char * input3temp = new char[CMD_MAX];
            snprintf(setaddresstemp, CMD_MAX, "sudo ip addr add %s/24 dev vxlan%s", networks[i]->get_gatewayip(), cmd.vni.c_str());
            snprintf(input3temp, CMD_MAX, "sudo iptables -A INPUT -s %s -d %s -p icmp --icmp-type 8 -m state --state NEW,ESTABLISHED,RELATED -j ACCEPT", networks[i]->get_cidr(), networks[i]->get_cidr());
            setaddress.push_back(setaddresstemp);
            input3.push_back(input3temp);
        }  

        snprintf(input1, CMD_MAX, "sudo iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT");
        snprintf(input2, CMD_MAX, "sudo iptables -A INPUT -s %s -d %s -p icmp --icmp-type 8 -m state --state NEW,ESTABLISHED,RELATED -j ACCEPT", underlay_network, underlay_network);
        //snprintf(input3, CMD_MAX, "sudo iptables -A INPUT -s %s -d %s -p icmp --icmp-type 8 -m state --state NEW,ESTABLISHED,RELATED -j ACCEPT", subnet, subnet);
        snprintf(input4, CMD_MAX, "sudo iptables -A INPUT -s %s -j ACCEPT", underlay_network);
        
        snprintf(forward1, CMD_MAX, "sudo iptables -A FORWARD -m conntrack --ctstate RELATED,ESTABLISHED -j ACCEPT");
        snprintf(forward2, CMD_MAX, "sudo iptables -A FORWARD -d %s -j DROP", underlay_network);
        snprintf(forward3, CMD_MAX, "sudo iptables -A FORWARD -i vxlan%s -o eth0 -j ACCEPT", cmd.vni.c_str());
        snprintf(forward4, CMD_MAX, "sudo iptables -A FORWARD -i eth0 -o vxlan%s -j ACCEPT", cmd.vni.c_str());

        snprintf(output1, CMD_MAX, "sudo iptables -A OUTPUT -j ACCEPT");

        // These should be created when virtual machines are launched. Not when the router is launched.
        snprintf(nat1, CMD_MAX, "sudo iptables -t nat -A PREROUTING -d %s -j DNAT --to-destination %s", nat_ip_nosub, vm_ip_nosub);
        snprintf(nat2, CMD_MAX, "sudo iptables -t nat -A POSTROUTING -s %s -j SNAT --to-source %s", vm_ip_nosub, nat_ip_nosub);

        snprintf(nat3, CMD_MAX, "sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE");

        snprintf(natip, CMD_MAX, "sudo ip addr add %s dev eth0", nat_ip);

        cmdbuilder.push_back(createlink);
        cmdbuilder.push_back(addfdb);
        cmdbuilder.push_back(linkup);
        cmdbuilder.push_back("sudo echo 1 | sudo tee /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts");
        cmdbuilder.push_back("sudo echo 0 | sudo tee /proc/sys/net/ipv4/conf/all/accept_source_route");
        cmdbuilder.push_back("sudo echo 1 | sudo tee /proc/sys/net/ipv4/tcp_syncookies");
        cmdbuilder.push_back("sudo echo 0 | sudo tee /proc/sys/net/ipv4/conf/all/accept_redirects");
        cmdbuilder.push_back("sudo echo 0 | sudo tee /proc/sys/net/ipv4/conf/all/send_redirects");
        cmdbuilder.push_back("sudo echo 1 | sudo tee /proc/sys/net/ipv4/conf/all/rp_filter");
        cmdbuilder.push_back("sudo echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward");
        cmdbuilder.push_back("sudo iptables --flush");
        cmdbuilder.push_back("sudo iptables --policy INPUT DROP");
        cmdbuilder.push_back("sudo iptables --policy OUTPUT DROP");
        cmdbuilder.push_back("sudo iptables --policy FORWARD DROP");

        cmdbuilder.push_back(input1);
        cmdbuilder.push_back(input2);
        cmdbuilder.push_back(input4);

        for (size_t i = 0; i < networks.size(); i++){
            cmdbuilder.push_back(setaddress[i]);
            cmdbuilder.push_back(input3[i]);
        }

        cmdbuilder.push_back(forward1);
        cmdbuilder.push_back(forward2);
        cmdbuilder.push_back(forward3);
        cmdbuilder.push_back(forward4);
   
        cmdbuilder.push_back(output1);

        cmdbuilder.push_back(nat1);
        cmdbuilder.push_back(nat2);
        cmdbuilder.push_back(nat3);

        cmdbuilder.push_back(natip);

        cmdstring = sshcmd(router_ip, &cmdbuilder);
        if (cmdstring == NULL){
            printf("Error: ssh command too large\n");
            retval = EXIT_FAILURE;
            goto EXIT_DEPLOY;
        }

        // Get this computer's hostname
        if ((hostname = getHostname()) == NULL) {
            fprintf(stderr, "Error: getting hostname. %s\n", strerror(errno));
            retval = EXIT_FAILURE;
            goto EXIT_DEPLOY;
        }

        // Create the Lock. Error if a lock already exists for a different host
        if ((res = createLock(hostname, lpath)) == -1) {
            printf("Error: creating lock.\n");
            retval = EXIT_FAILURE;
            goto EXIT_DEPLOY;
        }
        
        // Error if a lock already exists for this host
        if (res == 1) {
            printf("Error: creating lock.\n");
            retval = EXIT_FAILURE;
            goto EXIT_DEPLOY;
        }

        //ip link add br-vxlan1 type bridge
        createbridgeBuilder(&createbridge, bridge);

        //ip link set br-vxlan1 up
        upbridgeBuilder(&upbridge, bridge);

        //virt-install
        spawnvmBuilder(&spawnvm, d);


        if (is_interface_online(bridge) == 0) {
            this->Sandbox(&createbridge[0]);
            this->Sandbox(&upbridge[0]);

            printf("started interface");
        }
        this->Sandbox(&spawnvm[0]);


        // TODO
        // CLEAN
        
        while (true){
            std::cout << "Iteration: " << iteration << std::endl;
            if (!(fp =  popen(cmdstring, "r"))) {
                fprintf( stderr, "Could not execute command \n" );
                break;
            }
            
            int retval;
            if ((retval = pclose(fp)) == 0){
                break;
            }
            iteration ++;

            if (iteration > 50){
                break;
            }
            
            sleep(30);
            
        }

        goto EXIT_DEPLOY;

    EXIT_DEPLOY:
        if (cmdstring != NULL){
            delete [] cmdstring;
        }
        if (hostname != NULL){
            delete [] hostname;
        }
        if (d != NULL){
            delete d;
        }
        if (bridge != NULL){
            delete [] bridge;
        }

        for (size_t i = 0; i < networks.size(); i++){
            delete networks[i];
        }

        for (size_t i = 0; i < setaddress.size(); i++){
            delete [] setaddress[i];
            delete [] input3[i];
        }

    }
    else if (cmd.action == DESTROY)
    {
        // TODO
        char * hostname;

        std::vector<char *> destroyvm;
        std::vector<char *> undefinevm;

        char dpath[256];
        char lpath[256];

        sprintf(dpath, "/mnt/dcimages/networks/vxlan%s/config/deployment", cmd.vni.c_str());
        sprintf(lpath, "/mnt/dcimages/networks/vxlan%s/lock", cmd.vni.c_str());

        Deployment * d;
        char * bridge;

        if ((d = ParseDeployment(dpath)) == NULL) {
            printf("Error: parsing deployment");
            retval = EXIT_FAILURE;
            goto EXIT_DESTROY;
        }

        bridge = new char[22];
        sprintf(bridge, "br-lan0");
        
        // Get this computer's hostname
        if ((hostname = getHostname()) == NULL){
            fprintf(stderr, "Error: getting hostname. %s\n", strerror(errno));
            retval = EXIT_FAILURE;
            goto EXIT_DESTROY;
        }

        int res;

        // Delete the lock. Error if the lock exists and is owned by a different host
        if ((res = deleteLock(hostname, lpath)) == -1){
            printf("Error: deleting lock.\n");
            retval = EXIT_FAILURE;
            goto EXIT_DESTROY;
        }

        // Warn if the lock does not exist.
        if (res == 1){
            printf("Warning: lock does not exist. Attempting destroy anyway.\n");
        }

        //virsh destroy vmid
        destroyvmBuilder(&destroyvm, d);

        //virsh undefine vmid
        undefinevmBuilder(&undefinevm, d);

        this->Sandbox(&destroyvm[0]);
        this->Sandbox(&undefinevm[0]);

        goto EXIT_DESTROY;
    EXIT_DESTROY:   
        if (hostname != NULL){
            delete [] hostname;
        }
        if (d != NULL){
            delete d;
        }
        if (bridge != NULL){
            delete bridge;
        }
    }

    return retval;
}

char * sshcmd(const char * ip, std::vector<const char *> * cmdbuilder){
    size_t len = 0;
    int CMD_MAX = 8000;
    char * buff = new char[CMD_MAX];

    len += snprintf(buff + len, CMD_MAX - len, "ssh -t dc-user@%s '", ip);
    for (size_t i = 0; i < cmdbuilder->size(); i++){
        if (i == 0){
            len += snprintf(buff + len, CMD_MAX - len, "%s ", (*cmdbuilder)[i]);
        }
        else {
            len += snprintf(buff + len, CMD_MAX - len, "; %s ", (*cmdbuilder)[i]);
        }
        if (len == 0){
            delete [] buff;
            buff = NULL;
            break;
        }
    }
    if (buff != NULL){
        len += snprintf(buff + len, CMD_MAX - len, "'");
    }

    if (len > strlen(buff)){
        delete [] buff;
        return NULL;
    }
    return buff;
}
