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

bool run(ERROR_CODE *error)
{
    static int line_count = 0;
    static PCB *current;
    bool done = false;

    if (line_count == 0)
    {
        current = malloc(sizeof(PCB));
        current = CPU_Dequeue();
        cpu.IP = current->PC;
    }

    if (line_count < cpu.quanta)
    {
        if (PCB_Read_Line(cpu.IP, cpu.IR))
        {
            *error = parse(cpu.IR);
            errorCheck(*error);
            if (*error == ERROR_CODE_EXIT)
            {
                CPU_Init(QUANTA_LENGTH);
                done = true;
            }
            else
            {
                line_count++;
                *error = ERROR_CODE_NONE;
            }
        }
        else
        {
            line_count = 0;
            PCB_Close_PCB(current);
            done = true;
        }
    }
    else
    {
        current->PC = cpu.IP;
        CPU_Enqueue(current);
        free(current);
        line_count = 0;
    }
    return done;
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