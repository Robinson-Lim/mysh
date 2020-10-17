#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <sys/types.h>
#include <stdbool.h>

typedef struct file
{
    char* filepath;
    int flags;
    mode_t mode;
} File;

typedef struct command
{
    char* command;
    
    char** flags;
    int flagsSize;
    
    File* inputRedirectionFiles;
    int inputRedirectionCount;

    File* outputRedirectionFiles;
    int outputRedirectionCount;
} Command;

typedef struct command_pool
{
    Command** commandPool;
    int capacity;
    int size;
    int curPos;
    bool background;
} CommandPool;

void InitalizeCommand(Command* command);
void FreeCommand(Command* command);

void InsertOutputRedirectionFile(Command* command, const char* filepath, int flags, mode_t mode);
void InsertInputRedirectionFile(Command* command, const char* filepath, int flags, mode_t mode);

void SetCommand(Command* command, const char* commandStr);
void PushArgument(Command* command, const char* argument);

void InitalizeCommandPool(CommandPool* commandPool, int capacity);
void ClearCommandPool(CommandPool* commandPool);
void ReleaseCommandPool(CommandPool* commandPool);
int InsertCommand(CommandPool* commandPool, Command* command);

int Tokenize(const char* line, CommandPool* commandPool);

#endif