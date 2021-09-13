#ifndef MAMAN14_STRINGEXTENSION_H
#define MAMAN14_STRINGEXTENSION_H

#include "common.h"
#include "assert.h"

char *duplicateStr(char *str);

char *myStrsep(char **string, const char *delim);

char *trimStr(char *string);

char** strSplit(char* str, const char del);

void freeStringArray(char **str);
#endif

