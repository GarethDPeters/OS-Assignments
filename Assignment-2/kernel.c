#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "ram.h"

int main(void) 
{
    //Declaration of prompt character array, and input character array
    char prompt[10] = {'$',' ', '\0'};
    char input[INPUT_LENGTH];
    int exit = 1;
    printf("Kernel 1.0 loaded!\n");
    printf("Welcome to the Gareth Peters shell!\n");
    printf("Version 2.0 updated February 2019\n");
    
    ram_init();
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