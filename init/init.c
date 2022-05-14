#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <io.h>
#include <process.h>
#include <string.h>
#include <system.h>



void take(){
  /*
  int a = 1;
  printf("got to take\n");
  while(1){
    /*
    if (a % 1000 == 2){
      printf("%d\n", a);
    }
    a++;
    }*/
  printf("got to take\n");
  for(int c = 0; c < 10; c++){
    for(int d = 0; d < 1000; d++);
  }
    
  printf("finished take\n");
  while(1);
}

void foo(){
  printf("got to foo\n");
  /*
  thread_t* tar = malloc(sizeof(thread_t));
  create_thread(tar, "take", take, NULL);
  int b = 1;
  printf("got to foo\n");
  while(1){/*if(b%1000 == 1){printf("%d\n",b);}b++;}*/
  thread_t* tar = malloc(sizeof(thread_t));
  create_thread(tar, "take", take, NULL);
  
  for(int a = 0; a < 50; a++){
    for(int b = 0; b < 1000; b++);
  }
    
  printf("finished foo\n");
  while(1);
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
  int c = 1;
  /*while(1){if(!c%1000){printf("%d, %p\n",c, foo);}c++;}*/
  printf("we are in main\n");
  for(int i = 0; i < 9000000000; i++){
    for(int j = 0; j < 1000000000; j++);
  }
    
  printf("finished main\n");
  while(1);
  exit(1);
}
