
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"
#include "kernel.h"
#include "ram.h"

static int addToRAM(FILE *p);
static PCB *makePCB(FILE *p, int id);
static void addToReady(PCB *p);

int main(void)
{
    //Declaration of prompt character array, and input character array
    char prompt[10] = {'$', ' ', '\0'};
    char input[INPUT_LENGTH];
    int exit = 1;
    printf("Kernel 1.0 loaded!\n");
    printf("Welcome to the Gareth Peters shell!\n");
    printf("Version 2.0 updated February 2019\n");

    ram_Init();
    CPU_Init(QUANTA_LENGTH);
    while (exit)
    {
        ERROR_CODE errCode = ERROR_CODE_NONE;

        //If STDIN is from the terminal, then print the $ to wait for more input
        if (isatty(fileno(stdin)))
        {
            printf("%s", prompt);
            fflush(stdout);
        }

        //Check if fgets is NULL. If it is, then force quit. If it isn't, then parse the input
        if (fgets(input, INPUT_LENGTH - 1, stdin))
        {
            //Print commands from a piped input
            if (!isatty(fileno(stdin)))
            {
                printf("%s%s", prompt, input);
                fflush(stdout);
            }
            errCode = parse(input);
            exit = errorCheck(errCode);
        }
        else
        {
            freopen("/dev/tty", "r", stdin);
        }
    }
    printf("Goodbye!\n");

    return 1;
}

/*int main(void)
{
    PCB *pcb1 = malloc(sizeof(PCB));
    PCB *pcb2 = malloc(sizeof(PCB));
    PCB *pcb3 = malloc(sizeof(PCB));
    PCB *pcb4 = malloc(sizeof(PCB));

    pcb1->PC = (FILE *)0x10051b2e0;
    pcb2->PC = (FILE *)0x10051b2e1;

    CPU_Enqueue(pcb1);
    printf("PCB1 enqueued\n");
    CPU_Enqueue(pcb2);
    printf("PCB2 enqueued\n");

    pcb3 = CPU_Dequeue();
    printf("PCB1 dequeued into PCB3\n");
    pcb4 = CPU_Dequeue();
    printf("PCB2 dequeued into PCB4\n");

    CPU_Enqueue(pcb1);
    printf("PCB1 enqueued\n");
    CPU_Enqueue(pcb2);
    printf("PCB2 enqueued\n");

    pcb3 = CPU_Dequeue();
    printf("PCB1 dequeued into PCB3\n");
    pcb4 = CPU_Dequeue();
    printf("PCB2 dequeued into PCB4\n");

    printf("FILE: %p\n", pcb1->PC);
    printf("FILE: %p\n", pcb2->PC);
    printf("FILE: %p\n", pcb3->PC);
    printf("FILE: %p\n", pcb4->PC);
}*/

void myinit(FILE *p)
{
    int id = addToRAM(p);
    PCB *pcb = makePCB(p, id);
    addToReady(pcb);
}

ERROR_CODE scheduler(void)
{
    ERROR_CODE error = ERROR_CODE_NONE;
    while (!CPU_IsEmpty())
    {
        while (1)
        {
            if (run(&error))
                break;
        }
    }

    return error;
}

static int addToRAM(FILE *p)
{
    return ram_Add_PCB(p);
}

static PCB *makePCB(FILE *p, int id)
{
    return PCB_Create_PCB(p, id);
}

static void addToReady(PCB *p)
{
    CPU_Enqueue(p);
}
