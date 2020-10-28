#ifndef __INTERNAL_COMMAND_H__
#define __INTERNAL_COMMAND_H__

#include <stdio.h>
#include <stdbool.h>
#include "tokenizer.h"

extern char* INTERNAL_COMMAND[];

bool IsInternalCommand(const char* command);

bool RunInternalCommand(Command* command);

bool Help();
bool ChangeDirectory();
bool PWD();
bool Date();
bool WhoamI();

#endif