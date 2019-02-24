
#ifndef shell_h
#define shell_h

typedef enum{
    ERROR_CODE_NONE,
    ERROR_CODE_EXIT,
    ERROR_CODE_VAR_DNE,
    ERROR_CODE_CMD_UNKNOWN,
    ERROR_CODE_FILE_DNE
}ERROR_CODE;

#define INPUT_LENGTH 1000
#define WORD_LENGTH 100

ERROR_CODE parse(char ui[]);

#endif