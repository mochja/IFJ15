/**
 * Copyright (c) 2015, Team Unknown,
 *                     Ján Mochňak,    <xmochn00@vutbr.cz>
 *                     Tibor Dudlák,   <xdudla00@vutbr.cz>
 *                     Dávid Prexta,   <xprext00@vutbr.cz>
 *                     Martin Krajňák, <xkrajn02@vutbr.cz>
 *                     Patrik Segedy,  <xseged00@vutbr.cz>
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * license.txt file in the root directory of this source tree.
 */

#ifndef GEN1_H_
#define GEN1_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.h"
#include "vm.h"
#include "globals.h"



INLINED result_t process_ADD_pop_instr(stack_t *stack) {
    zval_t val;

    zval_t *b_ptr = &kv_pop(stack->data);
    zval_t *a_ptr = &kv_pop(stack->data);

if (ZVAL_IS_INT(b_ptr) && ZVAL_IS_INT(a_ptr)) {
        int b = ZVAL_GET_INT(b_ptr);

        int a = ZVAL_GET_INT(a_ptr);
        int res = a + b;
        zval_set(&val, res);
    } else if (ZVAL_IS_DOUBLE(b_ptr) && ZVAL_IS_DOUBLE(a_ptr)) {
        double b = ZVAL_GET_DOUBLE(b_ptr);

        double a = ZVAL_GET_DOUBLE(a_ptr);
        double res = a + b;
        ZVAL_SET_DOUBLE(&val, res);
    }

    kv_push(zval_t, stack->data, val);
    return EOK;
}

INSTR_T create_ADD_pop_instr() {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_ADD_pop;
    return i;
}




INLINED result_t process_ADDI_int_instr(stack_t *stack, const int a, const int b) {
    zval_t val;

    int res = a + b;
    zval_set(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}

INLINED result_t process_ADDI_int_pop_instr(stack_t *stack, const int a) {
    zval_t val;
    int b = ZVAL_GET_INT(&kv_pop(stack->data));

    int res = a + b;
    zval_set(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}

INLINED result_t process_ADDI_pop_int_instr(stack_t *stack, const int b) {
    zval_t val;

    int a = ZVAL_GET_INT(&kv_pop(stack->data));
    int res = a + b;
    zval_set(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}



INSTR_T create_ADDI_int_instr(const int a, const int b) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_ADDI_int;
    ZVAL_INIT_INT(i->first, a);
    ZVAL_INIT_INT(i->second, b);

    return i;
}

INSTR_T create_ADDI_pop_int_instr(const int b) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_ADDI_pop_int;
    ZVAL_INIT_INT(i->first, b);

    return i;
}

INSTR_T create_ADDI_int_pop_instr(const int a) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_ADDI_int_pop;
    ZVAL_INIT_INT(i->first, a);

    return i;
}


// static __attribute__ ((__unused__)) char *print_ADDI_int_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %d %d", "ADDI_int", ZVAL_GET_INT(i->first), ZVAL_GET_INT(i->second));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_ADDI_pop_int_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %d", "ADDI_int", ZVAL_GET_INT(i->first));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_ADDI_int_pop_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %d", "ADDI_int", ZVAL_GET_INT(i->first));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_ADDI_pop_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s", "ADDI_int");
//     return str;
// }




INLINED result_t process_ADDD_double_instr(stack_t *stack, const double a, const double b) {
    zval_t val;

    double res = a + b;
    ZVAL_SET_DOUBLE(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}

INLINED result_t process_ADDD_double_pop_instr(stack_t *stack, const double a) {
    zval_t val;
    double b = ZVAL_GET_DOUBLE(&kv_pop(stack->data));

    double res = a + b;
    ZVAL_SET_DOUBLE(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}

INLINED result_t process_ADDD_pop_double_instr(stack_t *stack, const double b) {
    zval_t val;

    double a = ZVAL_GET_DOUBLE(&kv_pop(stack->data));
    double res = a + b;
    ZVAL_SET_DOUBLE(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}



INSTR_T create_ADDD_double_instr(const double a, const double b) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_ADDD_double;
    ZVAL_INIT_DOUBLE(i->first, a);
    ZVAL_INIT_DOUBLE(i->second, b);

    return i;
}

INSTR_T create_ADDD_pop_double_instr(const double b) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_ADDD_pop_double;
    ZVAL_INIT_DOUBLE(i->first, b);

    return i;
}

INSTR_T create_ADDD_double_pop_instr(const double a) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_ADDD_double_pop;
    ZVAL_INIT_DOUBLE(i->first, a);

    return i;
}


