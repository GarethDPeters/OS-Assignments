
#ifndef pcb_h
#define pcb_h

#include <stdio.h>

#define MAX_THREAD_COUNT 3

typedef struct PCB
{
    int id;
    FILE *PC;
} PCB;

PCB *makePCB(FILE *file, int id);
char *PCB_Read_Line(FILE *file, char *input);
void PCB_Close_PCB(PCB *pcb);

#endif