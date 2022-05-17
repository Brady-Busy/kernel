// #include <stdint.h>
// #include <stddef.h>
// #include <stdbool.h>
// #include <io.h>
// #include <process.h>
// #include <string.h>
// #include <system.h>

// char change[] = "aaa";

// //char* testFile= "A sunny day, a cold day, not sure how the day's weather will turn out. It seems I can never be dressed right need to sleep need to get this demo to WORK AHHHHHH. Ok hopefully get reaal shit done";

// //l
// int letterSma = 0;
// int letterBig = 0;
// size_t letter_counts[26] = {0};
// size_t letter_countsB[26] = {0};

// void bar(char* xtr){
//   /*
//   int a = 1;
//   printf("got to bar\n");
//   while(1){
//     /*
//     if (a % 1000 == 2){
//       printf("%d\n", a);
//     }
//     a++;
//     }*/
//   printf("got to bar\n");
//   for(int c = 0; c < 10; c++){
//     for(int d = 0; d < 1000; d++);
//   }
  
//   for (int i= 0; i < 2000000000; i++);

//   printf("counted a total of %d upper-case letters\n", letterBig);
//   printf("now breakdown\n");
//   char ch = 'A';
//   for (int x = 0; x < strlen(xtr); x++){
//     if (xtr[x] >= 'A' && xtr[x] <= 'Z') {
//       letter_countsB[xtr[x] - 'A']++;
//     }
//   }
  
//   for(int j = 0; j<26; j++){
//     if(letter_countsB[j] == 0){
//       ch++;
//     }
//     else{
//       printf("%c: %d   ", ch, letter_countsB[j]);
//       ch++;
//     }
//   }
  

//   printf("\nfinished bar\n");
//   while(1);
//   exit(1);
  
// }

// void foo(char* count){
//   printf("got to foo\n");
//   /*
//   thread_t* tar = malloc(sizeof(thread_t));
//   create_thread(tar, "bar", bar, NULL);
//   int b = 1;
//   printf("got to foo\n");
//   while(1){/*if(b%1000 == 1){printf("%d\n",b);}b++;}*/
//   thread_t* tar = malloc(sizeof(thread_t));
//   create_thread(tar, "bar", bar, count);
  


//   for (int i= 0; i < 2000000000; i++);

//   for(int a = 0; a < 50; a++){
//     for(int b = 0; b < 1000; b++);
//   }
//   for(int x = 0; x < strlen(count); x++){
//     if (count[x] >= 'a' && count[x] <= 'z') {
//       letterSma++;
//     }
//   }
//   printf("counted a total of %d lower-case letters\n", letterSma);
//   printf("now breakdown\n");
//   char ch = 'a';
//   for (int x = 0; x < strlen(count); x++){
//     if (count[x] >= 'a' && count[x] <= 'z') {
//       letter_counts[count[x] - 'a']++;
//     }
//   }
  
//   for(int j = 0; j<26; j++){
//     if(letter_counts[j] == 0){
//       ch++;
//     }
//     else{
//       printf("%c: %d   ", ch, letter_counts[j]);
//       ch++;
//     }
//   }

//   printf("\nfinished foo\n");
//   //exit(1);
//   while(1);
// }

// void _start() {
//   //printf("$\n");
//   //char * buf = malloc(80);
//   //getline(buf, 80, 0);
//   // exec the specified file from the user
//   // exec will handle invalid file names internally
//   //exec(strtok(buf, " "), NULL);
//   printf("we are in main\n");
//   char * testFile2 = "A sunny day, a cold day, not sure how the day's weather will turn out.It seems I can never be dressed right need to sleep need to get this demo to WORK AHHHHHH. Ok hopefully get reaal shit done";
//   //printf("%d\n",strlen(testFile2)+1);
//   // char * test = malloc (strlen(testFile2)+1);
//   //strcpy(test, testFile);
//   printf("Testing string is \"%s\"\n", testFile2);
//   thread_t* bar = malloc(sizeof(thread_t));
//   create_thread(bar, "foo", foo, testFile2);
//   int c = 1;
//   for (int i= 0; i < 2000000000; i++);
//   /*while(1){if(!c%1000){printf("%d, %p\n",c, foo);}c++;}*/
  
//   for(int i = 0; i < 500; i++){
//     for(int j = 0; j < 100000; j++){
//       if (j == 999&& i % 100 == 0){
//         printf("%d\n",i);
//       }
//     }
//   }
    
//   printf("finished main\n");
//   while(1);
//   //exit(1);
// }


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