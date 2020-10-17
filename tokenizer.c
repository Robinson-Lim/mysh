#include "tokenizer.h"
#include "utils.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

void InitalizeCommand(Command* command)
{
    memset(command, 0, sizeof(Command));
}

void FreeCommand(Command* command)
{
    if (command->command != NULL)
    {
        free(command->command);
    }

    if (command->flags != NULL)
    {
        for(int i = 0 ; i < command->flagsSize ; i++)
        {
            if (command->flags[i] != NULL)
            {
                free(command->flags[i]);
            }
        }
        free(command->flags);
    }

    if (command->inputRedirectionFiles != NULL)
    {
        for (int i = 0 ; i < command->inputRedirectionCount ; i++)
        {
            if (command->inputRedirectionFiles[i].filepath != NULL)
            {
                free(command->inputRedirectionFiles[i].filepath);
            }
        }
        free(command->inputRedirectionFiles);
    }

    if (command->outputRedirectionFiles != NULL)
    {
        for (int i = 0 ; i < command->outputRedirectionCount ; i++)
        {
            if (command->outputRedirectionFiles[i].filepath != NULL)
            {
                free(command->outputRedirectionFiles[i].filepath);
            }
        }
        free(command->outputRedirectionFiles);
    }

    free(command);
}

void InsertOutputRedirectionFile(Command* command, const char* filepath, int flags, mode_t mode)
{
    char* pathClone = StrClone(filepath);

    if (command->outputRedirectionCount == 0)
    {
        command->outputRedirectionFiles = (File*)malloc(sizeof(File));
    }
    else
    {
        command->outputRedirectionFiles = (File*)realloc(command->outputRedirectionFiles, sizeof(File) * (command->outputRedirectionCount + 1));
    }

    command->outputRedirectionFiles[command->outputRedirectionCount].filepath = pathClone;
    command->outputRedirectionFiles[command->outputRedirectionCount].flags = flags;
    command->outputRedirectionFiles[command->outputRedirectionCount].mode = mode;

    command->outputRedirectionCount++;
}

void InsertInputRedirectionFile(Command* command, const char* filepath, int flags, mode_t mode)
{
    char* pathClone = StrClone(filepath);

    if (command->inputRedirectionCount == 0)
    {
        command->inputRedirectionFiles = (File*)malloc(sizeof(File));
    }
    else
    {
        command->inputRedirectionFiles = (File*)realloc(command->inputRedirectionFiles, sizeof(File) * (command->inputRedirectionCount + 1));
    }

    command->inputRedirectionFiles[command->inputRedirectionCount].filepath = pathClone;
    command->inputRedirectionFiles[command->inputRedirectionCount].flags = flags;
    command->inputRedirectionFiles[command->inputRedirectionCount].mode = mode;
    
    command->inputRedirectionCount++;
}

void SetCommand(Command* command, const char* commandStr)
{
    char* commandClone = StrClone(commandStr);

    if (command->command != NULL)
    {
        free(command->command);
    }

    command->command = commandClone;
}

void PushArgument(Command* command, const char* argument)
{
    char* argClone = StrClone(argument);

    if (command->flagsSize == 0)
    {
        command->flags = (char**)malloc(sizeof(char*) * 2);
    }
    else
    {
        command->flags = (char**)realloc(command->flags, sizeof(char*) * (command->flagsSize + 2));
    }

    command->flags[command->flagsSize] = argClone;
    command->flags[command->flagsSize + 1] = NULL;
    command->flagsSize++;
}

void InitalizeCommandPool(CommandPool* commandPool, int capacity)
{
    commandPool->capacity = capacity;
    commandPool->curPos = 0;
    commandPool->size = 0;
    commandPool->background = false;

    commandPool->commandPool = (Command**)malloc(sizeof(Command*) * capacity);
    memset(commandPool->commandPool, 0, sizeof(Command*) * capacity);
}

void ClearCommandPool(CommandPool* commandPool)
{
    for (int i = 0 ; i < commandPool->size ; i++)
    {
        if (commandPool->commandPool[i] != NULL)
        {
            FreeCommand(commandPool->commandPool[i]);
        }
    }

    memset(commandPool, 0, sizeof(Command*) * commandPool->capacity);

    commandPool->size = 0;
    commandPool->curPos = 0;
}

void ReleaseCommandPool(CommandPool* commandPool)
{
    for (int i = 0 ; i < commandPool->size ; i++)
    {
        if (commandPool->commandPool[i] != NULL)
        {
            FreeCommand(commandPool->commandPool[i]);
        }
    }

    free(commandPool->commandPool);
}

int InsertCommand(CommandPool* commandPool, Command* command)
{
    if (commandPool->size < commandPool->capacity)
    {
        commandPool->commandPool[commandPool->size] = command;
        commandPool->size++;
    }
    else
    {
        return -1;
    }

    return 0;
}

int Tokenize(const char* line, CommandPool* commandPool)
{
    char* lineClone = (char*)malloc(sizeof(char) * strlen(line));
    strcpy(lineClone, line);
    lineClone = trim(lineClone);

    // Tokenize by |(pipe)
    char** tmpPipeTokens = (char**)malloc(sizeof(char*) * commandPool->capacity);
    for (char* pipeToken = strtok(lineClone, "|") ; pipeToken != NULL ; pipeToken = strtok(NULL, "|"))
    {
        Command* newCommand = (Command*)malloc(sizeof(Command));
        InitalizeCommand(newCommand);

        char* tmpPipeToken = (char*)malloc(sizeof(char) * strlen(pipeToken));
        strcpy(tmpPipeToken, pipeToken);
        tmpPipeTokens[commandPool->size] = tmpPipeToken;
        
        // Create Command object
        int result = InsertCommand(commandPool, newCommand);
        if (result < 0)
        {
            fprintf(stderr, "Faild to insert command to command pool\n");
        }
    }

    for (int tokenIdx = 0 ; tokenIdx < commandPool->size ; tokenIdx++)
    {
        char* commandToken = NULL;
        bool redirectTo = false;
        bool redirectBy = false;
        bool appendMode = false;

        for (commandToken = strtok(tmpPipeTokens[tokenIdx], " ") ; commandToken != NULL ; commandToken = strtok(NULL, " "))
        {
            commandToken = trim(commandToken);

            Command* curCommand = commandPool->commandPool[tokenIdx];
            int flags = O_CREAT;
            mode_t mode = S_IRWXU;

            if (redirectTo)
            {
                if (appendMode)
                {
                    flags |= O_APPEND;

                    appendMode = false;
                }

                InsertOutputRedirectionFile(curCommand, commandToken, flags, mode);

                redirectTo = false;

                continue;
            }
            else if (redirectBy)
            {   
                InsertInputRedirectionFile(curCommand, commandToken, flags, mode);

                redirectBy = false;

                continue;
            }
            
            if (strcmp(commandToken, ">") == 0)
            {
                redirectTo = true;
            }
            else if (strcmp(commandToken, "<") == 0)
            {
                redirectBy = true;
            }
            else if(strcmp(commandToken, ">>") == 0)
            {
                redirectTo = true;
                appendMode = true;
            }
            else
            {
                if (curCommand->command == NULL)
                {
                    SetCommand(curCommand, commandToken);
                }
                else
                {
                    PushArgument(curCommand, commandToken);
                }
            }
        }
    }


    // Insert to command pool

    free(lineClone);

    for (int i = 0 ; i < commandPool->size; i++)
    {
        free(tmpPipeTokens[i]);
    }
    free(tmpPipeTokens);

    return 0;
}