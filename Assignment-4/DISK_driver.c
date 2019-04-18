#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "DISK_driver.h"

#define FAT_SIZE 20
#define FILE_SIZE 10
#define OPEN_FILE_SIZE 5

struct PARTITION
{
    char name[256];
    int total_blocks;
    int block_size;
} partit;

struct FAT
{
    char *filename;
    int file_length;
    int blockPtrs[FILE_SIZE];
    int current_location;
    int fppointer;
} fat[FAT_SIZE];

char *block_buffer;
FILE *fp[OPEN_FILE_SIZE];

static void initBufferFP(void);
static void initDirectory(void);

void initIO(void)
{
    initPartition();
    initFAT();
    initBufferFP();
    initDirectory();
}

void initPartition(void)
{
    partit.block_size = 0;
    partit.total_blocks = 0;
}
void initFAT(void)
{
    for (int i = 0; i < FAT_SIZE; i++)
    {
        if (fat[i].filename)
            free(fat[i].filename);
        fat[i].filename = NULL;
        fat[i].file_length = 0;
        fat[i].current_location = -1;
        for (int j = 0; j < FILE_SIZE; j++)
        {
            fat[i].blockPtrs[j] = 0;
        }
        fat[i].fppointer = -1;
    }
}
static void initBufferFP(void)
{
    if (block_buffer)
        free(block_buffer);
    block_buffer = NULL;
    for (int i = 0; i < OPEN_FILE_SIZE; i++)
    {
        fp[i] = NULL;
    }
}
static void initDirectory(void)
{
    DIR *dir = opendir("PARTITION");
    if (dir)
    {
        system("exec rm -r PARTITION");
    }
    closedir(dir);
}

int partition(char *name, int blocksize, int totalblocks)
{
    int error = 1;
    DIR *dir = opendir("PARTITION");
    if (!dir)
    {
        system("mkdir PARTITION");
    }
    closedir(dir);

    if (strcmp(partit.name, name) != 0)
    {
        strcpy(partit.name, name);
    }

    char *filename = malloc(strlen("PARTITION/") + strlen(name) + 1);
    strcpy(filename, "PARTITION/");
    strcat(filename, name);

    FILE *file = fopen(filename, "w+");
    free(filename);
    if (file && totalblocks <= FILE_SIZE)
    {
        char word[256];
        DIR *part = opendir(partit.name);
        if (!part)
        {
            char *command = malloc(strlen("mkdir ") + strlen(partit.name) + 1);
            strcpy(command, "mkdir ");
            strcat(command, partit.name);
            system(command);
            free(command);
        }
        closedir(part);
        sprintf(word, "%d\n", blocksize);
        fputs(word, file);
        sprintf(word, "%d\n", totalblocks);
        fputs(word, file);

        for (int i = 0; i < FAT_SIZE; i++)
        {
            sprintf(word, "%s\n", fat[i].filename);
            fputs(word, file);
            sprintf(word, "%d\n", fat[i].file_length);
            fputs(word, file);
            sprintf(word, "%d\n", fat[i].current_location);
            fputs(word, file);

            for (int j = 0; j < FILE_SIZE; j++)
            {
                sprintf(word, "%d\n", fat[i].blockPtrs[j]);
                fputs(word, file);
            }
        }

        for (int i = 0; i < totalblocks * blocksize; i++)
        {
            fputc('0', file);
        }
        fputs("\n", file);
        fclose(file);
    }
    else
    {
        error = 0;
    }
    return error;
}

