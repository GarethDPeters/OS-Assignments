#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "memorymanager.h"
#include "shell.h"
#include "cpu.h"
#include "ram.h"
#include "kernel.h"

static int copyFileToBackingStore(FILE *source, char *path);
static int findFrame(FILE *page);
static int findVictim(PCB *p);
static int updateFrame(int frameNumber, int victimFrame, FILE *page);
static int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame);

const char *BACKING_STORE = "BackingStore/";
static int memoryIndex;

void memorymanager_Init(void)
{
    memoryIndex = 1;
}

int launcher(FILE *source)
{
    char filename[256];
    sprintf(filename, "process%d.txt", memoryIndex++);
    char *path = malloc(strlen(BACKING_STORE) + strlen(filename) + 1);
    strcpy(path, BACKING_STORE);
    strcat(path, filename);
    int success = copyFileToBackingStore(source, path);

    if (success)
    {
        FILE *file = fopen(path, "r");
        if (file)
        {
            success &= myinit(file);
        }
        else
        {
            success = 0;
        }
    }

    return success;
}

static int copyFileToBackingStore(FILE *source, char *path)
{
    char input[INPUT_LENGTH];
    FILE *target = fopen(path, "a");
    int success = 1;

    if (target)
    {
        while (fgets(input, INPUT_LENGTH - 1, source))
        {
            fputs(input, target);
        }
    }
    else
    {
        success = 0;
    }

    fclose(target);
    fclose(source);
    return success;
}

int countTotalPages(FILE *f)
{
    int lines = 0;
    FILE *copy = fdopen(dup(fileno(f)), "r");
    while (!feof(copy))
    {
        if (fgetc(copy) == '\n')
            lines++;
    }
    fseek(copy, 0L, SEEK_SET);
    fclose(copy);
    return (lines / PAGE_SIZE) + 1;
}

FILE *findPage(int pageNumber, FILE *f)
{
    FILE *copy = fdopen(dup(fileno(f)), "r");
    for (int i = 0; i < PAGE_SIZE * pageNumber; i++)
    {
        while (fgetc(copy) != '\n')
        {
        }
    }
    return copy;
}

static int findFrame(FILE *page)
{
    return checkRAM();
}

static int findVictim(PCB *p)
{
    srand(time(NULL));
    int randomFrame = rand() % RAM_SIZE;
    for (int i = 0; i < RAM_SIZE; i++)
    {
        if (p->pageTable[i] == randomFrame)
        {
            randomFrame = (randomFrame + 1) % RAM_SIZE;
            i = -1;
        }
    }
    return randomFrame;
}

static int updateFrame(int frameNumber, int victimFrame, FILE *page)
{
    if (frameNumber < RAM_SIZE && victimFrame < RAM_SIZE)
    {
        if (frameNumber == -1)
        {
            addToRAM(victimFrame, page);
        }
        else
        {
            addToRAM(frameNumber, page);
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

static int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame)
{
    if (pageNumber < RAM_SIZE && frameNumber < RAM_SIZE && victimFrame < RAM_SIZE)
    {
        if (frameNumber == -1)
        {
            p->pageTable[pageNumber] = victimFrame;
        }
        else
        {
            p->pageTable[pageNumber] = frameNumber;
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

int memory_Update(PCB *pcb)
{
    int success = 1;
    int frameNumber = findFrame(pcb->PC);
    int victimNumber = -1;
    if (frameNumber == -1)
    {
        victimNumber = findVictim(pcb);
    }

    success &= updateFrame(frameNumber, victimNumber, pcb->PC);
    success &= updatePageTable(pcb, pcb->PC_page, frameNumber, victimNumber);
    if (victimNumber != -1)
    {
        PCB *victim = NULL;
        int victimPageNumber = -1;
        victim = CPU_findVictim(&victimPageNumber, victimNumber);
        if (victimPageNumber != -1)
        {
            success &= updatePageTable(victim, victimPageNumber, -1, -1);
        }
        else
        {
            success = 0;
        }
    }
    return success;
}
