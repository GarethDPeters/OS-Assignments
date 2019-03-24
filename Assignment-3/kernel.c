#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "interpreter.h"
#include "kernel.h"
#include "cpu.h"
#include "memorymanager.h"

static void boot(void);
static void addToReady(PCB *p);
static void backingStore_Init(void);

int main(void)
{
    //Declaration of prompt character array, and input character array
    printf("Kernel 1.0 loaded!\n");
    printf("Welcome to the Gareth Peters shell!\n");
    printf("Version 2.0 updated February 2019\n");

    boot();
    while (shell_Prompt())
    {
    }
    printf("Goodbye!\n");

    return 1;
}

static void boot(void)
{
    ram_Init();
    CPU_Init(QUANTA_LENGTH);
    backingStore_Init();
    memorymanager_Init();
}

int myinit(FILE *p)
{
    int success = 1;
    int numPages = countTotalPages(p);
    if (numPages < RAM_SIZE)
    {
        PCB *pcb = makePCB(p, numPages);
        if (pcb->pages_max == 1)
        {
            success &= memory_Update(pcb);
        }
        else
        {
            success &= memory_Update(pcb);
            pcb->PC_page++;
            success &= memory_Update(pcb);
            pcb->PC_page = 0;
        }
        if (success)
        {
            addToReady(pcb);
        }
        else
        {
            free(pcb);
        }
    }
    else
    {
        success = 0;
    }

    return success;
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
        CPU_SetOffset(current->PC_offset);
        while (error == ERROR_CODE_NONE)
        {
            if (current->pageTable[current->PC_page] != -1)
            {
                state = run(&error);
                if (state == CPU_ENQUEUE)
                {
                    current->PC = CPU_getIP();
                    current->PC_offset = CPU_getOffset();
                    CPU_Enqueue(current);
                    break;
                }
                else if (state == CPU_PAGEFAULT)
                {
                    current->PC = CPU_getIP();
                    current->PC_offset = 0;
                    current->PC_page++;

                    if (current->PC_page >= RAM_SIZE)
                    {
                        PCB_Close_PCB(current);
                        break;
                    }
                    if (current->pageTable[current->PC_page] == -1)
                    {
                        memory_Update(current);
                    }
                    CPU_SetOffset(0);
                    CPU_Enqueue(current);
                    break;
                }
                else if (state == CPU_EOF)
                {
                    PCB_Close_PCB(current);
                    break;
                }
            }
            else
            {
                memory_Update(current);
            }
        }
    }
    memorymanager_Init();
    backingStore_Init();

    return error;
}

static void addToReady(PCB *p)
{
    CPU_Enqueue(p);
}

static void backingStore_Init(void)
{
    DIR *dir = opendir("BackingStore");

    if (dir)
    {
        system("exec rm -r BackingStore");
    }
    system("mkdir BackingStore");

    closedir(dir);
}
