#include <stdlib.h>

#include "interpreter.h"
#include "kernel.h"
#include "shell.h"
#include "ram.h"

static void addToReady(PCB *p);

int main(void)
{
    //Declaration of prompt character array, and input character array
    printf("Kernel 1.0 loaded!\n");
    printf("Welcome to the Gareth Peters shell!\n");
    printf("Version 2.0 updated February 2019\n");

    ram_Init();
    CPU_Init(QUANTA_LENGTH);
    while (shell_Prompt())
    {
    }
    printf("Goodbye!\n");

    return 1;
}

void myinit(FILE *p)
{
    int id = addToRAM(p);
    PCB *pcb = makePCB(p, id);
    addToReady(pcb);
}

ERROR_CODE scheduler(void)
{
    ERROR_CODE error = ERROR_CODE_NONE;
    PCB *current = malloc(sizeof(PCB));
    while (!CPU_IsEmpty() && error == ERROR_CODE_NONE)
    {
        FILE_STATE state = CPU_READING;
        current = CPU_Dequeue();
        CPU_SetIP(current->PC);
        while (error == ERROR_CODE_NONE)
        {
            state = run(&error);
            if (state == CPU_ENQUEUE)
            {
                current->PC = CPU_getIP();
                CPU_Enqueue(current);
                break;
            }
            else if (state == CPU_EOF)
            {
                PCB_Close_PCB(current);
                break;
            }
        }
    }

    return error;
}

static void addToReady(PCB *p)
{
    CPU_Enqueue(p);
}
