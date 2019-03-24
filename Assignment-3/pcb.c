#include <stdlib.h>

#include "shell.h"
#include "pcb.h"

PCB *makePCB(FILE *file, int page_count)
{
    PCB *pcb = malloc(sizeof(PCB));
    pcb->PC = file;
    for (int i = 0; i < RAM_SIZE; i++)
    {
        pcb->pageTable[i] = -1;
    }
    pcb->PC_offset = 0;
    pcb->PC_page = 0;
    pcb->pages_max = page_count;
    return pcb;
}

void PCB_Close_PCB(PCB *pcb)
{
    ram_Remove_PCB(pcb->pageTable);
    fclose(pcb->PC);
    free(pcb);
}