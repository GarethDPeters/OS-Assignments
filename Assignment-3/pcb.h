
#ifndef pcb_h
#define pcb_h

#include <stdio.h>

#include "ram.h"

#define PAGE_SIZE 4

typedef struct PCB
{
    FILE *PC;
    int pageTable[RAM_SIZE];
    int PC_page, PC_offset, pages_max;
} PCB;

PCB *makePCB(FILE *file, int page_count);
void PCB_Close_PCB(PCB *pcb);

#endif