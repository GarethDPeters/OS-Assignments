#include <stdio.h>
#include <stdlib.h>

#include "shell.h"
#include "interpreter.h"
#include "ram.h"
#include "cpu.h"

struct CPU
{
    FILE *IP;
    char IR[INPUT_LENGTH];
    int quanta;
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

FILE_STATE run(ERROR_CODE *error)
{
    static int line_count = 0;
    FILE_STATE done = CPU_READING;

    if (PCB_Read_Line(cpu.IP, cpu.IR))
    {
        printf("$ %s", cpu.IR);
        fflush(stdout);
        *error = parse(cpu.IR);
        errorCheck(*error);
        if (*error == ERROR_CODE_EXIT)
        {
            CPU_Init(QUANTA_LENGTH);
            done = CPU_ENQUEUE;
        }
        else
        {
            line_count++;
            *error = ERROR_CODE_NONE;
            if (line_count == cpu.quanta)
            {
                line_count = 0;
                done = CPU_ENQUEUE;
            }
        }
    }
    else
    {
        line_count = 0;
        done = CPU_EOF;
    }
    return done;
}

FILE *CPU_getIP(void)
{
    return cpu.IP;
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