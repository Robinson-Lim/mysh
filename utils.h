#ifndef __UTILS_H__
#define __UTILS_H__

#include "tokenizer.h"

char* trim(char *s);
char* rtrim(char *s);
char* ltrim(char *s);

char* StrClone(const char* s);
int OpenFile(const File* file);

#endif