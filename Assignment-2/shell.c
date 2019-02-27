#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "interpreter.h"
#include "shellmemory.h"

// Parses the input character array to return an array of words.
ERROR_CODE parse(char ui[])
{
    char *words[WORD_LENGTH] = {NULL};
    char tmp[INPUT_LENGTH];
    int a, b;
    int w = 0;

    for (a = 0; ui[a] == ' ' && a < INPUT_LENGTH; a++)
        ;

    while (ui[a] != '\0' && a < INPUT_LENGTH)
    {
        for (b = 0; ui[a] != '\0' && ui[a] != ' ' && ui[a] != '\n' && ui[a] != '\r' && a < INPUT_LENGTH && b < (INPUT_LENGTH - 1); a++, b++)
            tmp[b] = ui[a];

        tmp[b] = '\0';
        words[w] = strdup(tmp);

        a++;
        w++;
    }

    return interpreter(words);
}