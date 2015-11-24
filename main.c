#include "ial.h"
#include <string.h>
int main()
{
    printf ("TEST 1 suppose to be 1 -> %d\n",length("test\0") == 4 );
    printf ("TEST 2 suppose to be 0 -> %d\n",length("test\0") == 5 );
    char* strng = substr("streptokok",6,5);
    printf ("TEST 3 suppose to be 1 -> %d\n",strcmp(strng,"tokok\0")==0 );
    printf ("TEST 4 suppose to be 0 -> %d\n",strcmp(strng,"stokok\0")==0 );
    free(strng);
    char* strng2 =concat("test", "pico");
    printf ("TEST 5 suppose to be 1 -> %d\n",strcmp(strng2,"testpico\0")==0 );
    printf ("TEST 6 suppose to be 0 -> %d\n",strcmp(strng2,"testpicko\0")==0 );
    free(strng2);
    return 0;
}
