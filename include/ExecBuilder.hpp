#ifndef EXEC_BUILDER_H
#define EXEC_BUILDER_H

#include <vector>
#include "Deployment.hpp"
#include "Network.hpp"

void createlinkBuilder(std::vector<char *> *cmd, Network *n);
void addfdbBuilder(std::vector<char *> *cmd, Network *n);
void createbridgeBuilder(std::vector<char *> *cmd, Network *n);
void setmasterBuilder(std::vector<char *> *cmd, Network *n);
void upinterfaceBuilder(std::vector<char *> *cmd, Network *n);
void upbridgeBuilder(std::vector<char *> *cmd, Network *n);
void spawnvmBuilder(std::vector<char *> *cmd, Deployment *d);
void destroyvmBuilder(std::vector<char *> *cmd, Deployment *d);
void undefinevmBuilder(std::vector<char *> *cmd, Deployment *d);
void deleteinterfaceBuilder(std::vector<char *> *cmd, Deployment *d);
void deletebridgeBuilder(std::vector<char *> *cmd, Deployment *d);

#endif