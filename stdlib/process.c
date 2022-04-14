#include "process.h"

//Wrappers for system calls to exec and exit
int exec (const char *program, const char *argv[]){
  return syscall(SYS_exec, program, argv);
}

int exit (int e_code){
  return syscall(SYS_exit, e_code);
}