#include <stdio.h>

#include "ram.h"
#include "shell.h"

#define RAM_SIZE 10

static FILE* ram[RAM_SIZE];

void ram_Init(void)
{
    for(int i = 0; i < RAM_SIZE; i++)
    {
        ram[i] = NULL;
    }
}

int ram_Add_PCB(char* filename)
{
    FILE* file = fopen(filename, "r");
    int id;

    for(id = 0; id < RAM_SIZE; id++)
    {
        if(ram[id] == NULL)
        {
            ram[id] = file;
            break;
        }
    }

    return id;
}

char* ram_Read_Line(int id, char* input)
{
    return fgets(input, INPUT_LENGTH-1, ram[id]);
}

void ram_Remove_PCB(int id)
{
    fclose(ram[id]);
    ram[id] = NULL;
}
