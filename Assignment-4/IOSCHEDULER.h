#ifndef ioscheduler_h
#define ioscheduler_h

#include "pcb.h"

void IOscheduler_Init(void);
char *IOscheduler(char *data, PCB *ptr, int cmd, int file);

#endif