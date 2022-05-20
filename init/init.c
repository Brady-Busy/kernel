#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <io.h>
#include <process.h>
#include <string.h>
#include <system.h>

void _start() {
  printf("$");
  char * buf = malloc(80);
  getline(buf, 80, 0);
  // exec the specified file from the user
  // exec will handle invalid file names internally
  exec(strtok(buf, " "), NULL);
  exit(1);
}