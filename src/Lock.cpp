#include "../include/Lock.hpp"
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

int createLock(char * hostname, char * lpath) {   

    std::ifstream ifile;
    ifile.open(lpath);
    
    // Check if the lock already exists
    if (ifile.good()){
        char * lockname = getLockHostname(lpath);
        // If the lock exists, but it belongs to this computer.
        if (strcmp(lockname, hostname) == 0){
            delete lockname;
            return 1;
        }
        // If the lock exists and it doesnt belong to this computer
        else {
            delete lockname;
            return -1;
        }
    } 
    // Lock does not exist  
    else {
        std::ofstream ofile;
        ofile.open(lpath);
        ofile << "hostname=" << hostname << std::endl;
        return 0;
    }
}

int deleteLock(char * hostname, char * lpath){

    std::ifstream ifile;
    ifile.open(lpath);
    
    // Check if the lock already exists
    if (ifile.good()){
        char * lockname = getLockHostname(lpath);

        // If the lock exists and it belongs to this computer.
        if (strcmp(lockname, hostname) == 0){
            delete lockname;

            int status;
            if((status = remove(lpath)) != 0){
                return -1;
            }
            return 0;
        }
        // If the lock exists, but it doesnt belong to this computer
        else {
            delete lockname;
            return -1;
        }
    } 
    // Lock does not exist  
    else {
        return 1;
    }

}

char * getLockHostname(char * lpath) {

    char * hostname = new char[HOST_NAME_MAX];


    std::fstream newfile;
    newfile.open(lpath, std::ios::in);

    if (newfile.is_open()) {
        std::string line;
        while (getline(newfile, line)) {

            int i = line.find("=");
            if (i >= 1){
                std::string n = line.substr(0, i);
                std::string p = line.substr(i+1);

                if (n == "hostname"){
                    sprintf(hostname, "%s", p.c_str());
                    break;
                }
            }
        }
        newfile.close();
    }
    return hostname;
}

char *getHostname()
{
    char *hostname = new char[HOST_NAME_MAX];
    int retval;
    if ((retval = gethostname(hostname, HOST_NAME_MAX)) == -1)
    {
        delete[] hostname;
        return NULL;
    }
    return hostname;
}