#pragma once
#include <stddef.h>
#include <system.h>

// Given two strings, compare them
// Return 0 if the strings are not equivalent, 1 if they are
int strcmp (const char* str1, const char* str2);

// Given a string
// Return the length of that string
int strlen (char * str);

// Given a string and a delimiter
// Return the corresponding token (chunks of code divided by delimiters)
char * strtok (char *str, const char * delim);

// Given a destination and a source, copy a string
// Return a pointer to the destination
char * strcpy (char * dest, const char * src);

// Given two strings
// Return the first address of the string s2 in string s1, if no instances, return NULL
char * strpbrk (const char * s1, const char * s2);

// Given a string
// Return the correspondinginteger value, if not an integer, return 0
int atoi (const char * str);

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 * Found here: https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
 * Adapted and commented by yours truly
 */
// Given a value, a pointer for where to write the result, and a base
// Convert the value to its base representation and write this to result
// Return the pointer to the location of the representation
char* itoa(uint64_t value, char* result, int base);