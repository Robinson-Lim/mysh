/**
 * Reference by    https://stackoverflow.com/questions/656542/trim-a-string-in-c 
*/
#include "utils.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

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