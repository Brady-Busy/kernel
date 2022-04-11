#include <stddef.h>

int strcmp(const char* str1, const char* str2){
  while (*str1 || *str2){
    if (*(str1++) != *(str2++)) return 0;
  }
  return 1;
}

//Given a string, return the length of the string
int strlen(char * str) {
  //Confirm str is not null
  if (str == NULL) {
    return 0;
  }
  //Count until null terminator
  int count = 0;
  char * tracer = str; //start at the beginning of the string
  while (*tracer != '\0') {
    count++;
    tracer++; // move the pointer
  }
  return count;
}

// Convert a string into an integer value, if no non-integer, return 0
int atoi(const char * str) {
    int result = 0; 
    int power = strlen(str);
    char * ptr = str;
    for (int i = 1; i <= power; i++) {
        if (ptr <= 57 && ptr >= 48) {
            // assuming decimal, 
            result += ((power - i) * 10 * (*ptr - 48));
        } else {
            return 0;
        }
    }
}

char * strcpy(char * dest, const char * src) {
    while(src++ != NULL) {
        *dest++ = *src++;
    }

    return dest;
}

char * strtok(char *str, const char * delim) {
    static char * cur_input;
    // Check if str is null
    if (str != NULL) {
        // update cur_input
        cur_input = str;
    }

    // all tokens extracted?
    if (cur_input == NULL) {
        return NULL;
    }

    // Going to store output here
    char * ret_token;
    
    for (int i = 0; i < strlen(cur_input); i++) {
        // iterate through and add to output token, as long as not delimiter
        if (cur_input[i] != delim) {
            // if not delim, update ret_token
            ret_token[i] = cur_input[i];
        } else {
            // if it is the delim, add a null terminator
            ret_token[i] = '\0';
            // update cur_input to point to the next spot
            cur_input = cur_input + i + 1;
            return ret_token;
        }
    }

    // If we're here, we have the token and just need to terminate and return
    ret_token[strlen(cur_input)] = '\0';
    cur_input = NULL;

    return ret_token;
}

char * strpbrk(const char * s1, const char * s2) {
    // Iterate through each char in s1 and compare with each in s2
    for (int i = 0; i < strlen(s1); i++) {
        char cur = s1[i];
        for (int j = 0; j < strlen(s2); j++) {
            if (cur = s2[j]) {
                // found the first match, return the address
                return s1 + i;
            }
        }
    }
    // If we make it to here, there are no matches
    return NULL;
}