// static __attribute__ ((__unused__)) char *print_ADDD_double_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %f %f", "ADDD_double", ZVAL_GET_DOUBLE(i->first), ZVAL_GET_DOUBLE(i->second));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_ADDD_pop_double_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %f", "ADDD_double", ZVAL_GET_DOUBLE(i->first));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_ADDD_double_pop_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %f", "ADDD_double", ZVAL_GET_DOUBLE(i->first));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_ADDD_pop_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s", "ADDD_double");
//     return str;
// }



INLINED result_t process_SUB_pop_instr(stack_t *stack) {
    zval_t val;

    zval_t *b_ptr = &kv_pop(stack->data);
    zval_t *a_ptr = &kv_pop(stack->data);

if (ZVAL_IS_INT(b_ptr) && ZVAL_IS_INT(a_ptr)) {
        int b = ZVAL_GET_INT(b_ptr);

        int a = ZVAL_GET_INT(a_ptr);
        int res = a - b;
        zval_set(&val, res);
    } else if (ZVAL_IS_DOUBLE(b_ptr) && ZVAL_IS_DOUBLE(a_ptr)) {
        double b = ZVAL_GET_DOUBLE(b_ptr);

        double a = ZVAL_GET_DOUBLE(a_ptr);
        double res = a - b;
        ZVAL_SET_DOUBLE(&val, res);
    }

    kv_push(zval_t, stack->data, val);
    return EOK;
}

INSTR_T create_SUB_pop_instr() {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_SUB_pop;
    return i;
}




INLINED result_t process_SUBI_int_instr(stack_t *stack, const int a, const int b) {
    zval_t val;

    int res = a - b;
    zval_set(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}

INLINED result_t process_SUBI_int_pop_instr(stack_t *stack, const int a) {
    zval_t val;
    int b = ZVAL_GET_INT(&kv_pop(stack->data));

    int res = a - b;
    zval_set(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}

INLINED result_t process_SUBI_pop_int_instr(stack_t *stack, const int b) {
    zval_t val;

    int a = ZVAL_GET_INT(&kv_pop(stack->data));
    int res = a - b;
    zval_set(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}



INSTR_T create_SUBI_int_instr(const int a, const int b) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_SUBI_int;
    ZVAL_INIT_INT(i->first, a);
    ZVAL_INIT_INT(i->second, b);

    return i;
}

INSTR_T create_SUBI_pop_int_instr(const int b) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_SUBI_pop_int;
    ZVAL_INIT_INT(i->first, b);

    return i;
}

INSTR_T create_SUBI_int_pop_instr(const int a) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_SUBI_int_pop;
    ZVAL_INIT_INT(i->first, a);

    return i;
}


// static __attribute__ ((__unused__)) char *print_SUBI_int_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %d %d", "SUBI_int", ZVAL_GET_INT(i->first), ZVAL_GET_INT(i->second));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_SUBI_pop_int_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %d", "SUBI_int", ZVAL_GET_INT(i->first));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_SUBI_int_pop_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %d", "SUBI_int", ZVAL_GET_INT(i->first));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_SUBI_pop_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s", "SUBI_int");
//     return str;
// }




INLINED result_t process_SUBD_double_instr(stack_t *stack, const double a, const double b) {
    zval_t val;

    double res = a - b;
    ZVAL_SET_DOUBLE(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}

INLINED result_t process_SUBD_double_pop_instr(stack_t *stack, const double a) {
    zval_t val;
    double b = ZVAL_GET_DOUBLE(&kv_pop(stack->data));

    double res = a - b;
    ZVAL_SET_DOUBLE(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}

INLINED result_t process_SUBD_pop_double_instr(stack_t *stack, const double b) {
    zval_t val;

    double a = ZVAL_GET_DOUBLE(&kv_pop(stack->data));
    double res = a - b;
    ZVAL_SET_DOUBLE(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}



INSTR_T create_SUBD_double_instr(const double a, const double b) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_SUBD_double;
    ZVAL_INIT_DOUBLE(i->first, a);
    ZVAL_INIT_DOUBLE(i->second, b);

    return i;
}

INSTR_T create_SUBD_pop_double_instr(const double b) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_SUBD_pop_double;
    ZVAL_INIT_DOUBLE(i->first, b);

    return i;
}

INSTR_T create_SUBD_double_pop_instr(const double a) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_SUBD_double_pop;
    ZVAL_INIT_DOUBLE(i->first, a);

    return i;
}


// static __attribute__ ((__unused__)) char *print_SUBD_double_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %f %f", "SUBD_double", ZVAL_GET_DOUBLE(i->first), ZVAL_GET_DOUBLE(i->second));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_SUBD_pop_double_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %f", "SUBD_double", ZVAL_GET_DOUBLE(i->first));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_SUBD_double_pop_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %f", "SUBD_double", ZVAL_GET_DOUBLE(i->first));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_SUBD_pop_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s", "SUBD_double");
//     return str;
// }



