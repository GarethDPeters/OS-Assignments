#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "shell.h"
#include "interpreter.h"
#include "shellmemory.h"
#include "kernel.h"
#include "ram.h"
#include "memorymanager.h"
#include "DISK_driver.h"
#include "IOSCHEDULER.h"

static ERROR_CODE help(void);
static ERROR_CODE quit(void);
static ERROR_CODE set(char *words[]);
static ERROR_CODE print(char *words[]);
static ERROR_CODE runFile(char *words[]);
static ERROR_CODE exec(char *words[]);
static ERROR_CODE mount_int(char *words[]);
static ERROR_CODE write(char *words[]);
static ERROR_CODE read(char *words[]);

// Prints all possible commands in the shell
static ERROR_CODE help(void)
{
    ERROR_CODE error = ERROR_CODE_NONE;

    printf("Legal commands:\n");
    printf("help.........................Display this help\n");
    printf("quit.........................Exits the shell\n");
    printf("set VAR STRING...............Assign STRING to VAR\n");
    printf("print VAR....................Display contents of VAR\n");
    printf("run SCRIPT.TXT...............Interpret SCRIPT.TXT\n");
    printf("exec a1.out a2.out...........Executes up to three programs in parallel\n");
    printf("Mount part #blks blk_sz......Mounts a partition to be accessed\n");
    printf("Write file [words]...........Writes words into the corresponding file\n");
    printf("Read file var................Reads from the specified file\n");

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
    int launch = 0;

    if (words[1] != NULL)
    {
        FILE *file = fopen(words[1], "r");

        if (file)
            launch |= launcher(file);
        else
            errorCheck(ERROR_CODE_FILE_DNE);

        if (words[2] != NULL)
        {
            FILE *file = fopen(words[2], "r");
            if (file)
                launch |= launcher(file);
            else
                errorCheck(ERROR_CODE_FILE_DNE);
        }
        if (words[3] != NULL)
        {
            FILE *file = fopen(words[3], "r");
            if (file)
                launch |= launcher(file);
            else
                errorCheck(ERROR_CODE_FILE_DNE);
        }
        if (launch)
            error = scheduler();
    }
    else
    {
        error = ERROR_CODE_CMD_UNKNOWN;
    }

    return error;
}

static ERROR_CODE mount_int(char *words[])
{
    ERROR_CODE error = ERROR_CODE_NONE;
    if (words[1] != NULL && words[2] != NULL && words[3] != NULL)
    {
        char *filename = malloc(strlen("PARTITION/" + strlen(words[1]) + 1));
        strcpy(filename, "PARTITION/");
        strcat(filename, words[1]);
        FILE *part = fopen(filename, "r");
        free(filename);
        if (part)
        {
            fclose(part);
            if (!mount(words[1]))
            {
                error = ERROR_CODE_MOUNT_FAIL;
            }
        }
        else
        {
            if (partition(words[1], atoi(words[3]), atoi(words[2])))
            {
                if (!mount(words[1]))
                {
                    error = ERROR_CODE_MOUNT_FAIL;
                }
            }
            else
            {
                error = ERROR_CODE_CREATE_PART;
            }
        }
    }
    else
    {
        error = ERROR_CODE_CMD_UNKNOWN;
    }

    return error;
}

static ERROR_CODE write(char *words[])
{
    ERROR_CODE error = ERROR_CODE_NONE;
    if (words[1] != NULL && words[2] != NULL)
    {
        int file_no = openfile(words[1]);
        if (!isOpen(file_no))
        {
            loadFile(words[1]);
            file_no = openfile(words[1]);
        }
        char *input = malloc(sizeof(char) * 2000);
        strcpy(input, words[2]);
        for (int i = 3; i < WORD_LENGTH; i++)
        {
            if (words[i] != NULL)
            {
                strcat(input, " ");
                strcat(input, words[i]);
            }
        }
        if (!IOscheduler(input, NULL, 1, file_no))
            error = ERROR_CODE_WRITE_FAIL;

        free(input);
        closeFile(file_no);
    }
    else
    {
        error = ERROR_CODE_CMD_UNKNOWN;
    }

    return error;
}

static ERROR_CODE read(char *words[])
{
    ERROR_CODE error = ERROR_CODE_NONE;
    if (words[1] != NULL && words[2] != NULL)
    {
        int file_no = openfile(words[1]);
        if (isOpen(file_no))
        {
            char *output = NULL;
            output = IOscheduler(NULL, NULL, 0, file_no);
            if (output != NULL)
            {
                char *var[3] = {"", words[2], output};
                error = set(var);
                free(output);
            }
            else
            {
                error = ERROR_CODE_READ_FAIL;
            }

            if (!output)
            {
                error = ERROR_CODE_READ_FAIL;
            }
            closeFile(file_no);
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
    else if (strcmp(words[0], "Mount") == 0 && words[4] == NULL)
        error = mount_int(words);
    else if (strcmp(words[0], "Write") == 0)
        error = write(words);
    else if (strcmp(words[0], "Read") == 0 && words[3] == NULL)
        error = read(words);
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
    case ERROR_CODE_CREATE_PART:
        printf("Partition file creation failure (File is corrupted or block number is larger than 10)\n");
        break;
    case ERROR_CODE_MOUNT_FAIL:
        printf("Mounting failure\n");
        break;
    case ERROR_CODE_READ_FAIL:
        printf("Reading of file failure\n");
        break;
    case ERROR_CODE_WRITE_FAIL:
        printf("Writing of file failure\n");
        break;
    case ERROR_CODE_NONE:
    default:
        break;
    }
    return exit;
}