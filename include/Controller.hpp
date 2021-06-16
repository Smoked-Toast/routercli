#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Command.hpp"

class Controller {
    public:
        //Constructor
        Controller();

        //Process the cmd line args
        int execute(int argc, char * argv[]);

    private:

        //Print usage for main interface
        void printUsage(char * name);

        //Execute in a seperate process
        int Sandbox(char * argv[]);

};

#endif