INLINED result_t process_MUL_pop_instr(stack_t *stack) {
    zval_t val;

    zval_t *b_ptr = &kv_pop(stack->data);
    zval_t *a_ptr = &kv_pop(stack->data);

if (ZVAL_IS_INT(b_ptr) && ZVAL_IS_INT(a_ptr)) {
        int b = ZVAL_GET_INT(b_ptr);

        int a = ZVAL_GET_INT(a_ptr);
        int res = a * b;
        zval_set(&val, res);
    } else if (ZVAL_IS_DOUBLE(b_ptr) && ZVAL_IS_DOUBLE(a_ptr)) {
        double b = ZVAL_GET_DOUBLE(b_ptr);

        double a = ZVAL_GET_DOUBLE(a_ptr);
        double res = a * b;
        ZVAL_SET_DOUBLE(&val, res);
    }

    kv_push(zval_t, stack->data, val);
    return EOK;
}

INSTR_T create_MUL_pop_instr() {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_MUL_pop;
    return i;
}




INLINED result_t process_MULI_int_instr(stack_t *stack, const int a, const int b) {
    zval_t val;

    int res = a * b;
    zval_set(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}

INLINED result_t process_MULI_int_pop_instr(stack_t *stack, const int a) {
    zval_t val;
    int b = ZVAL_GET_INT(&kv_pop(stack->data));

    int res = a * b;
    zval_set(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}

INLINED result_t process_MULI_pop_int_instr(stack_t *stack, const int b) {
    zval_t val;

    int a = ZVAL_GET_INT(&kv_pop(stack->data));
    int res = a * b;
    zval_set(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}



INSTR_T create_MULI_int_instr(const int a, const int b) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_MULI_int;
    ZVAL_INIT_INT(i->first, a);
    ZVAL_INIT_INT(i->second, b);

    return i;
}

INSTR_T create_MULI_pop_int_instr(const int b) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_MULI_pop_int;
    ZVAL_INIT_INT(i->first, b);

    return i;
}

INSTR_T create_MULI_int_pop_instr(const int a) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_MULI_int_pop;
    ZVAL_INIT_INT(i->first, a);

    return i;
}


// static __attribute__ ((__unused__)) char *print_MULI_int_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %d %d", "MULI_int", ZVAL_GET_INT(i->first), ZVAL_GET_INT(i->second));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_MULI_pop_int_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %d", "MULI_int", ZVAL_GET_INT(i->first));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_MULI_int_pop_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %d", "MULI_int", ZVAL_GET_INT(i->first));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_MULI_pop_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s", "MULI_int");
//     return str;
// }




INLINED result_t process_MULD_double_instr(stack_t *stack, const double a, const double b) {
    zval_t val;

    double res = a * b;
    ZVAL_SET_DOUBLE(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}

INLINED result_t process_MULD_double_pop_instr(stack_t *stack, const double a) {
    zval_t val;
    double b = ZVAL_GET_DOUBLE(&kv_pop(stack->data));

    double res = a * b;
    ZVAL_SET_DOUBLE(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}

INLINED result_t process_MULD_pop_double_instr(stack_t *stack, const double b) {
    zval_t val;

    double a = ZVAL_GET_DOUBLE(&kv_pop(stack->data));
    double res = a * b;
    ZVAL_SET_DOUBLE(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}



INSTR_T create_MULD_double_instr(const double a, const double b) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_MULD_double;
    ZVAL_INIT_DOUBLE(i->first, a);
    ZVAL_INIT_DOUBLE(i->second, b);

    return i;
}

INSTR_T create_MULD_pop_double_instr(const double b) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_MULD_pop_double;
    ZVAL_INIT_DOUBLE(i->first, b);

    return i;
}

INSTR_T create_MULD_double_pop_instr(const double a) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_MULD_double_pop;
    ZVAL_INIT_DOUBLE(i->first, a);

    return i;
}


// static __attribute__ ((__unused__)) char *print_MULD_double_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %f %f", "MULD_double", ZVAL_GET_DOUBLE(i->first), ZVAL_GET_DOUBLE(i->second));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_MULD_pop_double_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %f", "MULD_double", ZVAL_GET_DOUBLE(i->first));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_MULD_double_pop_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %f", "MULD_double", ZVAL_GET_DOUBLE(i->first));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_MULD_pop_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s", "MULD_double");
//     return str;
// }



