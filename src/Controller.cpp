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

        Deployment *d;

        char *hostname;
        Network * n;

        FILE * fp;
        char cmd[1500];
        const char * ip = "192.168.1.102";
        const char * cmd1 = "sudo ip link add vxlan1 type vxlan id 1 dev eth0 dstport 0";
        const char * cmd2 = "sudo bridge fdb append to 00:00:00:00:00:00 dst 192.168.1.14 dev vxlan1";
        const char * cmd3 = "sudo ip addr add 192.168.200.1/24 dev vxlan1";
        const char * cmd4 = "sudo ip link set up dev vxlan1";
        sprintf(cmd, "ssh -t dc-user@%s '%s ; %s ; %s ; %s'", ip, cmd1, cmd2, cmd3, cmd4);
        int iteration = 0;

        // Build deployment object
        if ((d = ParseDeployment(dpath)) == NULL)
        {
            printf("Error: parsing deployment");
            retval = EXIT_FAILURE;
            goto EXIT_DEPLOY;
        }

        n = new Network();

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
        createbridgeBuilder(&createbridge, n);

        //ip link set br-vxlan1 up
        upbridgeBuilder(&upbridge, n);

        //virt-install
        spawnvmBuilder(&spawnvm, d);


        if (is_interface_online(n->bridge) == 0) {
            this->Sandbox(&createbridge[0]);
            this->Sandbox(&upbridge[0]);

            printf("started interface");
        }
        this->Sandbox(&spawnvm[0]);


        // TODO
        // CLEAN
        
        while (true){
            std::cout << "Iteration: " << iteration << std::endl;
            if (!(fp =  popen(cmd, "r"))) {
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
        delete [] hostname;
        if (d != NULL){
            delete d;
        }
        if (n != NULL){
            delete n;
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
        Network * n;

        if ((d = ParseDeployment(dpath)) == NULL) {
            printf("Error: parsing deployment");
            retval = EXIT_FAILURE;
            goto EXIT_DESTROY;
        }

        n = new Network();
        
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
        delete [] hostname;

        if (d != NULL){
            delete d;
        }
        if (n != NULL){
            delete n;
        }
    }

    return retval;
}