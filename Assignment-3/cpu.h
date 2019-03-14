
#ifndef cpu_h
#define cpu_h

#include <stdbool.h>
#include "shell.h"
#include "pcb.h"

#define QUANTA_LENGTH 2

typedef enum
{
    CPU_READING,
    CPU_ENQUEUE,
    CPU_EOF
} FILE_STATE;

void CPU_Init(int quanta);
void CPU_Enqueue(PCB *pcb);
void CPU_SetIP(FILE *pcb);
FILE *CPU_getIP(void);
PCB *CPU_Dequeue(void);

FILE_STATE run(ERROR_CODE *error);
bool CPU_IsEmpty(void);

#endif