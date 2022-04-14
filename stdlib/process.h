#pragma once

#include "../kernel/kprocess.h"

int exec (const char *program, const char *argv[]);

int exit (int e_code);