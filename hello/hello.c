#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <io.h>
#include <process.h>
#include <string.h>
#include <system.h>

char change[] = "aaa";

int letterSma = 0;
int letterBig = 0;
size_t letter_counts[26] = {0};
size_t letter_countsB[26] = {0};

void bar(char* xtr){
  printf("got to bar\n");

  // Wait
  for(int c = 0; c < 10000; c++){
  }

  // Count number of capital letters
  for(int x = 0; x < strlen(xtr); x++){
    if (xtr[x] >= 'A' && xtr[x] <= 'Z') {
      letterBig++;
    }
  }

  // Report capital letters
  printf("counted a total of %d upper-case letters\n", letterBig);
  printf("now breakdown\n");
  
  // Find number of each capital letter
  char ch = 'A';
  for (int x = 0; x < strlen(xtr); x++){
    if (xtr[x] >= 'A' && xtr[x] <= 'Z') {
      letter_countsB[xtr[x] - 'A']++;
    }
  }
  
  // Print out capital letters and frequency if they were found
  for(int j = 0; j<26; j++){
    if(letter_countsB[j] == 0){
      ch++;
    }
    else{
      printf("%c: %d   ", ch, letter_countsB[j]);
      ch++;
    }
  }
  printf("\nfinished bar\n");

  // Since we have "finished", stop here
  while(1);
}

void foo(char* count){
  printf("got to foo\n");

  // Make space for and create a thread which will run bar
  thread_t* tar = malloc(sizeof(thread_t));
  create_thread(tar, "bar", bar, count);

  // Delay
  for (int i= 0; i < 2000050000; i++);

  // Count number of lowercase letters
  for(int x = 0; x < strlen(count); x++){
    if (count[x] >= 'a' && count[x] <= 'z') {
      letterSma++;
    }
  }

  // Report lowercase letters
  printf("counted a total of %d lower-case letters\n", letterSma);
  printf("now breakdown\n");

  // Find number of each lowercase letter
  char ch = 'a';
  for (int x = 0; x < strlen(count); x++){
    if (count[x] >= 'a' && count[x] <= 'z') {
      letter_counts[count[x] - 'a']++;
    }
  }
  
  // Print out lowercase letters and frequency if they were found
  for(int j = 0; j<26; j++){
    if(letter_counts[j] == 0){
      ch++;
    }
    else{
      printf("%c: %d   ", ch, letter_counts[j]);
      ch++;
    }
  }

  printf("\nfinished foo\n");
  while(1);
}

void _start() {
  printf("we are in main\n");
  char * testFile2 = "A sunny day, a cold day, not sure how the day's weather will turn out.It seems I can never be dressed right need to sleep need to get this demo to WORK AHHHHHH. Ok hopefully get reaal shit done";
  printf("Testing string is \"%s\"\n", testFile2);
  thread_t* bar = malloc(sizeof(thread_t));
  create_thread(bar, "foo", foo, testFile2);

  // Wait
  for (int i= 0; i < 2000000000; i++);

  // Wait with progress
  for(int i = 0; i < 500; i++){
    for(int j = 0; j < 100000; j++){
      if (j == 999&& i % 100 == 0){
        printf("Main at %d of 500\n",i);
      }
    }
  }
  printf("finished main\n");
  exit(1);
}