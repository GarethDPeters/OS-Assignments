#include "ram.h"

FILE *ram[RAM_SIZE];

void ram_Init(void)
{
    for (int i = 0; i < RAM_SIZE; i++)
    {
        ram[i] = NULL;
    }
}

int checkRAM(void)
{
    for (int id = 0; id < RAM_SIZE; id++)
    {
        if (ram[id] == NULL)
        {
            return id;
        }
    }
    return -1;
}

void addToRAM(int id, FILE *file)
{
    ram[id] = file;
}

void ram_Remove_PCB(int *pageTable)
{
    for (int i = 0; i < RAM_SIZE; i++)
    {
        int frame = pageTable[i];
        if (frame != -1)
        {
            ram[frame] = NULL;
        }
    }
}