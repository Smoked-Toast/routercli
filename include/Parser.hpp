#ifndef PARSER_H
#define PARSER_H

#include "Command.hpp"
#include "Deployment.hpp"

bool Testid(std::string id);
Command ParseArgs(int argc, char * argv[]);
Deployment * ParseDeployment(char * file);

#endif