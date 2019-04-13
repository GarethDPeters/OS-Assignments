
#ifndef memory_manager_h
#define memory_manager_h

#include <stdio.h>

#include "pcb.h"

int launcher(FILE *source);
int countTotalPages(FILE *f);
FILE *findPage(int pageNumber, FILE *f);
int memory_Update(PCB *pcb);
void memorymanager_Init(void);

#endif