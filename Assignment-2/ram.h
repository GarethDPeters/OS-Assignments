
#ifndef ram_h
#define ram_h

#include <stdio.h>

void ram_Init(void);
int ram_Add_PCB(FILE *file);
char *ram_Read_Line(int id, char *input);
void ram_Remove_PCB(int id);

#endif