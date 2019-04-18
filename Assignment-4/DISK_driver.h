#ifndef disk_driver_h
#define disk_driver_h

#include <stdio.h>

void initIO(void);
void initPartition(void);
void initFAT(void);
int partition(char *name, int blocksize, int totalblocks);
int mount(char *name);
int openfile(char *name);
int loadFile(char *name);
int readBlock(int file);
int writeBlock(int file, char **data);
char *returnBlock(void);
int isOpen(int id);
int closeFile(int id);
#endif