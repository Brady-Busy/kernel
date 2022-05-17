#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <io.h>
#include <process.h>
#include <string.h>
#include <system.h>

void bar(){
  printf("got to bar\n");
  int i = 0;
  while(1){
    printf("bar: %d\n", i++);
    if (i > 2000) break;
  }
    
  printf("finished bar\n");
  exit(1);
}

void foo(){
  printf("got to foo, got %p\n", bar);
  thread_t* tar = malloc(sizeof(thread_t));
  create_thread(tar, "bar", bar, NULL);

  int i = 0;
  while(1){
    printf("foo: %d\n", i++);
    if (i > 2000) break;
  }
    
  printf("finished foo\n");
  exit(1);
}

void _start() {
  thread_t* thread = malloc(sizeof(thread_t));
  create_thread(thread, "foo", foo, NULL);
  int i = 0;
  while(1){
    printf("main: %d\n", i++);
    if (i > 2000) break;
  }
    
  printf("finished main\n");
  exit(1);
}