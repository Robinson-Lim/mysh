#include "internal_command.h"
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <errno.h>
#include <time.h>

char* INTERNAL_COMMAND[] = {
    "help",
    "cd",
    "pwd",
    "date",
    "whoami"
};

bool IsInternalCommand(const char* command)
{
    if (command == NULL) return false;
    
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
    else if(strcmp(commandStr, "pwd") == 0)
    {
        return PWD();
    }
    else if(strcmp(commandStr, "date") == 0)
    {
        return Date();
    }
    else if(strcmp(commandStr, "whoami") == 0)
    {
        return WhoamI();
    }

    return false;
}

bool Help()
{
    printf("=== help commands ===\n");
    printf("help : print internal commands\n");
    printf("cd : change directory\n");
    printf("ls : list directory\n");
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

bool PWD()
{
    char cwd[PATH_MAX] = {0};

    if (getcwd(cwd, PATH_MAX) == NULL)
    {
        fprintf(stderr, "Failed to find current folder with errno : %d\n", errno);
        return false;
    }

    printf("%s\n", cwd);

    return true;
}

bool Date()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("현재 시각 : %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    return true;
}

bool WhoamI()
{
    printf("현재 유저 : %s\n", getlogin());

    return true;
}