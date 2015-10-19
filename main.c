#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#ifndef XtOffsetOf
#ifdef offsetof
#define XtOffsetOf(s_type, field) offsetof(s_type, field)
#else
#define XtOffsetOf(s_type, field) XtOffset(s_type*, field)
#endif
#endif /* !XtOffsetOf */

#include "ial.h"
#include "tu_str.h"

int main()
{
    tu_string_t* str = tu_string_init("testasdfasdfasdfasdfasdfasdf", sizeof("testasdfasdfasdfasdfasdfasdf") - 1);

    printf("%s (%llu)\n", TU_STR_VAL(str), TU_STR_LEN(str));

    tu_string_free(str);

	return length("test") == 4;
}
