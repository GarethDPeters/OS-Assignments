#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "interpreter.h"

struct CPU
{
    FILE *IP;
    char IR[INPUT_LENGTH];
    int quanta, offset;
};

typedef struct PCB_Node
{
    PCB *node;
    struct PCB_Node *next;
} Node;

struct Ready_Queue
{
    int length;
    Node *head;
    Node *tail;
};

static struct CPU cpu;
static struct Ready_Queue queue;

void CPU_Init(int quanta)
{
    cpu.quanta = quanta;
    cpu.offset = 0;
    queue.head = NULL;
    queue.tail = NULL;
    queue.length = 0;
}

void CPU_Enqueue(PCB *pcb)
{
    Node *tmp = malloc(sizeof(Node));
    tmp->node = pcb;
    tmp->next = NULL;
    if (queue.head == NULL && queue.tail == NULL)
    {
        queue.head = malloc(sizeof(Node));
        queue.tail = malloc(sizeof(Node));
        queue.head->next = tmp;
        queue.tail->next = tmp;
    }
    else
    {
        queue.tail->next->next = tmp;
        queue.tail->next = queue.tail->next->next;
    }
    queue.length++;
}

void CPU_SetIP(FILE *pc)
{
    cpu.IP = pc;
}

void CPU_SetOffset(int offset)
{
    cpu.offset = offset;
}

FILE_STATE run(ERROR_CODE *error)
{
    FILE_STATE done = CPU_READING;

    if (fgets(cpu.IR, INPUT_LENGTH - 1, cpu.IP))
    {
        printf("$ %s", cpu.IR);
        fflush(stdout);
        *error = parse(cpu.IR);
        errorCheck(*error);
        cpu.offset++;
        if (*error == ERROR_CODE_EXIT)
        {
            CPU_Init(QUANTA_LENGTH);
            done = CPU_READING;
        }
        else
        {
            *error = ERROR_CODE_NONE;
            if (cpu.offset == PAGE_SIZE)
            {
                done = CPU_PAGEFAULT;
            }
            else if (cpu.offset % cpu.quanta == 0)
            {
                done = CPU_ENQUEUE;
            }
        }
    }
    else
    {
        cpu.offset = 0;
        done = CPU_EOF;
    }
    return done;
}

FILE *CPU_getIP(void)
{
    return cpu.IP;
}

int CPU_getOffset(void)
{
    return cpu.offset;
}

PCB *CPU_Dequeue(void)
{
    PCB *pcb = NULL;
    if (!CPU_IsEmpty())
    {
        pcb = queue.head->next->node;
        if (queue.head->next->next == NULL)
        {
            free(queue.head->next);
            free(queue.head);
            free(queue.tail);
            queue.head = NULL;
            queue.tail = NULL;
        }
        else
        {
            Node *tmp = queue.head->next->next;
            free(queue.head->next);
            queue.head->next = tmp;
        }
        queue.length--;
    }
    return pcb;
}

bool CPU_IsEmpty(void)
{
    return queue.length == 0;
}

PCB *CPU_findVictim(int *pageNumber, int victimNumber)
{
    Node *iterator = NULL;
    if (queue.head != NULL && queue.tail != NULL)
    {
        iterator = queue.head->next;
        while (iterator != NULL)
        {
            for (int i = 0; i < RAM_SIZE; i++)
            {
                if (iterator->node->pageTable[i] == victimNumber)
                {
                    *pageNumber = i;
                    return iterator->node;
                }
            }
            iterator = iterator->next;
        }
    }
    *pageNumber = -1;
    return NULL;
}