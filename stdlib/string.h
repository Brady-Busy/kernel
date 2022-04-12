#pragma once
#include <stddef.h>

int strcmp(const char* str1, const char* str2);

int strlen(char * str);

char * strtok(char *str, const char * delim);

// TODO How are strtok and strsep different

char * strcpy(char * dest, const char * src);

char * strpbrk(const char * s1, const char * s2);

int atoi(const char * str);