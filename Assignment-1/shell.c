#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "interpreter.h"
#include "shellmemory.h"

int main(void) 
{
    //Declaration of prompt character array, and input character array
    char prompt[10] = {'$',' ', '\0'};
    char input[INPUT_LENGTH];
    int exit = 1;
    printf("Welcome to the Gareth Peters shell!\n");
    printf("Version 1.0 create January 2019\n");

    while(exit)
    {
        ERROR_CODE errCode = ERROR_CODE_NONE;

        //If STDIN is from the terminal, then print the $ to wait for more input
        if(isatty(fileno(stdin)))
        {
            printf("%s", prompt);
            fflush(stdout);
        }
        
        //Check if fgets is NULL. If it is, then force quit. If it isn't, then parse the input
        if(fgets(input, INPUT_LENGTH-1, stdin))
        {
            //Print commands from a piped input
            if(!isatty(fileno(stdin)))
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

// Parses the input character array to return an array of words.
ERROR_CODE parse(char ui[])
{
    char* words[WORD_LENGTH] = {NULL};
    char tmp[INPUT_LENGTH];
    int a,b;
    int w = 0;

    for(a=0; ui[a]==' ' && a < INPUT_LENGTH; a++);

    while(ui[a] != '\0' && a<INPUT_LENGTH)
    {
        for(b=0; ui[a]!='\0' && ui[a]!=' ' && ui[a] != '\n' && ui[a] != '\r' && a<INPUT_LENGTH && b<(INPUT_LENGTH-1); a++, b++)
            tmp[b] = ui[a];
        
        tmp[b] = '\0';
        words[w] = strdup(tmp);

        a++; w++;
    }

    return interpreter(words);
}