int mount(char *name)
{
    int error = 1;
    char *filename = malloc(strlen("PARTITION/") + strlen(name) + 1);
    strcpy(filename, "PARTITION/");
    strcat(filename, name);
    FILE *file = fopen(filename, "r");
    free(filename);

    if (file)
    {
        char reader[256];

        fgets(reader, 256, file);
        partit.block_size = atoi(strtok(reader, "\n"));
        fgets(reader, 256, file);
        partit.total_blocks = atoi(strtok(reader, "\n"));

        for (int i = 0; i < FAT_SIZE; i++)
        {
            fgets(reader, 256, file);
            if (strcmp(reader, "(null)\n") == 0)
            {
                fat[i].filename = NULL;
            }
            else
            {
                strcpy(fat[i].filename, strtok(reader, "\n"));
            }

            fgets(reader, 256, file);
            fat[i].file_length = atoi(strtok(reader, "\n"));

            fgets(reader, 256, file);
            fat[i].current_location = atoi(strtok(reader, "\n"));

            for (int j = 0; j < FILE_SIZE; j++)
            {
                fgets(reader, 256, file);
                fat[i].blockPtrs[j] = atoi(strtok(reader, "\n"));
            }
        }
        block_buffer = malloc(partit.block_size);
        fclose(file);
    }
    else
    {
        error = 0;
    }
    return error;
}

int openfile(char *name)
{
    for (int i = 0; i < FAT_SIZE; i++)
    {
        if (fat[i].filename != NULL)
        {
            if (strcmp(name, fat[i].filename) == 0)
            {
                for (int j = 0; j < OPEN_FILE_SIZE; j++)
                {
                    if (fp[j] == NULL)
                    {
                        char *filename = malloc(strlen(partit.name) + strlen("/") + strlen(name) + 1);
                        strcpy(filename, partit.name);
                        strcat(filename, "/");
                        strcat(filename, name);
                        fp[j] = fopen(filename, "r+");
                        fat[i].fppointer = j;
                        fat[i].current_location = 0;
                        free(filename);
                        return i;
                    }
                }
                return -1;
            }
        }
    }
}

int loadFile(char *name)
{
    for (int i = 0; i < FAT_SIZE; i++)
    {
        if (fat[i].filename == NULL)
        {
            char *filename = malloc(strlen(partit.name) + strlen("/") + strlen(name) + 1);
            strcpy(filename, partit.name);
            strcat(filename, "/");
            strcat(filename, name);
            fclose(fopen(filename, "w+"));
            fat[i].filename = malloc(strlen(name) + 1);
            strcpy(fat[i].filename, name);
            free(filename);

            return i;
        }
    }
    return -1;
}

int readBlock(int file)
{
    if (file != -1 && file < FAT_SIZE)
    {
        memset(block_buffer, 0, partit.block_size);
        int fpEntry = fat[file].fppointer;
        fseek(fp[fpEntry], fat[file].blockPtrs[fat[file].current_location], SEEK_SET);

        int bytes_read = fread(block_buffer, partit.block_size, 1, fp[fpEntry]);
        partition(partit.name, partit.block_size, partit.total_blocks);
        if (bytes_read == 1)
        {
            fat[file].current_location++;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return -1;
    }
    return 1;
}

char *returnBlock(void)
{
    return block_buffer;
}

int writeBlock(int file, char **data)
{
    if (file != -1)
    {
        int fpEntry = fat[file].fppointer;
        fseek(fp[fpEntry], fat[file].blockPtrs[fat[file].current_location], SEEK_SET);
        int data_length = strlen(*data);
        if (data_length < partit.block_size)
        {
            fat[file].file_length += data_length;
            fwrite(*data, strlen(*data), 1, fp[fpEntry]);
            return 0;
        }
        else
        {
            fwrite(*data, partit.block_size, 1, fp[fpEntry]);
            fat[file].file_length += partit.block_size;
            int index = fat[file].file_length / partit.block_size;
            if (index < FILE_SIZE)
            {
                fat[file].blockPtrs[index] = fat[file].blockPtrs[index - 1] + partit.block_size;
                fat[file].current_location++;
            }
            *data = (char *)*data + partit.block_size;
            partition(partit.name, partit.block_size, partit.total_blocks);
        }
    }
    else
    {
        return -1;
    }
    return 1;
}

int isOpen(int id)
{
    if (id != -1 && id < FAT_SIZE)
    {
        return fat[id].current_location != -1;
    }
    return 0;
}

int closeFile(int id)
{
    int fpEntry = fat[id].fppointer;
    fat[id].current_location = -1;
    partition(partit.name, partit.block_size, partit.total_blocks);
    fclose(fp[fpEntry]);
    fp[fpEntry] = NULL;
}
