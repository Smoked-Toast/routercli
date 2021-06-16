#ifndef LOCK_H
#define LOCK_H

int createLock(char *hostname, char *lpath);
int deleteLock(char * hostname, char * lpath);
char *getLockHostname(char *lpath);
char * getHostname();

#endif