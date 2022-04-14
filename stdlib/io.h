#pragma once

#include "../kernel/kprocess.h"
#include <string.h>

int read (int fd, char const *buf, int size);

int write (int fd, const char *buf, int size);

//TODO
//void printf(const char* format, ...);

size_t getline (char * buf, size_t sz, char * fd);

void perror(const char* str);