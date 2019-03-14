#include "ram.h"

#define RAM_SIZE 10

static FILE *ram[RAM_SIZE];

void ram_Init(void)
{
    for (int i = 0; i < RAM_SIZE; i++)
    {
        ram[i] = NULL;
    }
}

int addToRAM(FILE *file)
{
    for (int id = 0; id < RAM_SIZE; id++)
    {
        if (ram[id] == NULL)
        {
            ram[id] = file;
            return id;
        }
    }
}

void ram_Remove_PCB(int id)
{
    fclose(ram[id]);
    ram[id] = NULL;
}