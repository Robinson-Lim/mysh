#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include "tokenizer.h"
#include "internal_command.h"
#include "utils.h"

#ifndef PATH_MAX
#define PATH_MAX 2048
#endif

#define MAX_PROCESS_CALL 16

CommandPool commandPool = { 0 };

void RunCommand(const CommandPool* commandPool)
{
    for(int i = 0 ; i < commandPool->size ; i++)
    {
        bool failed = false;

        Command* curCommand = commandPool->commandPool[i];
        const char* command = curCommand->command;

        int* inputFds = NULL;
        int* outputFds = NULL;

        bool runBackground = commandPool->background;
        bool isInternalCommand = IsInternalCommand(command);
        int pid = 1;

        int savedStdin = dup(STDIN_FILENO);
        int savedStdout = dup(STDOUT_FILENO);
        int savedStderr = dup(STDERR_FILENO);

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
            if (curCommand->inputRedirectionCount > 0)
            {
                inputFds = (int*)malloc(sizeof(int) * curCommand->inputRedirectionCount);
            }

            if (curCommand->outputRedirectionCount > 0)
            {
                outputFds = (int*)malloc(sizeof(int) * curCommand->outputRedirectionCount);
            }

            for (int i = 0 ; i < curCommand->inputRedirectionCount ; i++)
            {
                int fd = OpenFile(&curCommand->inputRedirectionFiles[i]);

                if (fd < 0)
                {
                    failed = true;
                    break;
                }

                inputFds[i] = fd;
                if (dup2(fd, STDIN_FILENO) < 0)
                {
                    failed = true;
                    fprintf(stderr, "Failed to direction with errno : %d\n", errno);
                    break;
                }
            }

            for (int i = 0 ; i < curCommand->outputRedirectionCount ; i++)
            {
                int fd = OpenFile(&curCommand->outputRedirectionFiles[i]);

                if (fd < 0)
                {
                    failed = true;
                    break;
                }

                outputFds[i] = fd;
                if (dup2(fd, STDOUT_FILENO) < 0)
                {
                    failed = true;
                    fprintf(stderr, "Failed to direction with errno : %d\n", errno);
                    break;
                }
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
                else
                {
                    printf("External command\n");
                }
            }

            if (inputFds != NULL)
            {
                for (int i = 0 ; i < curCommand->inputRedirectionCount ; i++)
                {
                    int result = close(inputFds[i]);
                    if (result < 0)
                    {
                        fprintf(stderr, "Failed to close redirection file with errno : %d\n", errno);
                    }
                }

                free(inputFds);
            }
            
            if (outputFds != NULL)
            {
                for (int i = 0 ; i < curCommand->outputRedirectionCount ; i++)
                {
                    int result = close(outputFds[i]);
                    if (result < 0)
                    {
                        fprintf(stderr, "Failed to close redirection file with errno : %d\n", errno);
                    }
                }

                free(outputFds);
            }

            fflush(stdout);
        }

        if (pid == 0)
        {
            exit(EXIT_SUCCESS);
        }

        if (runBackground)
        {
            /** code **/
        }
        else if (!isInternalCommand)
        {
            wait(NULL);
        }
        else if (isInternalCommand)
        {
            dup2(savedStdin, STDIN_FILENO);
            dup2(savedStdout, STDOUT_FILENO);
            dup2(savedStderr, STDERR_FILENO);
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