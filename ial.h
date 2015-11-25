#ifndef IAL_H_

#define IAL_H_
#define length(x) strlen(x)

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* substr(char* input, int start, int size);
char* concat(char* str1 , char* str2);
int find(char*, char*);
char* sort(char*);

#endif  // IAL_H_
