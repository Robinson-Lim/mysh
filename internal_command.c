#include "internal_command.h"
#include <string.h>
#include <unistd.h>
#include <errno.h>

char* INTERNAL_COMMAND[] = {
    "help",
    "cd",
    "ps",
    "pwd",
    "date",
    "whoami",
    "ls"
};

bool IsInternalCommand(const char* command)
{
    for(int i = 0; i < (sizeof(INTERNAL_COMMAND) / sizeof(INTERNAL_COMMAND[0])) ; i++)
    {   
        if (strcmp(INTERNAL_COMMAND[i], command) == 0)
        {
            return true;
        }
    }

    return false;
}

bool RunInternalCommand(Command* command)
{
    const char* commandStr = command->command;

    if (strcmp(commandStr, "help") == 0)
    {
        return Help();
    }
    else if(strcmp(commandStr, "cd") == 0)
    {
        return ChangeDirectory(command);
    }

    return false;
}

bool Help()
{
    printf("=== help commands ===\n");
    printf("help : print internal commands\n");
    printf("cd : change directory\n");
    printf("ls : list directory\n");
    printf("ps : process status\n");
    printf("pwd : current password\n");
    printf("date : print current date\n");
    printf("whoami : print current user\n");
    printf("=== ============= ===\n");

    return true;
}

bool ChangeDirectory(Command* command)
{
    if (command->flagsSize == 0) return true;

    if (chdir(command->flags[0]) < 0)
    {
        fprintf(stderr, "Failed to change directory with errno : %d\n", errno);
        
        return false;
    }

    return true;
}