#include "process.h"

//Wrappers for system calls to exec and exit
int exec (const char *program, const char *argv[]){
  return syscall(SYS_exec, program, argv);
}

int exit (int e_code){
  return syscall(SYS_exit, e_code);
}

int pthread_c (thread_t * memory, const char * name, uintptr_t func, void * args){
  return create_thread(memory, name, func, args);
}