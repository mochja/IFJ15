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

char* concat(const char *str1 , const char *str2)
{
    char *result_str;

    const int str1_len = length(str1);
    const int str2_len = length(str2);

    result_str = malloc((str1_len + str2_len + 1) * sizeof(char));

    if (result_str == NULL) {
        // TODO: proper error handling
        exit(1);
    }

    memcpy(result_str, str1, str1_len);
    memcpy(result_str + str1_len, str2, str2_len + 1);

    return result_str;
}


