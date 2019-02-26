#include <stdlib.h>

#include "shell.h"
#include "pcb.h"
#include "ram.h"

PCB *makePCB(FILE *file, int id)
{
    PCB *pcb = malloc(sizeof(PCB));
    pcb->PC = file;
    pcb->id = id;
    return pcb;
}

char *PCB_Read_Line(FILE *file, char *input)
{
    return fgets(input, INPUT_LENGTH - 1, file);
}

void PCB_Close_PCB(PCB *pcb)
{
    ram_Remove_PCB(pcb->id);
    free(pcb);
}