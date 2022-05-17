#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <io.h>
#include <process.h>
#include <string.h>
#include <system.h>

void bar(){
  /*
  int a = 1;
  printf("got to bar\n");
  while(1){
    /*
    if (a % 1000 == 2){
      printf("%d\n", a);
    }
    a++;
    }*/
  printf("got to bar\n");
  int i = 0;
  while(1){
    printf("bar: %d\n", i++);
    if (i > 5000) break;
  }
    
  printf("finished bar\n");
  exit(1);
}

void foo(){
  printf("got to foo, got %p\n", bar);
  /*
  thread_t* tar = malloc(sizeof(thread_t));
  create_thread(tar, "bar", bar, NULL);
  int b = 1;
  printf("got to foo\n");
  while(1){/*if(b%1000 == 1){printf("%d\n",b);}b++;}*/
  thread_t* tar = malloc(sizeof(thread_t));
  create_thread(tar, "bar", bar, NULL);

  int i = 0;
  while(1){
    printf("foo: %d\n", i++);
    if (i > 5000) break;
  }
    
  printf("finished foo\n");
  exit(1);
}

void _start() {
  printf("$\n");
  //char * buf = malloc(80);
  //getline(buf, 80, 0);
  // exec the specified file from the user
  // exec will handle invalid file names internally
  //exec(strtok(buf, " "), NULL);
  thread_t* thread = malloc(sizeof(thread_t));
  create_thread(thread, "foo", foo, NULL);
  int c = 1;
  for (int i= 0; i < 2000000000; i++);
  /*while(1){if(!c%1000){printf("%d, %p\n",c, foo);}c++;}*/
  int i = 0;
  while(1){
    printf("main: %d\n", i++);
    if (i > 5000) break;
  }
    
  printf("finished main\n");
  exit(1);
}