INLINED result_t process_DIV_pop_instr(stack_t *stack) {
    zval_t val;

    zval_t *b_ptr = &kv_pop(stack->data);
    zval_t *a_ptr = &kv_pop(stack->data);

if (ZVAL_IS_INT(b_ptr) && ZVAL_IS_INT(a_ptr)) {
        int b = ZVAL_GET_INT(b_ptr);
        if (b == 0) return ESYS;
        int a = ZVAL_GET_INT(a_ptr);
        int res = a / b;
        zval_set(&val, res);
    } else if (ZVAL_IS_DOUBLE(b_ptr) && ZVAL_IS_DOUBLE(a_ptr)) {
        double b = ZVAL_GET_DOUBLE(b_ptr);
        if (b == 0) return ESYS;
        double a = ZVAL_GET_DOUBLE(a_ptr);
        double res = a / b;
        ZVAL_SET_DOUBLE(&val, res);
    }

    kv_push(zval_t, stack->data, val);
    return EOK;
}

INSTR_T create_DIV_pop_instr() {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_DIV_pop;
    return i;
}




INLINED result_t process_DIVI_int_instr(stack_t *stack, const int a, const int b) {
    zval_t val;
    if (b == 0) return ESYS;
    int res = a / b;
    zval_set(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}

INLINED result_t process_DIVI_int_pop_instr(stack_t *stack, const int a) {
    zval_t val;
    int b = ZVAL_GET_INT(&kv_pop(stack->data));
    if (b == 0) return ESYS;
    int res = a / b;
    zval_set(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}

INLINED result_t process_DIVI_pop_int_instr(stack_t *stack, const int b) {
    zval_t val;
    if (b == 0) return ESYS;
    int a = ZVAL_GET_INT(&kv_pop(stack->data));
    int res = a / b;
    zval_set(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}



INSTR_T create_DIVI_int_instr(const int a, const int b) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_DIVI_int;
    ZVAL_INIT_INT(i->first, a);
    ZVAL_INIT_INT(i->second, b);

    return i;
}

INSTR_T create_DIVI_pop_int_instr(const int b) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_DIVI_pop_int;
    ZVAL_INIT_INT(i->first, b);

    return i;
}

INSTR_T create_DIVI_int_pop_instr(const int a) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_DIVI_int_pop;
    ZVAL_INIT_INT(i->first, a);

    return i;
}


// static __attribute__ ((__unused__)) char *print_DIVI_int_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %d %d", "DIVI_int", ZVAL_GET_INT(i->first), ZVAL_GET_INT(i->second));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_DIVI_pop_int_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %d", "DIVI_int", ZVAL_GET_INT(i->first));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_DIVI_int_pop_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %d", "DIVI_int", ZVAL_GET_INT(i->first));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_DIVI_pop_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s", "DIVI_int");
//     return str;
// }




INLINED result_t process_DIVD_double_instr(stack_t *stack, const double a, const double b) {
    zval_t val;
    if (b == 0) return ESYS;
    double res = a / b;
    ZVAL_SET_DOUBLE(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}

INLINED result_t process_DIVD_double_pop_instr(stack_t *stack, const double a) {
    zval_t val;
    double b = ZVAL_GET_DOUBLE(&kv_pop(stack->data));
    if (b == 0) return ESYS;
    double res = a / b;
    ZVAL_SET_DOUBLE(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}

INLINED result_t process_DIVD_pop_double_instr(stack_t *stack, const double b) {
    zval_t val;
    if (b == 0) return ESYS;
    double a = ZVAL_GET_DOUBLE(&kv_pop(stack->data));
    double res = a / b;
    ZVAL_SET_DOUBLE(&val, res);
    kv_push(zval_t, stack->data, val);
    return EOK;
}



INSTR_T create_DIVD_double_instr(const double a, const double b) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_DIVD_double;
    ZVAL_INIT_DOUBLE(i->first, a);
    ZVAL_INIT_DOUBLE(i->second, b);

    return i;
}

INSTR_T create_DIVD_pop_double_instr(const double b) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_DIVD_pop_double;
    ZVAL_INIT_DOUBLE(i->first, b);

    return i;
}

INSTR_T create_DIVD_double_pop_instr(const double a) {
    instruction_t *i = calloc(1, sizeof(instruction_t));

    if (i == NULL) return NULL;

    i->type = I_DIVD_double_pop;
    ZVAL_INIT_DOUBLE(i->first, a);

    return i;
}


// static __attribute__ ((__unused__)) char *print_DIVD_double_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %f %f", "DIVD_double", ZVAL_GET_DOUBLE(i->first), ZVAL_GET_DOUBLE(i->second));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_DIVD_pop_double_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %f", "DIVD_double", ZVAL_GET_DOUBLE(i->first));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_DIVD_double_pop_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s %f", "DIVD_double", ZVAL_GET_DOUBLE(i->first));
//     return str;
// }

// static __attribute__ ((__unused__)) char *print_DIVD_pop_instr(instruction_t *i) {
//     char *str = malloc(64 * sizeof(char));
//     sprintf(str, "%s", "DIVD_double");
//     return str;
// }



#endif
