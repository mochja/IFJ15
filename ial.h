#ifndef IAL_H_
#define IAL_H_

#include <stdlib.h>
#include <string.h>

#define length(x) strlen(x)

char* substr(const char *str, int pos, int count);
char* concat(const char *str1 , const char *str2);
int find(char*, char*);
char* sort(char*);

#endif  // IAL_H_