#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tokenizer.h"


/**
 * Reference by    https://stackoverflow.com/questions/656542/trim-a-string-in-c 
*/
char* ltrim(char *s)
{
    while(isspace(*s)) 
    {
        s++;
    }

    return s;
}

char* rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';

    return s;
}

char* trim(char *s)
{
    return rtrim(ltrim(s)); 
}

char* StrClone(const char* s)
{
    int len = strlen(s);
    char* clone = (char*)malloc(sizeof(char) * len);
    strcpy(clone, s);

    return clone;
}

int OpenFile(const File* file)
{
    int fd = open(file->filepath, file->flags, file->mode);
    if (fd < 0)
    {
        fprintf(stderr,"Failed to open redirection file with errno : %d\n", errno);
        
        return fd;
    }

    return fd;
}