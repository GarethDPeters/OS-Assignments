#include <stdlib.h>
#include <string.h>

#include "IOSCHEDULER.h"
#include "DISK_driver.h"

#define IO_QUEUE_SIZE 10

typedef struct IO_Node
{
    char *data;
    PCB *ptr;
    int cmd;
    int file;
} Node;

struct IO_Queue
{
    Node arr[IO_QUEUE_SIZE];
    int front;
    int rear;
    int size;
} queue;

static void ioNode_Init(int index);
static Node IO_dequeue(void);
static void IO_enqueue(Node io_node);
static int IsEmpty(void);
static int IsFull(void);

void IOscheduler_Init(void)
{
    queue.front = 0;
    queue.rear = IO_QUEUE_SIZE - 1;
    queue.size = 0;
    for (int i = 0; i < IO_QUEUE_SIZE; i++)
    {
        ioNode_Init(i);
    }
}
char *IOscheduler(char *data, PCB *ptr, int cmd, int file)
{
    Node new;
    new.cmd = cmd;
    new.ptr = ptr;
    if (data != NULL)
    {
        new.data = malloc(strlen(data) + 1);
        strcpy(new.data, data);
    }
    new.file = file;
    if (IsFull())
    {
        return NULL;
    }
    IO_enqueue(new);
    char *output = malloc(1000);
    strcpy(output, "");
    while (1)
    {
        if (IsEmpty())
        {
            return NULL;
        }
        Node pop = IO_dequeue();
        // Cmd = 0 for read, Cmd = 1 for write
        if (pop.cmd == 0)
        {
            int state = readBlock(pop.file);

            if (state == 1)
            {
                strcat(output, returnBlock());
            }
            else if (state == 0)
            {
                strcat(output, returnBlock());
                return output;
            }
            else if (state == -1)
            {
                return NULL;
            }
        }
        else if (pop.cmd == 1)
        {
            int state = writeBlock(pop.file, &pop.data);
            if (state == 0)
            {
                return "";
            }
            else if (state == -1)
            {
                return NULL;
            }
        }
        if (IsFull())
        {
            return NULL;
        }
        IO_enqueue(pop);
    }
}

static void ioNode_Init(int index)
{
    queue.arr[index].data = NULL;
    queue.arr[index].ptr = NULL;
    queue.arr[index].cmd = -1;
    queue.arr[index].file = -1;
}

static Node IO_dequeue(void)
{
    Node pop = queue.arr[queue.front];
    ioNode_Init(queue.front);

    queue.size--;
    queue.front = (queue.front + 1) % IO_QUEUE_SIZE;
    return pop;
}

static void IO_enqueue(Node io_node)
{
    queue.rear = (queue.rear + 1) % IO_QUEUE_SIZE;
    queue.arr[queue.rear] = io_node;
    queue.size++;
}

static int IsEmpty(void)
{
    return queue.size == 0;
}

static int IsFull(void)
{
    return queue.size == IO_QUEUE_SIZE;
}