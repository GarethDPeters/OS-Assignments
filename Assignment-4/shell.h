
#ifndef shell_h
#define shell_h

#include <stdbool.h>

typedef enum
{
    ERROR_CODE_NONE,
    ERROR_CODE_EXIT,
    ERROR_CODE_VAR_DNE,
    ERROR_CODE_CMD_UNKNOWN,
    ERROR_CODE_FILE_DNE,
    ERROR_CODE_CREATE_PART,
    ERROR_CODE_MOUNT_FAIL,
    ERROR_CODE_WRITE_FAIL,
    ERROR_CODE_READ_FAIL
} ERROR_CODE;

#define INPUT_LENGTH 1000
#define WORD_LENGTH 100

bool shell_Prompt(void);
ERROR_CODE parse(char ui[]);

#endif