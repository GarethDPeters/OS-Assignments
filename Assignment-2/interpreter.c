#include <string.h>
#include <stdio.h>

#include "shell.h"
#include "interpreter.h"
#include "shellmemory.h"
#include "kernel.h"
#include "ram.h"

static ERROR_CODE help(void);
static ERROR_CODE quit(void);
static ERROR_CODE set(char *words[]);
static ERROR_CODE print(char *words[]);
static ERROR_CODE runFile(char *words[]);
static ERROR_CODE exec(char *words[]);

// Prints all possible commands in the shell
static ERROR_CODE help(void)
{
    ERROR_CODE error = ERROR_CODE_NONE;

    printf("Legal commands:\n");
    printf("help....................Display this help\n");
    printf("quit....................Exits the shell\n");
    printf("set VAR STRING..........Assign STRING to VAR\n");
    printf("print VAR...............Display contents of VAR\n");
    printf("run SCRIPT.TXT..........Interpret SCRIPT.TXT\n");
    printf("exec a1.out a2.out......Executes up to three programs in parallel\n");

    return error;
}

// Returns the exit error code to exit the program
static ERROR_CODE quit(void)
{
    return ERROR_CODE_EXIT;
}

// Assigns a string value to a string variable. Stores this in the shellmemory.c linked list
// Returns an error if the syntax for the set command is not recognizable.
static ERROR_CODE set(char *words[])
{
    ERROR_CODE error = ERROR_CODE_NONE;

    if (words[1] != NULL && words[2] != NULL)
        error = addItem(words[1], words[2]);
    else
        error = ERROR_CODE_CMD_UNKNOWN;

    return error;
}

// Prints the value of the variable state in the print command. Returns and error if variable does not exist or if syntax is not recognizable.
static ERROR_CODE print(char *words[])
{
    char *varValue = NULL;
    ERROR_CODE error = ERROR_CODE_NONE;

    if (words[1] != NULL)
        error = findItem(words[1], &varValue);
    else
        error = ERROR_CODE_CMD_UNKNOWN;

    if (error == ERROR_CODE_NONE)
        printf("%s = %s\n", words[1], varValue);

    return error;
}

// Helper function for the run command that calls the interpreter similarly to the main function
static ERROR_CODE runFile(char *words[])
{
    ERROR_CODE error = ERROR_CODE_NONE;

    if (words[1] != NULL)
    {
        FILE *file = fopen(words[1], "r");
        if (file)
        {
            char input[INPUT_LENGTH];

            while (fgets(input, INPUT_LENGTH, file))
            {
                printf("$ %s", input);
                fflush(stdout);
                error = parse(input);
                errorCheck(error);
                if (error == ERROR_CODE_EXIT)
                    break;
                else
                    error = ERROR_CODE_NONE;
            }
            fclose(file);
        }
        else
        {
            error = ERROR_CODE_FILE_DNE;
        }
    }
    else
    {
        error = ERROR_CODE_CMD_UNKNOWN;
    }

    return error;
}

static ERROR_CODE exec(char *words[])
{
    ERROR_CODE error = ERROR_CODE_NONE;

    if (words[1] != NULL)
    {
        FILE *file = fopen(words[1], "r");
        if (file)
            myinit(file);
        else
            errorCheck(ERROR_CODE_FILE_DNE);

        if (words[2] != NULL)
        {
            if (strcmp(words[1], words[2]) != 0)
            {
                FILE *file = fopen(words[2], "r");
                if (file)
                    myinit(file);
                else
                    errorCheck(ERROR_CODE_FILE_DNE);
            }
            else
            {
                printf("Error: Script %s already loaded. \n", words[2]);
            }
        }
        if (words[3] != NULL)
        {
            if ((strcmp(words[1], words[3]) != 0) && (strcmp(words[2], words[3]) != 0))
            {
                FILE *file = fopen(words[3], "r");
                if (file)
                    myinit(file);
                else
                    errorCheck(ERROR_CODE_FILE_DNE);
            }
            else
            {
                printf("Error: Script %s already loaded. \n", words[3]);
            }
        }

        printf("Here\n");
        error = scheduler();
    }
    else
    {
        error = ERROR_CODE_CMD_UNKNOWN;
    }

    return error;
}

// Chooses which command to run and returns an error if command does not exist.
int interpreter(char *words[])
{
    ERROR_CODE error = ERROR_CODE_NONE;

    if (strcmp(words[0], "help") == 0 && words[1] == NULL)
        error = help();
    else if (strcmp(words[0], "quit") == 0 && words[1] == NULL)
        error = quit();
    else if (strcmp(words[0], "run") == 0 && words[2] == NULL)
        error = runFile(words);
    else if (strcmp(words[0], "print") == 0 && words[2] == NULL)
        error = print(words);
    else if (strcmp(words[0], "set") == 0 && words[3] == NULL)
        error = set(words);
    else if (strcmp(words[0], "exec") == 0 && words[4] == NULL)
        error = exec(words);
    else
        error = ERROR_CODE_CMD_UNKNOWN;

    return error;
}

// Checks the error code of the input. Prints an error if needed and returns an exit if EXIT error is received.
int errorCheck(int error)
{
    int exit = 1;
    switch (error)
    {
    case ERROR_CODE_CMD_UNKNOWN:
        printf("Unknown command\n");
        break;
    case ERROR_CODE_EXIT:
        exit = 0;
        break;
    case ERROR_CODE_VAR_DNE:
        printf("Variable does not exist.\n");
        break;
    case ERROR_CODE_FILE_DNE:
        printf("File does not exist.\n");
        break;
    case ERROR_CODE_NONE:
    default:
        break;
    }
    return exit;
}