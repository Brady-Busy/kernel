#pragma once
#include <stddef.h>
#include <system.h>

int strcmp(const char* str1, const char* str2);

int strlen(char * str);

char * strtok(char *str, const char * delim);

// TODO How are strtok and strsep different

char * strcpy(char * dest, const char * src);

char * strpbrk(const char * s1, const char * s2);

int atoi(const char * str);

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 * Found here: https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
 * Adapted and commented by yours truly
 */
char* itoa(uint64_t value, char* result, int base);