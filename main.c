#include <stdio.h>
#include <unistd.h>
#include "tokenizer.h"

#define MAX_PROCESS_CALL 16

CommandPool commandPool = { 0 };

void Run()
{

}

int main(void)
{
    // char test[] = "head -a -b -c120 -d < test.txt > second.txt | wc -a -b | grep -a -b 1";
    char test[] = "head -a -b -c120 -d < test.txt > second.txt | wc -a -b | grep -a -b 1 &";
    // char test[] = "< test.txt > ttest.txt >> tttext.txt";

    InitalizeCommandPool(&commandPool, MAX_PROCESS_CALL);

    Tokenize(test, &commandPool);

    for(int i = 0 ; i < commandPool.size ; i++)
    {
        Command* command = commandPool.commandPool[i];

        printf("Command : %s\n", command->command);
        for (int f = 0 ; f < command->flagsSize ; f++)
        {
            printf("\tFlag : %s\n", command->flags[f]);
        }

        for (int f = 0 ; f < command->inputRedirectionCount ; f++)
        {
            printf("\t< Filename : %s\n", command->inputRedirectionFiles[f].filepath);
            printf("\t< flag : %d\n", command->inputRedirectionFiles[f].flags);
            printf("\t< mode : %d\n", command->inputRedirectionFiles[f].mode);
        }

        for (int f = 0 ; f < command->outputRedirectionCount ; f++)
        {
            printf("\t> Filename : %s\n", command->outputRedirectionFiles[f].filepath);
            printf("\t> flag : %d\n", command->outputRedirectionFiles[f].flags);
            printf("\t> mode : %d\n", command->outputRedirectionFiles[f].mode);
        }

        printf("\n");
    }
    printf("Run background : %d\n", commandPool.background);

    // ClearCommandPool(&commandPool);
    ReleaseCommandPool(&commandPool);

    return 0;
}