#pragma once

#include "../kernel/kprocess.h"

// Given the name of a program and its arguments, load and run the executable
// Return 1 upon success and 0 for failure
// Note: in our implementation, we do not pass the provided arguments
int exec (const char *program, const char *argv[]);

// Loads and executes the init program (shell), thus exiting from the current program
// Returns 0 upon success (although this will not actually end up returning as it launches init within it)
int exit (int e_code);