#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <memory.h>
#include <limits.h>
#include <linux/limits.h>
#include "tokenizer.h"
#include "internal_command.h"
#include "utils.h"

#define MAX_PROCESS_CALL 16

CommandPool commandPool = { 0 };

void RunCommand(const CommandPool* commandPool)
{
    int prevInput = STDIN_FILENO;
    int pipeDesc[2] = { 1 };

    int savedStdin = dup(STDIN_FILENO);
    int savedStdout = dup(STDOUT_FILENO);
    int savedStderr = dup(STDERR_FILENO);

    for(int i = 0 ; i < commandPool->size ; i++)
    {
        bool failed = false;

        Command* curCommand = commandPool->commandPool[i];
        const char* command = curCommand->command;

        bool runBackground = commandPool->background;
        bool isInternalCommand = IsInternalCommand(command);
        int pid = 1;

        // Create Pipe
        if (commandPool->size > 1)
        {
            pipe(pipeDesc);
        }

        if (runBackground || !isInternalCommand)
        {
            pid = fork();
        }

        if (pid < 0)
        {
            fprintf(stderr, "Failed to run command with errno : %d\n", errno);
        }

        if (pid == 0 || (isInternalCommand && !runBackground))
        {
            if (commandPool->size > 1)
            {
                if (prevInput != STDIN_FILENO)
                {
                    dup2(prevInput, 0);
                    close(prevInput);
                }
                if (pipeDesc[1] != STDOUT_FILENO && i != (commandPool->size - 1))
                {
                    dup2(pipeDesc[1], STDOUT_FILENO);
                    close(pipeDesc[1]);
                }
            }

            for (int i = 0 ; i < curCommand->inputRedirectionCount ; i++)
            {
                int fd = OpenFile(&curCommand->inputRedirectionFiles[i]);

                if (fd < 0)
                {
                    failed = true;
                    break;
                }

                if (dup2(fd, STDIN_FILENO) < 0)
                {
                    failed = true;
                    fprintf(stderr, "Failed to direction with errno : %d\n", errno);
                    break;
                }
                close(fd);
            }

            for (int i = 0 ; i < curCommand->outputRedirectionCount ; i++)
            {
                int fd = OpenFile(&curCommand->outputRedirectionFiles[i]);

                if (fd < 0)
                {
                    failed = true;
                    break;
                }

                if (dup2(fd, STDOUT_FILENO) < 0)
                {
                    failed = true;
                    fprintf(stderr, "Failed to direction with errno : %d\n", errno);
                    break;
                }
                close(fd);
            }

            if (!failed)
            {
                if (isInternalCommand)
                {
                    if (!RunInternalCommand(curCommand))
                    {
                        fprintf(stderr, "Failed to run internal command\n");
                    }
                }
                else if (command != NULL)
                {
                    char** args = (char**)malloc(sizeof(char*) * (curCommand->flagsSize + 2));

                    args[0] = (char*)command;
                    for (int i = 0 ; i < curCommand->flagsSize ; i++)
                    {
                        args[i + 1] = curCommand->flags[i];
                    }
                    args[curCommand->flagsSize + 1] = NULL;

                    execvp(command, args);
                    fprintf(stderr, "Failed run command with errno : %d\n", errno);
                }
            }

            fflush(stdout);
        }

        dup2(savedStdin, STDIN_FILENO);
        dup2(savedStdout, STDOUT_FILENO);
        dup2(savedStderr, STDERR_FILENO);

        if (commandPool->size > 1)
        {
            prevInput = pipeDesc[0];
            close(pipeDesc[1]);
        }

        if (pid == 0)
        {
            exit(EXIT_SUCCESS);
        }
        else
        {
            if (commandPool->background)
            {
                /** code **/
            }
            else
            {
                int status = -1;
                wait(&status);
            }
        }
    }
}

int main(int argc, char* argv[])
{
    char cwd[PATH_MAX] = {0};
    const char* currentUser = getlogin();

    char line[PATH_MAX] = {0};

    if (currentUser == NULL)
    {
        fprintf(stderr, "Failed to find login user");
        return 1;
    }

    InitalizeCommandPool(&commandPool, MAX_PROCESS_CALL);

    while(true)
    {
        if (getcwd(cwd, PATH_MAX) == NULL)
        {
            fprintf(stderr, "Failed to find current folder with errno : %d\n", errno);
            return 1;
        }

        printf("%s %s $", currentUser, cwd);
        fgets(line, PATH_MAX - 1, stdin);

        Tokenize(line, &commandPool);

        RunCommand(&commandPool);

        // PrintCommandPool(&commandPool);
        ClearCommandPool(&commandPool);
    }

    ReleaseCommandPool(&commandPool);

    return 0;
}