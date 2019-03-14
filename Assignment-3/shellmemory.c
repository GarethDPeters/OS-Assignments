#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "shell.h"
#include "shellmemory.h"

static _Bool varUpdate(char *varName, char *varValue);

typedef struct SHELLMEMORY
{
    char *name;
    char *value;
    struct SHELLMEMORY *next;
} Block;

static Block *head = NULL;

// Updates an existing variable varName to be varValue
static _Bool varUpdate(char *varName, char *varValue)
{
    if (head != NULL)
    {
        Block *tmp = head;
        while (tmp != NULL)
        {
            if (strcmp(varName, tmp->name) == 0)
            {
                tmp->value = strdup(varValue);
                printf("Updated %s\n", tmp->name);
                return true;
            }
            tmp = tmp->next;
        }
    }
    return false;
}

// Adds an item to the shell memory. If varName already exists, then it updates varName.
int addItem(char *varName, char *varValue)
{
    ERROR_CODE error = ERROR_CODE_NONE;
    if (head == NULL)
    {
        head = malloc(sizeof(Block));
        head->name = strdup(varName);
        head->value = strdup(varValue);
        head->next = NULL;
    }
    else
    {
        if (varUpdate(varName, varValue))
            ;
        else
        {
            Block *node = malloc(sizeof(Block));
            node->name = strdup(varName);
            node->value = strdup(varValue);
            node->next = head;
            head = node;
        }
    }

    return error;
}

// Finds the value of varName and stores it in varValue address. Returns error if variable does not exist.
int findItem(char *varName, char **varValue)
{
    if (head != NULL)
    {
        Block *tmp = head;
        while (tmp != NULL)
        {
            if (strcmp(varName, tmp->name) == 0)
            {
                *varValue = strdup(tmp->value);
                return ERROR_CODE_NONE;
            }
            tmp = tmp->next;
        }
    }
    return ERROR_CODE_VAR_DNE;
}
