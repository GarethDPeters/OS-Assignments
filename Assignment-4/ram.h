
#ifndef ram_h
#define ram_h

#include <stdio.h>

#define RAM_SIZE 10

extern FILE *ram[RAM_SIZE];

void ram_Init(void);
void addToRAM(int id, FILE *file);
int checkRAM(void);
void ram_Remove_PCB(int *pageTable);

#endif