#include "ial.h"

/*
 * Returns a substring [pos, pos+count). If the requested substring
 * extends past the end of the string, or if count == npos, the returned substring is [pos, size()).
 */
char* substr(const char *str, int pos, int count)
{
    char *result_str;
    const int str_len = length(str);

    if (pos < 0 || count < 0 || pos > str_len) {
        // TODO: proper error handling
        exit(1);
    }

    const int result_len = (count > str_len - pos) ? str_len - pos : count;
    result_str = malloc((result_len + 1) * sizeof(char));

    if (result_str == NULL) {
        // TODO: proper error handling
        exit(1);
    }

    if (pos < str_len) {
        memcpy(result_str, str + pos, result_len);
    }

    result_str[result_len] = '\0';

    return result_str;
}


char* concat(char *str1 , char *str2)
{
    char *result_str;

    const int result_len = length(str1) + length(str2);

    result_str = malloc((result_len + 1) * sizeof(char));

    if (result_str == NULL) {
        // TODO: proper error handling
        exit(1);
    }

    for (int i = 0; i < result_len; i++) {
        if ( i < length(str1) ) {
            result_str[i] = str1[i];
        } else {
            result_str[i] = str2[i - length(str1)];
        }
    }

    result_str[result_len] = '\0';

    return result_str;
}

