#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <io.h>
#include <process.h>
#include <string.h>
#include <system.h>



void take(){
  while(1){printf("sup\n");}
}

void foo(){
  thread_t* tar = malloc(sizeof(thread_t));
  create_thread(tar, "take", take, NULL);
  while(1){printf("hi\n");}
}

void _start() {
  printf("$\n");
  //char * buf = malloc(80);
  //getline(buf, 80, 0);
  // exec the specified file from the user
  // exec will handle invalid file names internally
  //exec(strtok(buf, " "), NULL);
  thread_t* bar = malloc(sizeof(thread_t));
  create_thread(bar, "foo", foo, NULL);
  while(1){printf("#\n");}
  exit(1);
}
