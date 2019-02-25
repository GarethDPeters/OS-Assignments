
#ifndef cpu_h
#define cpu_h

#include <stdbool.h>
#include "shell.h"
#include "pcb.h"

#define QUANTA_LENGTH 2

void CPU_Init(int quanta);
void CPU_Enqueue(PCB *pcb);
PCB *CPU_Dequeue(void);
bool run(ERROR_CODE *error);
bool CPU_IsEmpty(void);

#endif