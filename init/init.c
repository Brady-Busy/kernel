#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <io.h>
#include <process.h>
#include <string.h>
#include <system.h>



void take(){
  int a = 1;
  while(1){
    printf("%d\n", a);
    a++;
    }
}

void foo(){
  thread_t* tar = malloc(sizeof(thread_t));
  create_thread(tar, "take", take, NULL);
  int b = 10000;
  while(1){printf("%d\n",b);b++;}
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
  int c = 100000;
  while(1){printf("%c\n",c);c++;}
  exit(1);
}
