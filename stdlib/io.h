#pragma once

#include "../kernel/kprocess.h"
#include "../kernel/thread.h"
#include <string.h>

// Given the descriptor for where to read, buffer to store input and size, read input
// Return the number of characters read. Return -1 if an invalid descriptor for where to read is provided
// In our implementation, the only standard location is stdin (0)
int read (int fd, char const *buf, int size);

// Given the descriptor for where to write, pointer to what should be written and size, write input
// Return the number of characters written
// In our implmentation, permissable locations for where to write are stdout (1) and stderror (2)
int write (int fd, const char *buf, int size);

// Given the buffer to write to, size and where to read from
// Return the number of characters processed
// Note fd is not relevant in our implementation since all reads are from stdin
size_t getline (char * buf, size_t sz, char * fd);

// Placeholder
void perror(const char* str);

// Given a formatted string, print accordingly
void printf(const char* format, ...);