
#ifndef shell_h
#define shell_h

typedef enum{
    ERROR_CODE_NONE,
    ERROR_CODE_EXIT,
    ERROR_CODE_VAR_DNE,
    ERROR_CODE_CMD_UNKNOWN,
    ERROR_CODE_FILE_DNE
}ERROR_CODE;

ERROR_CODE parse(char ui[]);

#endif